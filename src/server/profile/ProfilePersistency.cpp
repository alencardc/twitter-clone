#include "ProfilePersistency.hpp"
#include <iostream>

ProfilePersistency::ProfilePersistency(std::string filename, char splitter) : m_filename(filename), m_split(splitter) {}

ProfilePersistency::~ProfilePersistency() {}

void ProfilePersistency::readUsers(std::unordered_map<std::string, User>& users) {

  m_csvFile_r.open(m_filename, m_split);

  int currentLine = 0;
  auto line =     m_csvFile_r.readLine(currentLine++);
  while(m_csvFile_r.canRead()){
    auto username = line[0];
    //add_user
    auto myUser = new User(users.size(), username);
    users.emplace(username, *myUser);
    //logging
    std::cout << "[DB]: adding user: " + username << std::endl;

    line = m_csvFile_r.readLine(currentLine++);
  }

  m_csvFile_r.close();
}

void ProfilePersistency::readFollowers(std::unordered_map<std::string, User>& users) {

  m_csvFile_r.open(m_filename, m_split);

  int currentLine = 0;
  auto line =     m_csvFile_r.readLine(currentLine++);

  while(m_csvFile_r.canRead()) {
    auto username = line[0];

    // start following
    for(auto follower = line.begin()+1; follower != line.end(); follower++){
      users[username].addFollower(*follower);
      //logging
      std::cout << "[DB]: "<< *follower << " started following " << username << std::endl;
    }

    line = m_csvFile_r.readLine(currentLine++);
  }

  m_csvFile_r.close();
}

void ProfilePersistency::saveNewUser(std::string user){

  m_csvFile_w.open(m_filename, m_split);

  std::vector<std::string> myUser;
  myUser.push_back(user);

  m_csvFile_w.append(myUser);

  m_csvFile_w.close();
}

void ProfilePersistency::saveNewFollower(std::string user, std::string follower){

  m_csvFile_w.open(m_filename, m_split);

  //m_csvFile_w.appendToLine();

  m_csvFile_w.close();
}


