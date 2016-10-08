#pragma once

// STD
#include <vector>

// glLoadGen
#include <glloadgen/gl_core_4_5.hpp>

// Engine
#include <engine/ShaderProgram.hpp>


namespace engine {
	class Material;

	class MaterialParameter {
		public:
			GLint location;
			std::vector<GLubyte> data;
			bool use = false;
			void (ShaderProgram::*func)(const unsigned int &index, std::vector<GLubyte> data);
	};
}