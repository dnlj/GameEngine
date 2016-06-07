#pragma once

#include <glloadgen/gl_core_4_4.h>

#include <engine/Material.hpp>


namespace engine {

	class SubMeshData {
		public:
			Material mat;
			GLuint ebo;
			GLsizei count;
	};

} // namespace: engine
