#include <models/service_model.h>

ServiceModel::ServiceModel(std::string id, std::string creator_id, std::optional<std::string> buyer_id, std::string title, std::string description, int price, std::string status, std::string type, std::optional<std::string> image_url, std::string created_at, std::string updated_at) : _id(id), _creator_id(creator_id), _buyer_id(buyer_id), _title(title), _description(description), _price(price), _status(status), _type(type), _image_url(image_url), _created_at(created_at), _updated_at(updated_at) {}

ServiceModel::ServiceModel(std::string id, std::string creator_id, std::optional<std::string> buyer_id, std::string title, std::string description, int price, std::string status, std::string type, std::optional<std::string> image_url, std::string created_at, std::string updated_at, UserModel creator, UserModel buyer) : _id(id), _creator_id(creator_id), _buyer_id(buyer_id), _title(title), _description(description), _price(price), _status(status), _type(type), _image_url(image_url), _created_at(created_at), _updated_at(updated_at), _creator(creator), _buyer(buyer) {}

std::string ServiceModel::get_id() const { return _id; }
std::string ServiceModel::get_creator_id() const { return _creator_id; }
std::optional<std::string> ServiceModel::get_buyer_id() const { return _buyer_id; }
std::string ServiceModel::get_title() const { return _title; }
std::string ServiceModel::get_description() const { return _description; }
int ServiceModel::get_price() const { return _price; }
std::string ServiceModel::get_status() const { return _status; }
std::string ServiceModel::get_type() const { return _type; }
std::optional<std::string> ServiceModel::get_image_url() const { return _image_url; }
std::string ServiceModel::get_created_at() const { return _created_at; }
std::string ServiceModel::get_updated_at() const { return _updated_at; }
UserModel ServiceModel::get_creator() const { return _creator; };
UserModel ServiceModel::get_buyer() const { return _buyer; };

std::unique_ptr<ServiceModel> ServiceModel::create_service(pqxx::connection& db, const std::string& creator_id, const std::string& title, const std::string& description, const int price, const std::string& type, const std::optional<std::string>& image_url, bool isThrow) {
	pqxx::work txn(db);

	pqxx::result result = txn.exec_params("INSERT INTO services (creator_id, title, description, price, type) VALUES ($1, $2, $3, $4, $5) RETURNING id,  creator_id, buyer_id, title, description, price, status, type, image_url, created_at, updated_at", creator_id, title, description, price, type);

	txn.commit();

	if (result.empty()) {
		if (isThrow)
			throw creation_exception("service could not be created");
		else
			return nullptr;
	}

	std::optional<std::string> buyer_id_field;
	std::optional<std::string> image_url_field;
	if (!result[0]["buyer_id"].is_null())
		buyer_id_field = result[0]["buyer_id"].as<std::string>();
	else
		buyer_id_field = std::nullopt;
	if (!result[0]["image_url"].is_null())
		image_url_field = result[0]["image_url"].as<std::string>();
	else
		image_url_field = std::nullopt;

	return std::make_unique<ServiceModel>(
		result[0]["id"].as<std::string>(),
		result[0]["creator_id"].as<std::string>(),
		buyer_id_field,
		result[0]["title"].as<std::string>(),
		result[0]["description"].as<std::string>(),
		result[0]["price"].as<int>(),
		result[0]["status"].as<std::string>(),
		result[0]["type"].as<std::string>(),
		image_url_field,
		result[0]["created_at"].as<std::string>(),
		result[0]["updated_at"].as<std::string>());
}
std::vector<std::unique_ptr<ServiceModel>> ServiceModel::get_services(pqxx::connection& db, const std::string& community_id, const std::string& status) {
	std::vector<std::unique_ptr<ServiceModel>> all_services;

	pqxx::work txn(db);

	std::string query =
		"SELECT s.id AS service_id, "
		"s.creator_id, "
		"s.buyer_id, "
		"s.title, "
		"s.description, "
		"s.price, "
		"s.status, "
		"s.type, "
		"s.image_url, "
		"s.created_at, "
		"s.updated_at, "
		"uc.id AS creator_id, "
		"uc.community_id AS creator_community_id, "
		"uc.username AS creator_username, "
		"uc.email AS creator_email, "
		"uc.type AS creator_type, "
		"uc.balance AS creator_balance, "
		"uc.created_at AS creator_created_at, "
		"uc.updated_at AS creator_updated_at, "
		"ub.id AS buyer_id, "
		"ub.community_id AS buyer_community_id, "
		"ub.username AS buyer_username, "
		"ub.email AS buyer_email, "
		"ub.type AS buyer_type, "
		"ub.balance AS buyer_balance, "
		"ub.created_at AS buyer_created_at, "
		"ub.updated_at AS buyer_updated_at "
		"FROM services AS s "
		"JOIN users AS uc ON s.creator_id = uc.id "
		"LEFT JOIN users AS ub ON s.buyer_id = ub.id "
		"WHERE uc.community_id = $1";

	// Agregar filtro de status si se proporciona
	if (!status.empty()) {
		query += " AND s.status = $2";
	}

	pqxx::result result;
	if (!status.empty()) {
		result = txn.exec_params(query, community_id, status);
	} else {
		result = txn.exec_params(query, community_id);
	}

	txn.commit();

	for (const auto& row : result) {
		std::optional<std::string> buyer_id_field;
		std::optional<std::string> image_url_field;
		if (!row["buyer_id"].is_null())
			buyer_id_field = row["buyer_id"].as<std::string>();
		else
			buyer_id_field = std::nullopt;
		if (!row["image_url"].is_null())
			image_url_field = row["image_url"].as<std::string>();
		else
			image_url_field = std::nullopt;

		// Crear instancia de UserModel para el creador
		UserModel creator(
			row["creator_id"].as<std::string>(),
			row["creator_community_id"].as<std::string>(),
			row["creator_username"].as<std::string>(),
			row["creator_email"].as<std::string>(),
			row["creator_type"].as<std::string>(),
			row["creator_balance"].as<int>(),
			row["creator_created_at"].as<std::string>(),
			row["creator_updated_at"].as<std::string>());

		// Crear instancia de UserModel para el comprador, si existe
		UserModel buyer;
		if (buyer_id_field) {
			buyer = UserModel(
				row["buyer_id"].as<std::string>(),
				row["buyer_community_id"].as<std::string>(),
				row["buyer_username"].as<std::string>(),
				row["buyer_email"].as<std::string>(),
				row["buyer_type"].as<std::string>(),
				row["buyer_balance"].as<int>(),
				row["buyer_created_at"].as<std::string>(),
				row["buyer_updated_at"].as<std::string>());
		}

		// Crear instancia de ServiceModel con UserModel como argumento adicional
		all_services.push_back(std::make_unique<ServiceModel>(
			row["service_id"].as<std::string>(),
			row["creator_id"].as<std::string>(),
			buyer_id_field,
			row["title"].as<std::string>(),
			row["description"].as<std::string>(),
			row["price"].as<int>(),
			row["status"].as<std::string>(),
			row["type"].as<std::string>(),
			image_url_field,
			row["created_at"].as<std::string>(),
			row["updated_at"].as<std::string>(),
			creator,
			buyer));
	}

	return all_services;
}

std::unique_ptr<ServiceModel> get_service(pqxx::connection& db, const std::string& column, const std::string& value, bool throw_when_null) {
	pqxx::work txn(db);
	pqxx::result result = txn.exec_params(std::format("SELECT * FROM users WHERE {} = $1", column), value);
	txn.commit();

	if (result.empty()) {
		if (throw_when_null)
			throw data_not_found_exception("service not found");
		else
			return nullptr;
	}

	return std::make_unique<ServiceModel>(
		result[0]["id"].as<std::string>(),
		result[0]["creator_id"].as<std::string>(),
		result[0]["buyer_id"].as<std::string>(),
		result[0]["tittle"].as<std::string>(),
		result[0]["description"].as<std::string>(),
		result[0]["price"].as<int>(),
		result[0]["service_status"].as<std::string>(),
		result[0]["service_type"].as<std::string>(),
		result[0]["image_url"].as<std::string>(),
		result[0]["created_at"].as<std::string>(),
		result[0]["updated_at"].as<std::string>());
}

std::unique_ptr<ServiceModel> ServiceModel::get_service_by_id(pqxx::connection& db, const std::string& id, bool throw_when_null) {
	return get_service(db, "id", id, throw_when_null);
}

bool ServiceModel::delete_service_by_id(pqxx::connection& db, const std::string id) {
	try {
		pqxx::work txn(db);

		pqxx::result result = txn.exec_params("DELETE FROM services WHERE id = $1 RETURNING id", id);

		txn.commit();

		if (!result.empty()) return true;

		return false;
	} catch (const std::exception& e) {
		std::cerr << "Failed to delete service: " << e.what() << std::endl;
		return false;
	}
}