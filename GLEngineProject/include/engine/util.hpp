#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>

#include <glloadgen/gl_core_4_4.h>

// GLFW
#include <GLFW/glfw3.h>

namespace engine {
	namespace util {

		std::string loadFile(const std::string &path);
	
		void checkGLErrors();

		void printGLInfo();

		void initializeOpenGL();

		std::string getDateString();

	}
}