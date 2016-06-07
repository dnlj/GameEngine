#pragma once

#include <engine/SubMeshData.hpp>

namespace engine {
	class MeshData {
		public:
			GLuint vbo;
			GLuint vao;
			std::vector<SubMeshData> subMeshes;
	};
}