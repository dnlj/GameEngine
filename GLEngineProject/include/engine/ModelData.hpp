#pragma once

// STD
#include <vector>

// Engine
#include <engine/SubModelData.hpp>

namespace engine {
	class ModelData {
		public:
			GLuint vbo;
			GLuint vao;
			std::vector<SubModelData> subModels;
	};
}