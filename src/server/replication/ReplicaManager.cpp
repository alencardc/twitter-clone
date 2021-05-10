#include <mutex>
#include <algorithm>
#include <functional>
#include <condition_variable>

#include "server/replication/ReplicaManager.hpp"
#include "server/replication/rm/ReplicaHandler.hpp"
#include "lib/socket/TCPClient.hpp"
#include "lib/utils/vector_pair.hpp"
#include "server/ServerHandler.hpp"
#include "GlobalConfig.hpp"

ReplicaManager::ReplicaManager(
  ReplicaInfo info,
  std::string ipServer,
  int portServer,
  bool isLeader,
  std::string ipLeader,
  int portLeader
):  
  m_info(info),
  m_ipServer(ipServer),
  m_portServer(portServer),
  m_isLeader(isLeader),
  m_ipLeader(ipLeader),
  m_portLeader(portLeader),
  m_leaderConn(NULL)
{
  // Bind callbacks
  m_profileManager.onUpdateCallback = std::bind(
    &ReplicaManager::sendProfileUpdate,
    this,
    std::placeholders::_1
  );
  m_notificationManager.onUpdateCallback = std::bind(
    &ReplicaManager::sendNotificationUpdate,
    this,
    std::placeholders::_1
  );
}

void ReplicaManager::start() {
  if (m_isLeader == true) {
    m_profileManager.loadUsers();
    m_notificationManager.loadUsers(m_profileManager.getAllUsernames());
  }
  
  try {
    TCPServer* serverRM = new TCPServer(m_info.ip, m_info.port);
    if (serverRM->start() == false) {
      printf("[error] Unable to start the RM server!\n");
      return;
    }

    run(serverRM);
  } catch (std::bad_alloc& e) {
    printf("[error] bad_alloc caught: %s\n", e.what());
  }
}

void ReplicaManager::run(TCPServer* serverRM) {
  std::mutex electionMutex; // Used during election with the receivedAnswer condition variable
  std::condition_variable receivedAnswerCV; // Used to notify an ELECTION answer

  SyncAccess<bool> lostElection;
  lostElection.set(false);
  SyncAccess<bool> startElection;
  startElection.set(false);

  ServerHandler* notificationServer;
  while (true) {

    if (m_isLeader == true) {
      // Start notification server at DEFAULT_IP:DEFAULT_SERVER_PORT
      notificationServer = new ServerHandler(m_ipServer, m_portServer, m_profileManager, m_notificationManager);
      notificationServer->start();
    }

    if (m_isLeader == false && m_leaderConn == NULL) {
      TCPClient client;
      m_leaderConn = client.connect(m_ipLeader, m_portLeader);
      if (m_leaderConn == NULL)
        printf("[error] Unable to connect to leader RM server.\n");
      else
        runStartHandshake(startElection, lostElection, electionMutex, receivedAnswerCV);
    }

    while (startElection.get() == false) {
      TCPConnection* conn = serverRM->accept(800);
      if (conn != NULL) {
        printf("[Replica] Received a new connection\n");
        removeClosedConnections(m_replicasVector);
        answerStartHandshake(conn, startElection, lostElection, electionMutex, receivedAnswerCV);
      }
    }
    
    removeClosedConnections(m_replicasVector);

    // Do the election

    printf("I am going to send ELECTION to %ld replicas\n", m_replicasVector.size());
    sendElection(lostElection, electionMutex, receivedAnswerCV);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    // If I didn`t lost to any other replica, then I'm the new leader
    if (lostElection.get() == false) { 
      printf("[Replica] I'm the new leader!\n");
      sendCoordinator(); // Notify all connected replicas
      notifyFrontends();
      m_isLeader = true;
      delete m_leaderConn; m_leaderConn = NULL;
    }
    startElection.set(false);
    lostElection.set(false);
  }
}

void ReplicaManager::runStartHandshake(SyncAccess<bool>& startElection, SyncAccess<bool>& lostElection, std::mutex& mutex, std::condition_variable& cv) {
  sendHello(m_leaderConn, m_info);
  std::vector<ReplicaInfo> replicas = receiveState(m_leaderConn, m_info);
  
  ReplicaHandler* handler = new ReplicaHandler(
    m_leaderConn, m_info, m_profileManager, m_notificationManager,
    startElection, true, lostElection, mutex, cv);
  handler->start();

  // Start connection with online replicas
  for (ReplicaInfo replica : replicas) {
    printf("[Replica] Tried to connect to <%s>\n", replica.serialize().c_str());

    TCPClient client;
    TCPConnection* conn = client.connect(replica.ip, replica.port);
    if (conn != NULL) {
      printf("[Replica] Connected to <%s>\n", replica.serialize().c_str());
      sendHello(conn, m_info);
      m_replicasVector.push_back(std::make_pair(conn, replica));

      ReplicaHandler* handler = new ReplicaHandler(
        conn, m_info, m_profileManager, m_notificationManager,
        startElection, false, lostElection, mutex, cv);
      handler->start();
    }
  }
}


void ReplicaManager::answerStartHandshake(TCPConnection* conn, SyncAccess<bool>& startElection, SyncAccess<bool>& lostElection, std::mutex& mutex, std::condition_variable& cv) {
  ReplicaInfo newReplica = receiveHello(conn);
  if (newReplica.isValid()) {
    if (m_isLeader == true) {
      std::vector<ReplicaInfo> replicasInfo;
      extract_second(m_replicasVector, replicasInfo);
      sendState(conn, newReplica, replicasInfo);
      // Send PrfileManager and NotificationManager update
      Packet updatePacket1 = Packet(
        UPDATE_PROFILE, m_profileManager.serialize().c_str()
      );
      conn->send(&updatePacket1);
      Packet updatePacket2 = Packet(
        UPDATE_NOTIFICATION, m_notificationManager.serialize().c_str()
      );
      int size = conn->send(&updatePacket2);
      std::string packet = updatePacket2.serialize();
      printf("Size: %ld Sent: %d\n",packet.size(), size);
      printf("%s\n",packet.c_str());
    }

    m_replicasVector.push_back(std::make_pair(conn, newReplica));

    std::vector<ReplicaInfo> replicas;
    extract_second(m_replicasVector, replicas);
    printf("%s\n", serialize(replicas).c_str());

    ReplicaHandler* handler = new ReplicaHandler(
      conn, m_info, m_profileManager, m_notificationManager,
      startElection, false, lostElection, mutex, cv);
    handler->start();
  } else {
    printf("[Replica] Replica was invalid, closing connection...\n");
    delete conn; conn = NULL;
  }
}

void ReplicaManager::sendElection(SyncAccess<bool>& lostElection, std::mutex& electionMutex, std::condition_variable& receivedAnswerCV) {
  for (auto replicaPair : m_replicasVector) {
    TCPConnection* conn = replicaPair.first;
    ReplicaInfo replica = replicaPair.second;
    bool isNotEqualToMe = replica.ip != m_info.ip || replica.port != m_info.port;
    bool isGreaterThanMe = replica.id < m_info.id; // Greater id has less value

    if (isNotEqualToMe && isGreaterThanMe) {
      Packet electionPacket = Packet(ELECTION, m_info.serialize().c_str());
      conn->send(&electionPacket);

      std::unique_lock<std::mutex> lock(electionMutex);
      receivedAnswerCV.wait(lock);
      lock.unlock();

      if (lostElection.get() == true)
        break;
    }
  }
}

void ReplicaManager::sendCoordinator() {
  std::vector<TCPConnection*> connections;
  extract_first(m_replicasVector, connections);
  for (TCPConnection* conn : connections) {
    Packet coordPacket = Packet(COORDINATOR, m_info.serialize().c_str());
    conn->send(&coordPacket);
  }
}



bool ReplicaManager::isClosed(std::pair<TCPConnection*, ReplicaInfo> pair) {
  return pair.first->isClosed();
}

void ReplicaManager::removeClosedConnections(
  std::vector<std::pair<TCPConnection*, ReplicaInfo>>& replicasVector
) {
  replicasVector.erase(
    std::remove_if(replicasVector.begin(), replicasVector.end(), isClosed),
    replicasVector.end()
  );
}

std::string ReplicaManager::serialize(std::vector<ReplicaInfo> replicas) {
  std::string serial;
  if (replicas.size() == 0)
    return "";

  serial.append(replicas[0].serialize());
  for (unsigned i = 1; i < replicas.size(); i++) {
    serial.append("\n" + replicas[i].serialize());
  }

  return serial;
}

std::vector<ReplicaInfo> ReplicaManager::deserialize(std::string payload) {
  std::vector<ReplicaInfo> replicas;
  std::vector<std::string> splits = split(payload, "\n");
  for (std::string raw : splits)
    replicas.push_back(ReplicaInfo::deserialize(raw.c_str()));
  
  return replicas;
}

void ReplicaManager::sendHello(TCPConnection* conn, ReplicaInfo info) {
  Packet helloPacket = Packet(NEW_REPLICA, info.serialize().c_str());
  conn->send(&helloPacket);
}

std::vector<ReplicaInfo> ReplicaManager::receiveState(TCPConnection* conn, ReplicaInfo& info) {
  std::vector<ReplicaInfo> replicas;

  Packet* answerPacket = conn->receive();
  if (answerPacket != NULL) {
    std::string payload = answerPacket->payload();
    if (payload.length() > 0) {
      std::vector<std::string> idAndReplicas = splitFirst(payload, "\n");

      info.id = atoi(idAndReplicas[0].c_str());
      if (idAndReplicas[1].size() > 0)
        replicas  = deserialize(idAndReplicas[1]);
    }
  }

  return replicas;
}

ReplicaInfo ReplicaManager::receiveHello(TCPConnection* conn) {
  ReplicaInfo replica;
  Packet* helloPacket = conn->receive();
  if (helloPacket != NULL && helloPacket->type() == NEW_REPLICA) {
    replica = ReplicaInfo::deserialize(helloPacket->payload());
  }
  delete helloPacket;
  return replica;
}


void ReplicaManager::sendState(
  TCPConnection* conn, 
  ReplicaInfo& replica,
  std::vector<ReplicaInfo> replicasInfo
) {
  replica.id = replicasInfo.size();

  std::string payload = std::to_string(replica.id) + "\n" + serialize(replicasInfo); 
  Packet statePacket = Packet(REPLICAS, payload.c_str());
  conn->send(&statePacket);
}

bool ReplicaManager::sendProfileUpdate(ProfileManager& manager) {

  printf("[Replica] Send PROFILE update!\n");
  for (auto replicaPair : m_replicasVector) {
    TCPConnection* conn = replicaPair.first;
    ReplicaInfo replica = replicaPair.second;
    bool isNotEqualToMe = replica.ip != m_info.ip || replica.port != m_info.port;

    if (isNotEqualToMe) {
      Packet updatePacket = Packet(
        UPDATE_PROFILE, m_profileManager.serialize().c_str()
      );
      conn->send(&updatePacket);
    }
  }

  return true;
}

bool ReplicaManager::sendNotificationUpdate(NotificationManager& manager) {

  printf("[Replica] Send NOTIFICATION update!\n");
  for (auto replicaPair : m_replicasVector) {
    TCPConnection* conn = replicaPair.first;
    ReplicaInfo replica = replicaPair.second;
    bool isNotEqualToMe = replica.ip != m_info.ip || replica.port != m_info.port;

    if (isNotEqualToMe) {
      Packet updatePacket = Packet(
        UPDATE_NOTIFICATION, m_notificationManager.serialize().c_str()
      );
      conn->send(&updatePacket);
    }
  }

  return true;
}

void ReplicaManager::notifyFrontends() {
  std::string payload = m_info.ip + " " + std::to_string(m_info.port);
  Packet packet = Packet(NEW_LEADER, payload.c_str());
  for (auto addr : GlobalConfig::FRONTEND_ADDRESSES) {
    TCPClient client;
    TCPConnection* conn = client.connect(addr.ip, addr.port);
    if (conn != NULL) {
      conn->send(&packet);
      delete conn; conn = NULL;
    }
  }
}