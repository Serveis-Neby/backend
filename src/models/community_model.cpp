#include <models/community_model.h>

CommunityModel::CommunityModel(std::string id, std::string community_name, std::string community_code) : _id(id), _community_name(community_name), _community_code(community_code) {
}

std::string CommunityModel::generate_community_code() {
	const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	const int codeLength = 8;
	std::string code;

	std::srand(std::time(nullptr));

	for (int i = 0; i < codeLength; ++i) {
		code += charset[std::rand() % charset.size()];
	}

	return code;
}

CommunityModel CommunityModel::create_community(pqxx::connection& db, std::string community_name) {
	pqxx::work txn(db);

	std::string community_code = generate_community_code();

	txn.exec_params("INSERT INTO communities (community_name, community_code) VALUES ($1, $2)", community_name, community_code);

	pqxx::result result = txn.exec_params("SELECT id FROM communities WHERE community_code = $1", community_code);

	std::string res = result[0][0].as<std::string>();

	txn.commit();

	return CommunityModel(res, community_code, community_name);
}

std::string CommunityModel::get_community_id(pqxx::connection& db, std::string community_code) {
	try {
		pqxx::work txn(db);

		// Construir la consulta SQL parametrizada para obtener el ID de la comunidad dado su código
		std::string query = "SELECT id FROM communities WHERE community_code = $1";

		// Ejecutar la consulta parametrizada
		pqxx::result result = txn.exec_params(query, community_code);

		// Comprobar si se encontró la comunidad
		if (!result.empty() && !result[0]["id"].is_null()) {
			std::string community_id = result[0]["id"].as<std::string>();
			txn.commit();
			return community_id;
		} else {
			// Si no se encuentra la comunidad, devolver un valor nulo o lanzar una excepción según sea necesario
			txn.commit();
			return "";	// O cualquier otro valor que desees para indicar que la comunidad no se encontró
		}
	} catch (const std::exception& e) {
		// Manejar cualquier excepción que ocurra durante la ejecución de la consulta
		return "";	// O lanzar una excepción adecuada según sea necesario
	}
}

std::string CommunityModel::get_id() const {
	return _id;
}

std::string CommunityModel::get_community_name() const {
	return _community_name;
}

std::string CommunityModel::get_community_code() const {
	return _community_code;
}

bool CommunityModel::comunity_exist(pqxx::connection& db, std::string community_name) {
	try {
		pqxx::work txn(db);

		pqxx::result result = txn.exec_params("SELECT community_name FROM communities WHERE community_name = $1", community_name);

		bool communityExists = !result.empty() && !result[0][0].is_null();

		txn.commit();

		return communityExists;
	} catch (const std::exception& e) {
		return false;
	}
}