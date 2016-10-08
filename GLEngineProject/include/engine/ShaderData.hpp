#pragma once

// STD
#include <iostream>

// glLoadGen
#include <glloadgen/gl_core_4_5.hpp>

// Engine
#include <engine/Resource.hpp>

namespace engine {
	class ShaderData {
		public:
			GLuint shader;
			GLenum type;

		private:
			template<class Type, class TypeData>
			friend static void Resource<Type, TypeData>::cleanup();

			void deleteData();
	};
}