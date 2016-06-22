#include <engine/Shader.hpp>

namespace engine {
	Shader::Shader(GLenum shaderType, std::string source) : shader(0), type(shaderType) {
		shader = glCreateShader(type);

		if (shader == 0) {
			engine_error("glCreateShader failed.");
		}

		const char *charSource = source.c_str();
		glShaderSource(shader, 1, (const GLchar**)&charSource, nullptr);
		glCompileShader(shader);

		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			std::string errorMsg = "Shader compile failed with error:\n";

			GLint leng;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &leng);

			GLchar *buffer = new GLchar[leng]();
			glGetShaderInfoLog(shader, leng, nullptr, buffer);

			errorMsg.append(buffer);
			delete[] buffer;
			deleteShader();

			engine_error(errorMsg);
		}
	}

	Shader::~Shader() {
		glDeleteShader(shader);
	}

	GLenum Shader::getType() {
		return type;
	}

	GLuint Shader::getShader() {
		return shader;
	}

	void Shader::deleteShader() {
		glDeleteShader(shader);
		shader = 0;
	}
}