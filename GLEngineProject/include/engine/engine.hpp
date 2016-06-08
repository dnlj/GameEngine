#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <chrono>
#include <iomanip>

// Engine
#include <engine/util.hpp>

#define engine_error(msg) ::engine::_error(__FILE__, __LINE__, msg);

namespace engine {
	void _error(const std::string &file, const size_t line, const std::string &msg);
}