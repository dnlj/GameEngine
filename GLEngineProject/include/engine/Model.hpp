#pragma once

// Engine
#include <engine/Resource.hpp>
#include <engine/ModelData.hpp>
#include <engine/Vertex.hpp>
#include <engine/Camera.hpp>

// TODO: remove this is a temp include


// TODO: More testing, to make sure the Resrouce class is working with cleanup and duplicate load files
namespace engine {
	class Model : public Resource<Model, ModelData> {
		public:
			using Resource<Model, ModelData>::Resource;
			virtual ~Model();
			void render(const glm::mat4& mvp, const glm::mat4& model, const Camera& camera, const glm::vec3& lightPosition, float metalness, float intensity) const;
			bool operator==(const Model &model) const;



		////////////////////////////////////////////////////////////////
		// Static
		////////////////////////////////////////////////////////////////
		public:
			static Model load(const ResourcePath& path, const float scale = 1.0f, const float textureScale = 1.0f);
			static ModelData setupGLObjects(const std::vector<Vertex> &vertices, const std::vector<std::vector<GLuint>> &indices);
			static void setupVertexAttributes(const ModelData& modelData);
	};
}