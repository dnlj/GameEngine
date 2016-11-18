// Engine
#include <engine/json/json.hpp>
#include <engine/Shader.hpp>
#include <engine/ShaderProgram2.hpp>
#include <engine/util/util.hpp>

namespace engine {
	GLuint ShaderProgram2::getProgram() const {
		return getDataAt(index).program;
	}

	void ShaderProgram2::setUniform(const GLint location, const GLint data) {
		glUniform1i(location, data);
	}

	void ShaderProgram2::setUniform(const GLint location, const GLuint data) {
		glUniform1ui(location, data);
	}

	void ShaderProgram2::setUniform(const GLint location, const GLfloat data) {
		glUniform1f(location, data);
	}

	void ShaderProgram2::setUniform(const GLint location, const Texture& data) {
		glUniform1ui(location, data.getTexture());
	}

	GLint ShaderProgram2::getUniformIndex(const std::string& name) {
		const auto& properties = getDataAt(index).properties;
		

		for (const auto& prop : properties) {
			if (prop.name == name) {
				return prop.location;
			}
		}

		throw InvalidUniformName{"Uniform name \"" + name + "\" not found."};
	}


	ShaderProgram2 ShaderProgram2::load(const ResourcePath& path) {
		const auto resolvedPath = path.getResolvedPath();
		auto& loadInfo = loadResource(resolvedPath);

		// Load the object if it has not already been loaded
		if (!loadInfo.alreadyLoaded) {
			auto& programData = loadInfo.data;

			programData.program = glCreateProgram();

			if (programData.program == 0) {
				engine_error("glCreateProgram failed.");
			}

			rapidjson::Document doc;
			json::loadDocument(resolvedPath, doc);


			if (doc.HasMember("VERTEX_SHADER")) {
				const auto& shaderPath = json::getString(doc, "VERTEX_SHADER")->value.GetString();
				const auto& shader = Shader::load(shaderPath, GL_VERTEX_SHADER);

				glAttachShader(programData.program, shader.getShader());
			}

			if (doc.HasMember("FRAGMENT_SHADER")) {
				const auto& shaderPath = json::getString(doc, "FRAGMENT_SHADER")->value.GetString();
				const auto& shader = Shader::load(shaderPath, GL_FRAGMENT_SHADER);

				glAttachShader(programData.program, shader.getShader());
			}


			glLinkProgram(programData.program);

			{ // Check link status
				GLint status;

				glGetProgramiv(programData.program, GL_LINK_STATUS, &status);

				if (status == GL_FALSE) {
					std::string errorMsg = "Program linking failed with error:\n";

					GLint leng;
					glGetProgramiv(programData.program, GL_INFO_LOG_LENGTH, &leng);

					GLchar *buffer = new GLchar[leng]{};
					glGetProgramInfoLog(programData.program, leng, nullptr, buffer);

					errorMsg.append(buffer);
					delete[] buffer;

					glDeleteProgram(programData.program);
					programData.program = 0;

					engine_error(errorMsg);
				}
			}

			{ // Detach shader
				GLint count;
				glGetProgramiv(programData.program, GL_ATTACHED_SHADERS, &count);
				GLuint *shaders = new GLuint[count];
				glGetAttachedShaders(programData.program, count, nullptr, shaders);

				for (int i = 0; i < count; i++) {
					glDetachShader(programData.program, shaders[i]);
				}

				delete[] shaders;
				glGetProgramiv(programData.program, GL_ATTACHED_SHADERS, &count);
			}

			programData.loadProgramUniforms();
		}
		return loadInfo.object;
	}
}