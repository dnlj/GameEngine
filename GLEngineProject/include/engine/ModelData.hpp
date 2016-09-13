#pragma once

#include <vector>

#include <engine/SubModelData.hpp>

namespace engine {
	class ModelData {
		public:
			GLuint vbo;
			GLuint vao;
			std::vector<SubModelData> subMeshes;
	};
}