#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>

#include <glloadgen/gl_core_4_5.hpp>

// GLFW
#include <GLFW/glfw3.h>

// Engine
#include <engine/engine.hpp>

namespace engine {
	namespace util {
		std::string loadFile(const std::string &path);
		void checkGLErrors();
		void printGLInfo();
		void initializeOpenGL();
		std::string getDateString();
		std::string typeEnumToString(const GLenum &type);
	}
}