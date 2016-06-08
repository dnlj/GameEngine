#include <engine/util.hpp>

namespace engine {
	namespace util {

		std::string loadFile(const std::string &fileName) {
			std::ifstream file(fileName, std::ios::in | std::ios::binary);
		
			if (!file) { return ""; }

			std::string ret;

			file.seekg(0, std::ios::end);
			ret.resize(file.tellg());
			file.seekg(0, std::ios::beg);
			file.read(&ret[0], ret.size());
			return ret;
		}

		void checkGLErrors() {
			std::cout << "[Start checkGLErrors]: " << std::endl;
			for (int err = glGetError(); err != GL_NO_ERROR; err = glGetError()) {
				switch (err) {
					case GL_NO_ERROR:
						std::cout << "\tGL_NO_ERROR" << std::endl;
						break;
					case GL_INVALID_ENUM:
						std::cout << "\tGL_INVALID_ENUM" << std::endl;
						break;
					case GL_INVALID_VALUE:
						std::cout << "\tGL_INVALID_VALUE" << std::endl;
						break;
					case GL_INVALID_OPERATION:
						std::cout << "\tGL_INVALID_OPERATION" << std::endl;
						break;
					case GL_INVALID_FRAMEBUFFER_OPERATION:
						std::cout << "\tGL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
						break;
					case GL_OUT_OF_MEMORY:
						std::cout << "\tGL_OUT_OF_MEMORY" << std::endl;
						break;
					case GL_STACK_UNDERFLOW:
						std::cout << "\tGL_STACK_UNDERFLOW" << std::endl;
						break;
					case GL_STACK_OVERFLOW:
						std::cout << "\tGL_STACK_OVERFLOW" << std::endl;
						break;
					default:
						std::cout << "Unknown return value from checkGLErrors()" << std::endl;
						break;
				}
			}

			std::cout << "[End checkGLErrors]" << std::endl;
		}

		void printGLInfo() {
			int major, minor, rev;
			glfwGetVersion(&major, &minor, &rev);

			std::cout << "-- OpenGL Info -----------------------------------------------------------------" << std::endl;
			std::cout << "GLFW version: " << major << "." << minor << "." << rev << std::endl;
			std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
			std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
			std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
			std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
			std::cout << "--------------------------------------------------------------------------------" << std::endl;
		}


		void initializeOpenGL() {
			int loaded = ogl_LoadFunctions();
			if (loaded == ogl_LOAD_FAILED) {
				engine_error(std::string("[glLoadGen] Initialization failed: ").append("TODO: Get there error here")); // TODO: Get there error here
			}

			int num_failed = loaded - ogl_LOAD_SUCCEEDED;
			if (num_failed > 0) {
				printf("[glLoadGen] %i functions failed to load.\n", num_failed);
			}

			if (!ogl_IsVersionGEQ(4, 4)) { engine_error("OpenGL 4.4 API is not available."); }
		}

		std::string getDateString() {
			const time_t now = time(nullptr);

			#ifdef _MSC_VER
				tm timeInfo{};
				localtime_s(&timeInfo, &now);

				std::stringstream stream;
				stream << std::put_time(&timeInfo, "%a %b %d %I:%M:%S %p %Y");

				return stream.str();
			#else
				// TODO: Create a thread safe version of getDateString for other platforms (localtime_r ?)
				#error TODO: Create a thread safe version of getDateString for this platform
			#endif
		}

	}
}