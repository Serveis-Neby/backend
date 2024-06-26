#pragma once

#include <crow.h>
#include <models/achievement_notification_model.h>
#include <models/notification_model.h>
#include <models/notification_service_model.h>
#include <models/service_model.h>
#include <models/user_achievement_model.h>
#include <models/user_model.h>
#include <utils/common.h>
#include <utils/utils.h>
#include <format>
#include <map>
#include <memory>
#include <pqxx/pqxx>
#include <string>

class NotificationController {
	public:
	static void create_notification(pqxx::connection& db, const crow::request& req, crow::response& res);
	static void handle_notification(pqxx::connection& db, crow::request& req, crow::response& res, const std::string& notification_id);
	static void create_service_notification(pqxx::connection& db, const crow::request& req, crow::response& res);
	static void get_notification_service_by_id(pqxx::connection& db, const crow::request& req, crow::response& res, const std::string& service_id);
	static void get_notifications(pqxx::connection& db, const crow::request& req, crow::response& res);
};
