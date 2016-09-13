#pragma once

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Engine
#include <engine/Resource.hpp>
#include <engine/ModelData.hpp>
#include <engine/Vertex.hpp>

// TODO: remove this is a temp include
#include <engine/ShaderProgram.hpp>

// TODO: This class is completely untested
namespace engine {
	class Model : public Resource<Model, ModelData> {
		public:
			using Resource<Model, ModelData>::Resource;
			virtual ~Model();

			void render() const;

			// TODO: Remove this its temp untill i get materials/model/shader colab working
			void tempSetupGLStuff(const ShaderProgram& program);

			bool operator==(const Model &model) const;

		////////////////////////////////////////////////////////////////
		// Static
		////////////////////////////////////////////////////////////////
		public:
			static Model loadModel(const ResourcePath& path, const float scale = 1.0f, const float textureScale = 1.0f);
			static ModelData setupGLObjects(const std::vector<Vertex> &vertices, const std::vector<std::vector<GLuint>> &indices);
	};
}