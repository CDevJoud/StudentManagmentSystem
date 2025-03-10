#pragma once
#include <nlohmann/json.hpp>

struct Globals {
	nlohmann::json config;
	nlohmann::json db;
};

extern Globals g;
