#pragma once

// Includes
#include <iostream>
#include <vector>
#include <unordered_map>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Engine
#include <engine/engine.hpp>
#include <engine/MeshData.hpp>
#include <engine/Vertex.hpp>

namespace engine {
	class Mesh {
		public:
			Mesh(engine::index idx);
			~Mesh();
			void render();

			bool operator==(const Mesh &mesh) const;
		private:
			engine::index index;
			
		////////////////////////////////////////////////////////////////
		// Static Stuff
		////////////////////////////////////////////////////////////////
		public:
		// Static stuff
			static Mesh loadMesh(const std::string &path, const float scale = 1.0f, const float textureScale = 1.0f);
			static void cleanup();
		private:
			public:// TODO: this is just temporray until i implement the vertex attrib stuff propperly
				static std::vector<MeshData> meshes;
			private:
			static std::unordered_map<std::string, engine::index> pathLookup; // TODO: You will need to handle multiple paths leading to the same mesh, maybe instead of using paths use some other kind of identifier (in the loading functions also)
			static MeshData setupGLObjects(const std::vector<Vertex> &vertices, const std::vector<std::vector<GLuint>> &indices);
	};
}