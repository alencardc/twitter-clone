#include "server/profile/ProfilePersistency.hpp"
#include <unordered_map>
#include <iostream>
#include <string>
#include <utility>

int main(){

  constexpr bool isHandMadeFile = false;

  std::unordered_map<std::string, User> myUsersMap;
  ProfilePersistency db;

  if(!isHandMadeFile) {
    std::string user1 = "matheus";
    std::string user2 = "gabriel";
    std::string user3 = "zika";
    db.saveNewUser(user1);
    db.saveNewUser(user2);
    db.saveNewUser(user3);
    db.saveNewFollower(user1, user2);
    db.saveNewFollower(user2, user3);
    db.saveNewFollower(user3, user2);
    db.saveNewFollower(user1, user3);
    std::cout << "Inserting new users...\n";
  }

  myUsersMap = db.readUsers();
  std::cout << "Size: " << myUsersMap.size() << std::endl;
  
  for(auto user: myUsersMap) {
    printf("%s -> ", user.second.username().c_str());
    for (auto follower : user.second.followers()) {
      printf("%s;", follower.c_str());
    }
    printf("\n");
  }

  return 0;
}
