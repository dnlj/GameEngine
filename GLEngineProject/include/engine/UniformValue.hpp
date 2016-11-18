#pragma once

// STD
#include <vector>

// glLoadGen
#include <glloadgen/gl_core_4_5.hpp>

// Engine
//#include <engine/ShaderProgram2.hpp>


namespace engine {
	// TODO: move to own files
	class UniformValueBase {
		virtual void setUniform(ShaderProgram2& program) const = 0;
	};

	// TODO: split into tpp file
	template<class T>
	class UniformValue : public UniformValueBase {
		public:
			UniformValue(int location, T data) : location{location}, data{data}  {}

			void setUniform(ShaderProgram2& program) const override {
				program.setUniform(location, data);
			}

			int location;
			T data;
	};
}