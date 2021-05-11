#ifndef __ReplicaManager_hpp__
#define __ReplicaManager_hpp__

#include <vector>

#include "lib/SyncAccess.hpp"
#include "lib/socket/TCPServer.hpp"
#include "server/replication/ReplicaInfo.hpp"
#include "server/profile/ProfileManager.hpp"
#include "server/notification/NotificationManager.hpp"

class ReplicaManager {
  ReplicaInfo m_info;
  const std::string m_ipServer;
  const int m_portServer;

  bool m_isLeader;
  std::string m_ipLeader;
  int m_portLeader;
  TCPConnection* m_leaderConn; // init with null
  
  ProfileManager m_profileManager;
  NotificationManager m_notificationManager;
  std::vector<std::pair<TCPConnection*, ReplicaInfo>> m_replicasVector;


  public:
    ReplicaManager(
      ReplicaInfo info,
      std::string ipServer,
      int portServer,
      bool isLeader,
      std::string ipLeader,
      int portLeader
    );

    // Init structures and call run method
    void start();

  private:
    // Run Replica Manager logic
    void run(TCPServer* serverRM);

    // Start handshake with RM leader
    // Method called by RM that is not the leader
    void runStartHandshake(
      SyncAccess<bool>& startElection, 
      SyncAccess<bool>& lostElection, 
      std::mutex& mutex, std::condition_variable& cv
    );

    // RM leader answer the new RM replica that arrives
    void answerStartHandshake(
      TCPConnection* conn, 
      SyncAccess<bool>& startElection, 
      SyncAccess<bool>& lostElection, 
      std::mutex& mutex, std::condition_variable& cv
    );

    // Send ELECTION message to replicas with id greater than current RM
    void sendElection(
      SyncAccess<bool>& lostElection,
      std::mutex& electionMutex, std::condition_variable& receivedAnswerCV
    );

    // Send COORDINATOR message to all replicas notifing the new leader
    void sendCoordinator();

    // Helper function to check if RM connection is closed
    static bool isClosed(std::pair<TCPConnection*, ReplicaInfo> pair);

    // Use isClosed() method to remove all connection that are closed
    void removeClosedConnections(
      std::vector<std::pair<TCPConnection*, ReplicaInfo>>& replicasVector
    );

    // Serialize vector of data containing id, ip and port of online RMs
    std::string serialize(std::vector<ReplicaInfo> replicas);

    // Deerialize vector of data containing id, ip and port of online RMs
    std::vector<ReplicaInfo> deserialize(std::string payload);

    // Send a NEW_REPLICA packet to the current leader RM to notify the
    // connection of a new replica
    void sendHello(TCPConnection* conn, ReplicaInfo info);

    // Method called by a new replica to receive the info of the current online RMs
    std::vector<ReplicaInfo> receiveState(TCPConnection* conn, ReplicaInfo& info);

    // Method called by the leader RM to wait for the NEW_REPLICA packet
    ReplicaInfo receiveHello(TCPConnection* conn);

    // Method called by the leader RM to send the info about the current online RMs.
    void sendState(
      TCPConnection* conn, 
      ReplicaInfo& replica,
      std::vector<ReplicaInfo> replicasInfo
    );

    // Callback called by ProfileManager to notify ReplicaManager of a new update
    bool sendProfileUpdate(ProfileManager& manager);

    // Callback called by ProfileManager to notify NotificationManager of a new update
    bool sendNotificationUpdate(NotificationManager& manager);

    // Method called after the election of a new leader to notify all front-ends
    void notifyFrontends();

};

#endif // __ReplicaManager_hpp__