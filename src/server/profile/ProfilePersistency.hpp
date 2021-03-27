#ifndef _PROFILEPERSISTENCY_HPP
#define _PROFILEPERSISTENCY_HPP
#include "../../lib/csv/CSVReader.hpp"
//#include "lib/csv/CSVWriter.hpp"
#include "User.hpp"
#include <unordered_map>
#include <string>

class ProfilePersistency{
	private:
		std::string m_filename;
                char m_split;
		CSVReader m_csvFile;
	public:
		ProfilePersistency(std::string filename = "UsersFollowing.db", char splitter=';');
		~ProfilePersistency();

		//void saveNewUser();
		//void saveNewFollower();

		void readFollowers(std::unordered_map<std::string, User>& users);
		void readUsers(std::unordered_map<std::string, User>& users);
};

#endif //_PROFILEPERSISTENCY_HPP
