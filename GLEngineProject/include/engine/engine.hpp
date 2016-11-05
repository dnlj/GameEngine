#pragma once

// STD
#include <string>
#include <sstream>

// glLoadGen
#include <glloadgen/gl_core_4_5.hpp>

// GLFW
#include <GLFW/glfw3.h>


#define engine_error(msg) ::engine::_error(__FILE__, __LINE__, msg)

#define engine_warning(msg) ::engine::_warning(__FILE__, __LINE__, msg)


namespace engine {
	// TODO: make index upper case. Dont know why it is lower
	using index = unsigned int;

	/**
	 * Called by engine_error. Should not be called directly.
	 * 
	 * @param[in] file The file the error was generated from.
	 * @param[in] line The line number the error was generted from.
	 * @param[in] msg The error message
	 * @return void
	 */
	void _error(const std::string& file, const std::size_t line, const std::string &msg);

	/**
	 * Called by engine_warning. Should not be called directly.
	 *
	 * @param[in] file The file the warning was generated from.
	 * @param[in] line The line number the warning was generted from.
	 * @param[in] msg The warning message
	 * @return void
	 */
	void _warning(const std::string &file, const std::size_t line, const std::string &msg);

	/**
	 * Called when the program closes. Used primarily for cleanup. Should not be called directly.
	 * 
	 * @return void
	 */
	void _atExit();

	/**
	 * Performs resource cleanup.
	 * 
	 * @return void
	 */
	void cleanup();

	/**
	 * Gets the window used by this program.
	 * 
	 * @return The window used by this program.
	 */
	GLFWwindow* getWindow();

	/**
	 * Setups @p window.
	 * 
	 * @param[in] window The window to setup.
	 * @param[in] title The window title.
	 * @retun void
	 */
	void setupWindow(GLFWwindow*& window, std::string title = "Window Title");
}