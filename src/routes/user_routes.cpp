#include <routes/user_routes.h>

void initialize_user_routes(NebyApp& app, pqxx::connection& db) {
	CROW_ROUTE(app, "/api/users").methods(crow::HTTPMethod::GET).CROW_MIDDLEWARES(app, VerifyJWT)([&db](const crow::request& req, crow::response& res) {
		UserController::get_users(db, req, res);
	});

	CROW_ROUTE(app, "/api/users/<string>").methods(crow::HTTPMethod::GET).CROW_MIDDLEWARES(app, VerifyJWT)([&db](const crow::request& req, crow::response& res, const std::string& user_id) {
		UserController::get_user_by_id(db, req, res, user_id);
	});

	CROW_ROUTE(app, "/api/users/<string>").methods(crow::HTTPMethod::DELETE).CROW_MIDDLEWARES(app, VerifyJWT)([&db](const crow::request& req, crow::response& res, const std::string& user_id) {
		UserController::delete_user_by_id(db, res, user_id);
	});

	CROW_ROUTE(app, "/api/users/<string>").methods(crow::HTTPMethod::PUT).CROW_MIDDLEWARES(app, VerifyJWT)([&db](const crow::request& req, crow::response& res, const std::string& user_id) {
		UserController::update_user_by_id(db, req, res, user_id);
	});

	CROW_ROUTE(app, "/api/users/self").methods(crow::HTTPMethod::PUT).CROW_MIDDLEWARES(app, VerifyJWT)([&db](const crow::request& req, crow::response& res) {
		UserController::update_self(db, req, res);
	});
}
