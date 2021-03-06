#pragma once

// STD
#include <vector>
#include <memory>

// Engine
#include <engine/ShaderProgram2.hpp>
#include <engine/UniformValue.hpp>


namespace engine {
	class MaterialData2 {
		template<class Type, class TypeData>
		friend static void Resource<Type, TypeData>::cleanup();

		public:
			ShaderProgram2 program;

			std::vector<std::unique_ptr<UniformValueBase>> uniforms;
			std::vector<Texture> textures;

		private:
			void deleteData() {
				// TODO: mvoe to .cpp and implement
			};
	};
}