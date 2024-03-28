#ifndef COMMUNITY_MODEL_H
#define COMMUNITY_MODEL_H

#include <ctime>
#include <pqxx/pqxx>
#include <string>

class CommunityModel {
	private:
	std::string _id;
	std::string _community_name;
	std::string _community_code;

	public:
	CommunityModel(std::string id, std::string community_name, std::string community_code);

	std::string get_id() const;
	std::string get_community_name() const;
	std::string get_community_code() const;

	static std::string generate_community_code();

	static CommunityModel create_community(pqxx::connection& db, std::string community_name);
	static std::string get_community_id(pqxx::connection& db, std::string community_code);
	static bool comunity_exist(pqxx::connection& db, std::string community_name);
};

#endif