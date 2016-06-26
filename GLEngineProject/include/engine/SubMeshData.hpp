#pragma once

#include <glloadgen/gl_core_4_5.h>

//#include <engine/Material.hpp>


namespace engine {
	class SubMeshData {
		public:
			//Material mat;
			GLuint ebo;
			GLsizei count;
	};
} // namespace: engine
