#pragma once

// STD
#include <iostream>
#include <vector>

// Engine
#include <engine/SubModelData.hpp>
#include <engine/Resource.hpp>

namespace engine {
	class ModelData {
		public:
			GLuint vbo;
			GLuint vao;
			std::vector<SubModelData> subModels;

		private:
			template<class Type, class TypeData>
			friend static void Resource<Type, TypeData>::cleanup();

			void deleteData();
	};
}