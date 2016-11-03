// STD
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <iomanip>

// ImGui
// TODO: #define IMGUI_DISABLE_TEST_WINDOWS
#include <imgui/imgui.h>
// TODO: This is temp until i get around to doing this myself
#include <imgui_test.hpp>

// Engine
#include <engine/engine.hpp>
#include <engine/CubeMap.hpp>
#include <engine/Texture.hpp>
#include <engine/Shader.hpp>
#include <engine/Model.hpp>
#include <engine/util/util.hpp>


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

	void _atExit() {
		std::cout << "engine::_atExit()\n";

		// Cleanup all OpenGL stuff
		cleanup();
		
		// Shutdown ImGui
		ImGui_ImplGlfwGL3_Shutdown();
		//ImGui::Shutdown(); // Called by the above

		// Destroy the GLFW window and OpenGL context
		glfwDestroyWindow(getWindow());
		glfwTerminate();
	}

	void cleanup() {
		engine::CubeMap::cleanup();
		engine::Texture::cleanup();
		engine::Model::cleanup();
		engine::Shader::cleanup();
	}

	GLFWwindow* getWindow() {
		static GLFWwindow* window = nullptr;

		if (window == nullptr) {
			setupWindow(window);
		}

		return window;
	}

	void setupWindow(GLFWwindow*& window, std::string title) {
		// GLFW setup
		glfwSetErrorCallback([](int error, const char* desc) {
			std::stringstream stream;
			stream << "GLFW error 0x"
				<< std::setfill('0') << std::setw(sizeof(int) * 2) << std::hex
				<< error << ": " << desc;
			engine_error(stream.str());
		});


		// Initialize GLFW and create the window
		if (!glfwInit()) { engine_error("GLFW initialization failed."); }


		// GLFW hints
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
		glfwWindowHint(GLFW_DECORATED, GL_TRUE);

		glfwWindowHint(GLFW_RED_BITS, 8);
		glfwWindowHint(GLFW_GREEN_BITS, 8);
		glfwWindowHint(GLFW_BLUE_BITS, 8);
		glfwWindowHint(GLFW_ALPHA_BITS, 8);
		glfwWindowHint(GLFW_DEPTH_BITS, 32);
		glfwWindowHint(GLFW_STENCIL_BITS, 8);

		glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);


		// Window setup
		constexpr bool fullscreen = false;
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *videoMode = glfwGetVideoMode(monitor);

		if (fullscreen) {
			window = glfwCreateWindow(videoMode->width, videoMode->height, title.c_str(), monitor, nullptr);
		} else {
			window = glfwCreateWindow(1280, 720, title.c_str(), nullptr, nullptr);
		}

		if (!window) { engine_error("GLFW failed to create window."); }

		if (!fullscreen) {
			int width, height;
			glfwGetWindowSize(window, &width, &height);

			glfwSetWindowPos(window, videoMode->width / 2 - width / 2, videoMode->height / 2 - height / 2);
		}

		glfwMakeContextCurrent(window);
		glfwSwapInterval(1); // VSYNC 0=off        x = rate/x        1=rate/1 2=rate/2 etc...
		glfwSetWindowSizeCallback(window, [](GLFWwindow *window, int width, int height) { glViewport(0, 0, width, height); });
	}
}