// Engine
#include <engine/Texture.hpp>
#include <engine/Material2.hpp>

namespace engine {

	ShaderProgram2 Material2::getProgram() const {
		return getDataAt(index).program;
	}

	void Material2::use() const {
		const auto& data = getDataAt(index);
		glUseProgram(data.program.getProgram());
		
		// Set textures
		// TODO: isnt this stored in the vao? shouldnt need to do this every use only once i think. check that
		for (int i = 0; i < data.textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(i));
			glBindTexture(data.textures[i].getType(), data.textures[i].getTexture());
		}

		// Set uniforms
		for (const auto& uniform : data.uniforms) {
			uniform->setUniform(data.program);
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
			ResourcePath program = json::getString(doc, "shader_program")->value.GetString();
			auto params = json::getObject(doc, "parameters");

			ResourcePath albedo = json::getString(params->value, "albedo")->value.GetString();
			ResourcePath normal = json::getString(params->value, "normal")->value.GetString();
			ResourcePath roughness = json::getString(params->value, "roughness")->value.GetString();

			// Load the ShaderProgram 
			materialData.program = ShaderProgram2::load(program);



			// TODO: Need to make these auto detect what is used based on the .mat file
			// TODO: shouldnt i be useing ShaderProgram2 properties that i pre load?
			// TODO: make this simpler to add values to

			// Albedo
			materialData.uniforms.emplace_back(
				std::make_unique<UniformValue<GLint>>(
					materialData.program.getUniformLocation("albedoMap"),
					static_cast<GLint>(materialData.textures.size())
				)
			);
			materialData.textures.emplace_back(Texture::load(albedo));


			// Normal
			materialData.uniforms.emplace_back(
				std::make_unique<UniformValue<GLint>>(
					materialData.program.getUniformLocation("normalMap"),
					static_cast<GLint>(materialData.textures.size())
				)
			);
			materialData.textures.emplace_back(Texture::load(normal));

			// Roughness
			materialData.uniforms.emplace_back(
				std::make_unique<UniformValue<GLint>>(
					materialData.program.getUniformLocation("roughnessMap"),
					static_cast<GLint>(materialData.textures.size())
				)
			);
			materialData.textures.emplace_back(Texture::load(roughness));
		}

		return loadInfo.object;
	}
}