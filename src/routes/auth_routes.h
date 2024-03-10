#ifndef AUTH_ROUTES_H
#define AUTH_ROUTES_H

#include <pqxx/pqxx>

#include "crow.h"

#include "../controllers/auth_controller.h"

void initialize_auth_routes(crow::SimpleApp& app, pqxx::connection& db);

#endif
