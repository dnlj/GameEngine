#pragma once

// glLoadGen
#include <glloadgen/gl_core_4_5.hpp>

// Engine
#include <engine/Resource.hpp>
//#include <engine/ShaderProgram2.hpp>
#include <engine/Uniform2.hpp>


namespace engine {
	class ShaderProgramData2 {
		public:
			GLuint program;
			std::vector<Uniform2> properties;
			
			void loadProgramUniforms();

		private:
			template<class Type, class TypeData>
			friend static void Resource<Type, TypeData>::cleanup();

			void deleteData();
	};
}
