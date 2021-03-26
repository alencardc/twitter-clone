#include "../src/server/profile/ProfilePersistency.hpp"
#include <unordered_map>
#include <iostream>
#include <string>
#include <utility>

int main(){

	std::unordered_map<std::string, User> myUsersMap;
	ProfilePersistency db("UsersFollowing.db");
	db.readUsers(myUsersMap);
	db.readFollowers(myUsersMap);

	return 0;
}
