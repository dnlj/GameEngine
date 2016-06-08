#include <engine/engine.hpp>

namespace engine {
	void _error(const std::string &file, const size_t line, const std::string &msg) {
		// TODO: Test with utf8, wcerr?

		std::string date = engine::util::getDateString();
		std::string err = "[ERROR][" + date + "][" + file + ":" + std::to_string(line) + "] " + msg;

		std::ofstream logFile("log.txt", std::ios::app | std::ios::binary); // TODO: Make error file configurable

		if (!logFile) {
			std::cerr << "[ERROR] Failed to load log file." << std::endl;
		} else {
			logFile << err << std::endl;
		}

		std::cerr << err << std::endl;

		abort();
	}
}