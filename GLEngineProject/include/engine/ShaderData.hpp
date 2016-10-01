#pragma once

// glLoadGen
#include <glloadgen/gl_core_4_5.hpp>

namespace engine {
	class ShaderData {
		public:
			GLuint shader;
			GLenum type;
	};
}