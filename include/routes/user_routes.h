#pragma once

#include <pqxx/pqxx>

#include "controllers/user_controller.h"
#include "crow.h"
#include "utils/common.h"

void initialize_user_routes(NebyApp& app, pqxx::connection& db);
