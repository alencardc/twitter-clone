#ifndef _PROFILEPERSISTENCY_HPP
#define _PROFILEPERSISTENCY_HPP

#include <unordered_map>
#include <string>
#include "lib/csv/CSVReader.hpp"
#include "lib/csv/CSVWriter.hpp"
#include "User.hpp"

class ProfilePersistency{
  private:
    const std::string m_filename = "Users.db";
    const char m_split = ';';

    CSVReader m_csvFile_r;
    CSVWriter m_csvFile_w;
  public:
    ProfilePersistency();
    ~ProfilePersistency();

    void saveNewUser(std::string user);
    void saveNewFollower(std::string user, std::string follower);

    std::unordered_map<std::string, User> readUsers();
};

#endif //_PROFILEPERSISTENCY_HPP
