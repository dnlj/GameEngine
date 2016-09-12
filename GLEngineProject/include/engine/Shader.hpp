#pragma once

// Includes
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>

// glLoadGen
#include <glloadgen\gl_core_4_5.h>

// Engine
#include <engine/engine.hpp>
#include <engine/ShaderData.hpp>

// TODO: Maybe add a enum class for GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, and the others like you did for TextureFormat
namespace engine {
	class Shader {
		public:

			//enum class {
			//	FRAGMENT,
			//	VERTEX,
			//};

			Shader(engine::index idx);
			~Shader();

			GLenum getType() const;
			GLuint getShader() const;


		private:
			engine::index index;

		////////////////////////////////////////////////////////////////
		// Static Stuff
		////////////////////////////////////////////////////////////////
		public:
			static Shader loadShader(const std::string &path, const GLenum type);
			static void cleanup();

		private:
			static std::vector<ShaderData> shaders;
			static std::unordered_map<std::string, engine::index> pathLookup;
	};
}