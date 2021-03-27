#include "../src/server/profile/ProfilePersistency.hpp"
#include <unordered_map>
#include <iostream>
#include <string>
#include <utility>

int main(){

  constexpr bool isHandMadeFile = false;

  std::unordered_map<std::string, User> myUsersMap;
  ProfilePersistency db("UsersFollowing.db");

  if(!isHandMadeFile) {
    std::string user1 = "matheus";
    std::string user2 = "gabriel";
    std::string user3 = "zika";
    db.saveNewUser(user1);
    db.saveNewUser(user2);
    db.saveNewUser(user3);
    std::cout << "Inserting new users...\n";
  }

  db.readUsers(myUsersMap);
  db.readFollowers(myUsersMap);

  for(auto &i: myUsersMap)
    std::cout << i.first << " ";
  std::cout << std::endl;

  return 0;
}
