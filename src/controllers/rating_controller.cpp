#include <controllers/rating_controller.h>

void RatingController::create_rating(pqxx::connection& db, const crow::request& req, crow::response& res, const std::string& service_id) {
	try {
		if (!isValidUUID(service_id)) {
			handle_error(res, "invalid UUID format", 400);
			return;
		}

		crow::json::rvalue body = crow::json::load(req.body);

		const std::vector<std::string> required_fields = {"id", "rating", "description"};
		if (!validate_required_body_fields(body, required_fields, res)) return;

		std::unique_ptr<ServiceModel> service = ServiceModel::get_service_by_id(db, service_id, true);

		if (service->get_status() == ServiceStatus::OPEN) {
			handle_error(res, "service not closed", 400);
			return;
		}

		if (service->get_type() == ServiceType::OFFERED) {
			if (service->get_buyer_id() != body["id"].s()) {
				handle_error(res, "user can not rate this service", 403);
				return;
			}
		} else {
			if (service->get_creator_id() != body["id"].s()) {
				handle_error(res, "user can not rate this service", 403);
				return;
			}
		}

		int rating = body["rating"].i();
		if (rating > 5 || rating < 1) {
			if (service->get_creator_id() != body["id"].s()) {
				handle_error(res, "invalid rating", 400);
				return;
			}
		}

		std::string description = body["description"].s();

		std::unique_ptr<RatingModel> created_rating = RatingModel::create_rating(db, service_id, rating, description, true);

		crow::json::wvalue data(
			{{"id", created_rating->get_id()},
			 {"service_id", created_rating->get_service_id()},
			 {"rating", created_rating->get_rating()},
			 {"description", created_rating->get_description()}});

		res.code = 201;
		res.write(data.dump());
		res.end();
	} catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	} catch (const data_not_found_exception& e) {
		CROW_LOG_ERROR << "Data not found exception: " << e.what();
		handle_error(res, e.what(), 404);
	} catch (const creation_exception& e) {
		CROW_LOG_ERROR << "Creation exception: " << e.what();
		handle_error(res, e.what(), 404);
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error in create_rating controller: " << e.what();
		handle_error(res, "internal server error", 500);
	}
}

void RatingController::get_user_ratings(pqxx::connection& db, crow::response& res, const std::string& user_id) {
	try {
		if (!isValidUUID(user_id)) {
			handle_error(res, "invalid UUID format", 400);
			return;
		}

		UserModel::get_user_by_id(db, user_id, true);
		std::vector<std::unique_ptr<RatingModel>> s_rating = RatingModel::get_rating_by_user_id(db, user_id, true);

		crow::json::wvalue::list user_ratings;
		for (unsigned int i = 0; i < s_rating.size(); i++) {
			crow::json::wvalue data;

			data["id"] = s_rating[i]->get_id();
			data["service_id"] = s_rating[i]->get_service_id();
			data["rating"] = s_rating[i]->get_rating();
			data["description"] = s_rating[i]->get_description();

			std::optional<std::string> sender_id = s_rating[i]->get_sender_id();

			if (sender_id.has_value()) {
				data["sender_id"] = sender_id.value();
			}

			user_ratings.push_back(data);
		}
		crow::json::wvalue data{{"ratings", user_ratings}};

		res.write(data.dump());
		res.code = 200;
		res.end();
	} catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	} catch (const data_not_found_exception& e) {
		CROW_LOG_ERROR << "Data not found exception: " << e.what();
		handle_error(res, e.what(), 404);
	} catch (const std::exception& e) {
		std::cerr << "Error getting ratings: " << e.what() << std::endl;
		handle_error(res, "internal server error", 500);
	}
}

void RatingController::get_service_rating(pqxx::connection& db, crow::response& res, const std::string& service_id) {
	try {
		if (!isValidUUID(service_id)) {
			handle_error(res, "invalid UUID format", 400);
			return;
		}

		ServiceModel::get_service_by_id(db, service_id, true);

		std::unique_ptr<RatingModel> s_rating = RatingModel::get_rating_by_service_id(db, service_id, true);

		crow::json::wvalue data;

		data["id"] = s_rating->get_id();
		data["service_id"] = s_rating->get_service_id();
		data["rating"] = s_rating->get_rating();
		data["description"] = s_rating->get_description();

		std::optional<std::string> sender_id = s_rating->get_sender_id();

		if (sender_id.has_value()) {
			data["sender_id"] = sender_id.value();
		}

		res.write(data.dump());
		res.code = 200;
		res.end();
	} catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	} catch (const data_not_found_exception& e) {
		CROW_LOG_ERROR << "Data not found exception: " << e.what();
		handle_error(res, e.what(), 404);
	} catch (const std::exception& e) {
		std::cerr << "Error getting ratings: " << e.what() << std::endl;
		handle_error(res, "internal server error", 500);
	}
}
