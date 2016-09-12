#pragma once

#include <string>

#include <glloadgen/gl_core_4_5.h>

namespace engine {
	class Uniform { // TODO: Rename? idk
		public:
		Uniform(const std::string &name, const GLenum &type, const GLint &location) : name(name), type(type), location(location) {};
		std::string name;
		GLenum type;
		GLint location;
	};
}