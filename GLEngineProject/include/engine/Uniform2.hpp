#pragma once

// STD
#include <string>

// glLoadGen
#include <glloadgen/gl_core_4_5.hpp>


namespace engine {
	class Uniform2 { // TODO: Rename? idk
		public:
			Uniform2(const std::string &name, const GLenum &type, const GLint &location) :
				name(name),
				type(type),
				location(location) {
			};

			std::string name;
			GLenum type;
			GLint location;
	};
}