#include <engine/Shader.hpp>

namespace engine {
	Shader::Shader(engine::index idx) : index{idx} {
	}

	Shader::~Shader() {
	}

	
	GLenum Shader::getType() const {
		return shaders[index].type;
	}
	
	GLuint Shader::getShader() const {
		return shaders[index].shader;
	}

	////////////////////////////////////////////////////////////////
	// Static Stuff
	////////////////////////////////////////////////////////////////
	std::vector<ShaderData> Shader::shaders;
	std::unordered_map<std::string, engine::index> Shader::pathLookup;

	Shader Shader::loadShader(const std::string &path, const GLenum type) {
		{ // TODO: Untested
			auto it = pathLookup.find(path);
			if (it != pathLookup.end()) {
				if (shaders[it->second].type == type) {
					return Shader{it->second};
				}
			}
		}

		ShaderData shaderData{glCreateShader(type), type};

		if (shaderData.shader == 0) {
			engine_error("glCreateShader failed.");
		}

		const std::string source = engine::util::loadFile(path);
		const char *charSource = source.c_str();
		glShaderSource(shaderData.shader, 1, (const GLchar**)&charSource, nullptr);
		glCompileShader(shaderData.shader);

		GLint status;
		glGetShaderiv(shaderData.shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			std::string errorMsg = "Shader compilation failed with error:\n";

			GLint leng;
			glGetShaderiv(shaderData.shader, GL_INFO_LOG_LENGTH, &leng);

			GLchar *buffer = new GLchar[leng]();
			glGetShaderInfoLog(shaderData.shader, leng, nullptr, buffer);

			errorMsg.append(buffer);
			delete[] buffer;

			glDeleteShader(shaderData.shader);
			shaderData.shader = 0;

			engine_error(errorMsg);
		}

		engine::index index = static_cast<engine::index>(shaders.size());

		shaders.push_back(shaderData);
		pathLookup[path] = index;
		return Shader{index};
	}

	void Shader::cleanup() {
		for (const ShaderData& data : shaders) { // TODO: since this is a reference to a vector element could this cause problems if the vector re-allocates?
			glDeleteShader(data.shader);
		}
	}
}