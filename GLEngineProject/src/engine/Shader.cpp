#pragma once

#include <engine/Shader.hpp>

namespace engine {
	Shader::~Shader() {
	}

	GLuint Shader::getType() const {
		return getDataAt(index).type;
	}

	GLuint Shader::getShader() const {
		return getDataAt(index).shader;
	}

	

	////////////////////////////////////////////////////////////////
	// Static
	////////////////////////////////////////////////////////////////
	// TODO: Should i make "type" part of the shader file? maybe
	Shader Shader::loadShader(const ResourcePath& path, const GLenum type) {
		const auto resolvedPath = path.getResolvedPath();
		auto& loadInfo = loadResource(resolvedPath);

		// Load the object if it has not already been loaded
		if (!loadInfo.alreadyLoaded) {
			auto& shaderData = loadInfo.data;
		
			// Attempt to create a shader
			shaderData.shader = glCreateShader(type);
			shaderData.type = type;
		
			// Check if glCreateShader was successful
			if (shaderData.shader == 0) {
				glDeleteShader(shaderData.shader);
				engine_error("glCreateShader failed.");
			}
		
			// Load thie file from disk onto the GPU
			const std::string source = engine::util::loadFile(resolvedPath);
			const char* charSource = source.c_str();
			glShaderSource(shaderData.shader, 1, (const GLchar**)&charSource, nullptr);
			glCompileShader(shaderData.shader);
		
			// Check for errors
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
		}

		return loadInfo.object;
	}
}
