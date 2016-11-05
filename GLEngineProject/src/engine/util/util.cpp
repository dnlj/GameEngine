// STD
#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>

// Engine
#include <engine/util/util.hpp>


namespace engine::util {

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

	std::string typeEnumToString(const GLenum &type) {
		// NOTE: This was done by hand. Find a better way to do this or it may become out dated.
		switch (type) {
			case GL_FLOAT: return "GL_FLOAT";
			case GL_FLOAT_VEC2: return "GL_FLOAT_VEC2";
			case GL_FLOAT_VEC3: return "GL_FLOAT_VEC3";
			case GL_FLOAT_VEC4: return "GL_FLOAT_VEC4";
			case GL_DOUBLE: return "GL_DOUBLE";
			case GL_DOUBLE_VEC2: return "GL_DOUBLE_VEC2";
			case GL_DOUBLE_VEC3: return "GL_DOUBLE_VEC3";
			case GL_DOUBLE_VEC4: return "GL_DOUBLE_VEC4";
			case GL_INT: return "GL_INT";
			case GL_INT_VEC2: return "GL_INT_VEC2";
			case GL_INT_VEC3: return "GL_INT_VEC3";
			case GL_INT_VEC4: return "GL_INT_VEC4";
			case GL_UNSIGNED_INT: return "GL_UNSIGNED_INT";
			case GL_UNSIGNED_INT_VEC2: return "GL_UNSIGNED_INT_VEC2";
			case GL_UNSIGNED_INT_VEC3: return "GL_UNSIGNED_INT_VEC3";
			case GL_UNSIGNED_INT_VEC4: return "GL_UNSIGNED_INT_VEC4";
			case GL_BOOL: return "GL_BOOL";
			case GL_BOOL_VEC2: return "GL_BOOL_VEC2";
			case GL_BOOL_VEC3: return "GL_BOOL_VEC3";
			case GL_BOOL_VEC4: return "GL_BOOL_VEC4";
			case GL_FLOAT_MAT2: return "GL_FLOAT_MAT2";
			case GL_FLOAT_MAT3: return "GL_FLOAT_MAT3";
			case GL_FLOAT_MAT4: return "GL_FLOAT_MAT4";
			case GL_FLOAT_MAT2x3: return "GL_FLOAT_MAT2x3";
			case GL_FLOAT_MAT2x4: return "GL_FLOAT_MAT2x4";
			case GL_FLOAT_MAT3x2: return "GL_FLOAT_MAT3x2";
			case GL_FLOAT_MAT3x4: return "GL_FLOAT_MAT3x4";
			case GL_FLOAT_MAT4x2: return "GL_FLOAT_MAT4x2";
			case GL_FLOAT_MAT4x3: return "GL_FLOAT_MAT4x3";
			case GL_DOUBLE_MAT2: return "GL_DOUBLE_MAT2";
			case GL_DOUBLE_MAT3: return "GL_DOUBLE_MAT3";
			case GL_DOUBLE_MAT4: return "GL_DOUBLE_MAT4";
			case GL_DOUBLE_MAT2x3: return "GL_DOUBLE_MAT2x3";
			case GL_DOUBLE_MAT2x4: return "GL_DOUBLE_MAT2x4";
			case GL_DOUBLE_MAT3x2: return "GL_DOUBLE_MAT3x2";
			case GL_DOUBLE_MAT3x4: return "GL_DOUBLE_MAT3x4";
			case GL_DOUBLE_MAT4x2: return "GL_DOUBLE_MAT4x2";
			case GL_DOUBLE_MAT4x3: return "GL_DOUBLE_MAT4x3";
			case GL_SAMPLER_1D: return "GL_SAMPLER_1D";
			case GL_SAMPLER_2D: return "GL_SAMPLER_2D";
			case GL_SAMPLER_3D: return "GL_SAMPLER_3D";
			case GL_SAMPLER_CUBE: return "GL_SAMPLER_CUBE";
			case GL_SAMPLER_1D_SHADOW: return "GL_SAMPLER_1D_SHADOW";
			case GL_SAMPLER_2D_SHADOW: return "GL_SAMPLER_2D_SHADOW";
			case GL_SAMPLER_1D_ARRAY: return "GL_SAMPLER_1D_ARRAY";
			case GL_SAMPLER_2D_ARRAY: return "GL_SAMPLER_2D_ARRAY";
			case GL_SAMPLER_1D_ARRAY_SHADOW: return "GL_SAMPLER_1D_ARRAY_SHADOW";
			case GL_SAMPLER_2D_ARRAY_SHADOW: return "GL_SAMPLER_2D_ARRAY_SHADOW";
			case GL_SAMPLER_2D_MULTISAMPLE: return "GL_SAMPLER_2D_MULTISAMPLE";
			case GL_SAMPLER_2D_MULTISAMPLE_ARRAY: return "GL_SAMPLER_2D_MULTISAMPLE_ARRAY";
			case GL_SAMPLER_CUBE_SHADOW: return "GL_SAMPLER_CUBE_SHADOW";
			case GL_SAMPLER_BUFFER: return "GL_SAMPLER_BUFFER";
			case GL_SAMPLER_2D_RECT: return "GL_SAMPLER_2D_RECT";
			case GL_SAMPLER_2D_RECT_SHADOW: return "GL_SAMPLER_2D_RECT_SHADOW";
			case GL_INT_SAMPLER_1D: return "GL_INT_SAMPLER_1D";
			case GL_INT_SAMPLER_2D: return "GL_INT_SAMPLER_2D";
			case GL_INT_SAMPLER_3D: return "GL_INT_SAMPLER_3D";
			case GL_INT_SAMPLER_CUBE: return "GL_INT_SAMPLER_CUBE";
			case GL_INT_SAMPLER_1D_ARRAY: return "GL_INT_SAMPLER_1D_ARRAY";
			case GL_INT_SAMPLER_2D_ARRAY: return "GL_INT_SAMPLER_2D_ARRAY";
			case GL_INT_SAMPLER_2D_MULTISAMPLE: return "GL_INT_SAMPLER_2D_MULTISAMPLE";
			case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY";
			case GL_INT_SAMPLER_BUFFER: return "GL_INT_SAMPLER_BUFFER";
			case GL_INT_SAMPLER_2D_RECT: return "GL_INT_SAMPLER_2D_RECT";
			case GL_UNSIGNED_INT_SAMPLER_1D: return "GL_UNSIGNED_INT_SAMPLER_1D";
			case GL_UNSIGNED_INT_SAMPLER_2D: return "GL_UNSIGNED_INT_SAMPLER_2D";
			case GL_UNSIGNED_INT_SAMPLER_3D: return "GL_UNSIGNED_INT_SAMPLER_3D";
			case GL_UNSIGNED_INT_SAMPLER_CUBE: return "GL_UNSIGNED_INT_SAMPLER_CUBE";
			case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: return "GL_UNSIGNED_INT_SAMPLER_1D_ARRAY";
			case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: return "GL_UNSIGNED_INT_SAMPLER_2D_ARRAY";
			case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: return "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE";
			case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY";
			case GL_UNSIGNED_INT_SAMPLER_BUFFER: return "GL_UNSIGNED_INT_SAMPLER_BUFFER";
			case GL_UNSIGNED_INT_SAMPLER_2D_RECT: return "GL_UNSIGNED_INT_SAMPLER_2D_RECT";
			default: return std::to_string(type);
		}
	}

	std::string toString(const glm::vec3& vec) {
		return "(" + std::to_string(vec.x)  + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ")";
	}
}