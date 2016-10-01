#include <engine/engine.hpp>

namespace engine {
	void _error(const std::string &file, const std::size_t line, const std::string &msg) {
		// TODO: Add a (cross platform) stack trace.

		std::string date = engine::util::getDateString();
		std::string err = "[ERROR][" + date + "][" + file + ":" + std::to_string(line) + "] " + msg;

		std::ofstream logFile("error.log", std::ios::app | std::ios::binary);

		if (!logFile) {
			std::cerr << "[ERROR] Failed to load log file." << std::endl;
		} else {
			logFile << err << std::endl;
			std::cerr << err << std::endl;
		}

		abort();
	}

	void _warning(const std::string &file, const std::size_t line, const std::string &msg) {
		// TODO: Add a (cross platform) stack trace.

		std::string date = engine::util::getDateString();
		std::string err = "[WARNING][" + date + "][" + file + ":" + std::to_string(line) + "] " + msg;

		std::ofstream logFile("warning.log", std::ios::app | std::ios::binary);

		if (!logFile) {
			std::cerr << "[WARNING] Failed to load log file." << std::endl;
		} else {
			logFile << err << std::endl;
		}

		std::cerr << err << std::endl;
	}
}