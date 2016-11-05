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

	/**
	 * Loads a file as a string.
	 * 
	 * @param[in] path The path to the file to load.
	 * @return The contents of the file.
	 */
	std::string loadFile(const std::string &path);

	/**
	 * Prints out any OpenGL errors.
	 * 
	 * @return void
	 */
	void checkGLErrors();

	/**
	 * Prints OpenGl and related info.
	 * 
	 * @return void
	 */
	void printGLInfo();

	/**
	 * Loads OpenGL functions.
	 * 
	 * @return void
	 */
	void initializeOpenGL();

	/**
	 * Get the current date as a string.
	 * 
	 * @return The current date as a string.
	 */
	std::string getDateString();

	/**
	 * Converts a OpenGL type enumeration to a string.
	 * 
	 * @return The string representation of the enumeration.
	 */
	std::string typeEnumToString(const GLenum &type);

	/**
	 * Converts @p vec to a string.
	 * 
	 * @return The string representation of @p vec.
	 */
	std::string toString(const glm::vec3& vec);
}