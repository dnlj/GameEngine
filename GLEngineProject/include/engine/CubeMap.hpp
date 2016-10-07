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
			// TODO: Need to make a test.cube file or something so i can handle cubemaps that use multiple files
			static CubeMap loadCubeMap(const ResourcePath& path);
	};
}