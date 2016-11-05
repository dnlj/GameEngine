#pragma once

// STD
#include <string>

// glLoadGen
#include <glloadgen/gl_core_4_5.hpp>

// GLM
#include <glm/glm.hpp>

// GLFW
#include <GLFW/glfw3.h>

// Engine
#include <engine/engine.hpp>


namespace engine::util {
	std::string loadFile(const std::string &path);
	void checkGLErrors();
	void printGLInfo();
	void initializeOpenGL();
	std::string getDateString();
	std::string typeEnumToString(const GLenum &type);
	std::string toString(const glm::vec3& vec);
}