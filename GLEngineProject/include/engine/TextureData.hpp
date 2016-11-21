#pragma once

// glLoadGen
#include <glloadgen/gl_core_4_5.hpp>

// Engine
#include <engine/TextureFormat.hpp>
#include <engine/Resource.hpp>


namespace engine {
	class TextureData {
		public:
			TextureFormat format;
			GLuint texture;

		private:
			template<class Type, class TypeData>
			friend static void Resource<Type, TypeData>::cleanup();

			void deleteData();
	};
}