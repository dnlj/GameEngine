#pragma once

// glLoadGen
#include <glloadgen/gl_core_4_5.hpp>

// Engine
#include <engine/TextureFormat.hpp>

namespace engine {
	class TextureData {
		public:
			GLuint texture;
			TextureFormat format;
	};
}