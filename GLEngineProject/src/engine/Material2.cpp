// Engine
#include <engine/Texture.hpp>
#include <engine/Material2.hpp>

namespace engine {

	void Material2::setupUniforms() {
		const auto& data = getDataAt(index);
		for (const auto& uniform : data.uniforms) {
		}
	}

	Material2 Material2::load(const ResourcePath& path) {
		const auto resolvedPath = path.getResolvedPath();
		auto& loadInfo = loadResource(resolvedPath);

		// Load the object if it has not already been loaded
		if (!loadInfo.alreadyLoaded) {
			auto& materialData = loadInfo.data;

			// Load the json document
			rapidjson::Document doc;
			json::loadDocument(resolvedPath, doc);

			// Load the resource paths
			ResourcePath program = json::getString(doc, "path")->value.GetString();
			auto params = json::getObject(doc, "parameters");

			ResourcePath albedo = json::getString(params->value, "albedo")->value.GetString();
			ResourcePath normal = json::getString(params->value, "normal")->value.GetString();
			ResourcePath roughness = json::getString(params->value, "roughness")->value.GetString();

			// Load the ShaderProgram 
			materialData.program = ShaderProgram2::load(program);



			// TODO: Need to make these auto detect what is used based on the .mat file

			// Albedo
			materialData.uniforms.emplace_back(
				std::make_unique<UniformValue<Texture>>(
					materialData.program.getUniformIndex("albedoMap"),
					Texture::load(albedo)
				)
			);

			// Normal
			materialData.uniforms.emplace_back(
				std::make_unique<UniformValue<Texture>>(
					materialData.program.getUniformIndex("normalMap"),
					Texture::load(normal)
				)
			);

			// Roughness
			materialData.uniforms.emplace_back(
				std::make_unique<UniformValue<Texture>>(
					materialData.program.getUniformIndex("roughnessMap"),
					Texture::load(roughness)
				)
			);
		}

		return loadInfo.object;
	}
}