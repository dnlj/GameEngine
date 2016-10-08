#pragma once

// STD
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <chrono>
#include <iomanip>

// glLoadGen
#include <glloadgen/gl_core_4_5.hpp>

// GLFW
#include <GLFW/glfw3.h>

// Engine
#include <engine/util.hpp>

#define engine_error(msg) ::engine::_error(__FILE__, __LINE__, msg)

#define engine_warning(msg) ::engine::_warning(__FILE__, __LINE__, msg)

namespace engine {
	// TODO: make index upper case. Dont know why it is lower
	using index = unsigned int;
	void _error(const std::string &file, const std::size_t line, const std::string &msg);
	void _warning(const std::string &file, const std::size_t line, const std::string &msg);
	void _atExit();

	void cleanup();
	GLFWwindow* getWindow();
	void setupWindow(GLFWwindow*& window, std::string title = "Window Title");
}