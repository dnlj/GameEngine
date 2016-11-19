#pragma once

// glLoadGen
#include <glloadgen/gl_core_4_5.hpp>

// Engine
#include <engine/Material2.hpp>


namespace engine {
	class SubModelData {
		public:
			Material2 material;
			GLuint ebo;
			GLsizei count;
	};
}
