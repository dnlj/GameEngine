#pragma once

// glLoadGen
#include <glloadgen/gl_core_4_5.hpp>

// GLM
#include <glm/glm.hpp>

// SOIL
#include <SOIL.h>

// Engine
#include <engine/Resource.hpp>
#include <engine/CubeMapData.hpp>

namespace engine {
	class CubeMap : public Resource<CubeMap, CubeMapData> {
		public:
			using Resource<CubeMap, CubeMapData>::Resource;
			~CubeMap();

			GLuint getCubeMapTexture();

		////////////////////////////////////////////////////////////////
		// Static
		////////////////////////////////////////////////////////////////
		public:
			static CubeMap loadCubeMap(const ResourcePath& path);
	};
}