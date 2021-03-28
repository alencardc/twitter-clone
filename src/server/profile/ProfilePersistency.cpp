#include "ProfilePersistency.hpp"

ProfilePersistency::ProfilePersistency() {}

ProfilePersistency::~ProfilePersistency() {}

std::unordered_map<std::string, User> ProfilePersistency::readUsers() {

  m_csvFile_r.open(m_filename, m_split);
  std::unordered_map<std::string, User> users;

  int userId = 1;
  auto usersRaw = m_csvFile_r.readAll();
  for (auto userRaw : usersRaw) {
    std::string username = userRaw[0];
    userRaw.erase(userRaw.begin());

    User user = User(userId, username);
    for (std::string follower : userRaw) {
      user.addFollower(follower);
    }
    users.emplace(username, user);
    userId += 1;

    printf("[DB] Loading user '%s'\n", username.c_str());    
  }

  m_csvFile_r.close();
  return users;
}

void ProfilePersistency::saveNewUser(std::string user){

  m_csvFile_w.open(m_filename, m_split);

  std::vector<std::string> myUser;
  myUser.push_back(user);

  m_csvFile_w.append(myUser);

  printf("[DB] Saving user '%s'\n", user.c_str());    
  m_csvFile_w.close();
}

void ProfilePersistency::saveNewFollower(std::string user, std::string follower){

  m_csvFile_w.open(m_filename, m_split);

  std::vector<std::string> followers;
  followers.push_back(follower);
  m_csvFile_w.appendToLine(0, user, followers);

  printf("[DB] Adding follow '%s' to user '%s'\n", user.c_str(), follower.c_str()); 
  m_csvFile_w.close();
}


