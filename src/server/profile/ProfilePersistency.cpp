#include "ProfilePersistency.hpp"
#include <iostream>

ProfilePersistency::ProfilePersistency(std::string filename, char splitter) : m_filename(filename), m_split(splitter) {}

ProfilePersistency::~ProfilePersistency() {}

void ProfilePersistency::readUsers(std::unordered_map<std::string, User>& users) {

  m_csvFile.open(m_filename, m_split);

  int currentLine = 0;
  auto line =     m_csvFile.readLine(currentLine++);
  while(m_csvFile.canRead()){
    auto username = line[0];
    //add_user
    auto myUser = new User(users.size(), username);
    users.emplace(username, *myUser);
    //logging
    std::cout << "[DB]: adding user: " + username << std::endl;

    line = m_csvFile.readLine(currentLine++);
  }

  m_csvFile.close();
}

void ProfilePersistency::readFollowers(std::unordered_map<std::string, User>& users) {

  m_csvFile.open(m_filename, m_split);

  int currentLine = 0;
  auto line =     m_csvFile.readLine(currentLine++);

  while(m_csvFile.canRead()) {
    auto username = line[0];

    // start following
    for(auto follower = line.begin()+1; follower != line.end(); follower++){
      users[username].addFollower(*follower);
      //logging
      std::cout << "[DB]: "<< *follower << " started following " << username << std::endl;
    }

    line = m_csvFile.readLine(currentLine++);
  }

  m_csvFile.close();
}
