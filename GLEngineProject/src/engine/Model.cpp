// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Engine
#include <engine/Model.hpp>
#include <engine/util/util.hpp>


namespace engine {
	Model::~Model() {
	}

	// TODO: Eventually this render function should be removed and everything that needs to be rendered should be
	//			sorted by material/texture to reduce gl calls
	void Model::render(const glm::mat4& mvp, const glm::mat4& model, const Camera& camera, const glm::vec3& lightPosition) const {
		const ModelData &modelData = getDataAt(index); // TODO: Since this is a reference could this cause problems if the vector re-allocates?

		glBindVertexArray(modelData.vao);

		// TODO: need to make this handle multiple materials

		// Draw all sub models
		for (const auto& subModel : modelData.subModels) {
			subModel.material.use();
			// TODO: some kind of cacheing so you dont have to call this if the last draw was of the same material/program
			const auto& program = subModel.material.getProgram();

			glUseProgram(program.getProgram()); // TODO: this gets rid of some errors
			// Probably just revert back to the old system and convert over pice by pice instaed of all at once
			// eneed to do this stuff
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, 1);

			// TODO: move these calls into the program class. program.setUniform(name, matrix)
			glUniform3fv(program.getUniformIndex("viewPos"), 1, &camera.getPosition()[0]);
			glUniform3fv(program.getUniformIndex("lightPos"), 1, &lightPosition[0]);

			glUniformMatrix4fv(program.getUniformIndex("mvp"), 1, GL_FALSE, &mvp[0][0]);
			glUniformMatrix4fv(program.getUniformIndex("modelMatrix"), 1, GL_FALSE, &model[0][0]);

			// TODO: need to make these controlled by imgui again
			glUniform1f(program.getUniformIndex("metalness"), 0.0f);
			glUniform1f(program.getUniformIndex("intensity"), 1.0f);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subModel.ebo);
			glDrawElements(GL_TRIANGLES, subModel.count, GL_UNSIGNED_INT, nullptr);
		}

		glBindVertexArray(0);
	}

	// TODO: Move this into the Resource base class?
	bool Model::operator==(const Model &model) const {
		return index == model.index;
	}

	////////////////////////////////////////////////////////////////
	// Static
	////////////////////////////////////////////////////////////////
	Model Model::load(const ResourcePath& path, const float scale, const float textureScale) {
		const auto resolvedPath = path.getResolvedPath();
		auto& loadInfo = loadResource(resolvedPath);

		// Load the object if it has not already been loaded
		if (!loadInfo.alreadyLoaded) {
			Assimp::Importer importer;

			// TODO: Need to go over all theres params and see exactly what each does and if there needs to be any
			//			error checking or anything done with them.
			const aiScene *scene = importer.ReadFile(resolvedPath,
				aiProcess_GenSmoothNormals |
				aiProcess_JoinIdenticalVertices |
				aiProcess_ImproveCacheLocality |
				aiProcess_RemoveRedundantMaterials |
				aiProcess_Triangulate |
				aiProcess_GenUVCoords |
				aiProcess_SortByPType | // TODO: figure out how to use settings for this so you can exclude point and line primatives
				aiProcess_FindInvalidData |
				aiProcess_OptimizeMeshes |
				aiProcess_Debone |
				aiProcess_CalcTangentSpace |
				//aiProcess_ValidateDataStructure	|
				//aiProcess_FindDegenerates			|
				//aiProcess_SplitLargeMeshes		|
				0);

			// TODO: More assimp error checking, lookinto this
			// TODO: More error checking and make it load an error model (like source)
			//			as well as print something to the debug console
			if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
				std::cout << "[ERROR - ASSIMP] " << importer.GetErrorString() << std::endl;
			}

			size_t vertexReserveSize = 0;
			std::vector<std::size_t> indexReserveSizes;
			indexReserveSizes.resize(scene->mNumMaterials);

			// NOTE: After doing some testing, I found having this extra loop and reserving to be faster than without.
			for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
				const aiMesh *mesh = scene->mMeshes[meshIndex];
				vertexReserveSize += mesh->mNumVertices;
				indexReserveSizes[mesh->mMaterialIndex] += mesh->mNumFaces * 3;
			}


			std::vector<unsigned int> materials; // TODO: add another loop to populate the materials vector
			std::vector<std::vector<GLuint>> indices; // NOTE: Could do a reserve on each sub vector to make this faster
			std::vector<Vertex> vertices;

			vertices.reserve(vertexReserveSize);
			materials.reserve(scene->mNumMaterials);
			indices.resize(scene->mNumMaterials);

			for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
				indices[i].reserve(indexReserveSizes[i]);
			}

			unsigned int indexOffset = 0;

			for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
				const aiMesh *mesh = scene->mMeshes[meshIndex];

				for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
					const aiFace *face = &mesh->mFaces[faceIndex];

					indices[mesh->mMaterialIndex].emplace_back(face->mIndices[0] + indexOffset);
					indices[mesh->mMaterialIndex].emplace_back(face->mIndices[1] + indexOffset);
					indices[mesh->mMaterialIndex].emplace_back(face->mIndices[2] + indexOffset);
				}


				for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++) {
					engine::Vertex vert{};

					vert.position.x = mesh->mVertices[vertexIndex].x * scale;
					vert.position.y = mesh->mVertices[vertexIndex].y * scale;
					vert.position.z = mesh->mVertices[vertexIndex].z * scale;

					vert.normal.x = mesh->mNormals[vertexIndex].x;
					vert.normal.y = mesh->mNormals[vertexIndex].y;
					vert.normal.z = mesh->mNormals[vertexIndex].z;

					vert.tangent.x = mesh->mTangents[vertexIndex].x;
					vert.tangent.y = mesh->mTangents[vertexIndex].y;
					vert.tangent.z = mesh->mTangents[vertexIndex].z;

					vert.uv.x = mesh->mTextureCoords[0][vertexIndex].x * textureScale;
					vert.uv.y = mesh->mTextureCoords[0][vertexIndex].y * textureScale;

					vertices.push_back(vert);

					indexOffset++;
				}
			}

			loadInfo.data = setupGLObjects(vertices, indices);

			std::cout << "\tMaterials: " << scene->mNumMaterials << std::endl; // TODO: Temp for testing, need to make  sure multiple materials work before removing

			// TODO: Need to add material to meshData
		}

		return loadInfo.object;
	}

	ModelData Model::setupGLObjects(const std::vector<Vertex> &vertices, const std::vector<std::vector<GLuint>> &indices) {
		ModelData modelData{};
		modelData.subModels.resize(indices.size());


		glGenVertexArrays(1, &modelData.vao);
		glBindVertexArray(modelData.vao);

		glGenBuffers(1, &modelData.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, modelData.vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		// TODO: Temp for debugging
		std::cout << "vao: " << modelData.vao << std::endl;
		std::cout << "\tvbo: " << modelData.vbo << std::endl;


		// TODO: Temp while testing new material system
		Material2 material = Material2::load("Materials:tiles.mat");


		for (unsigned int i = 0; i < indices.size(); i++) {
			glGenBuffers(1, &modelData.subModels[i].ebo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelData.subModels[i].ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(GLuint), &indices[i][0], GL_STATIC_DRAW);

			modelData.subModels[i].count = static_cast<GLsizei>(indices[i].size());
			modelData.subModels[i].material = material;

			// TODO: Temp for debugging
			std::cout << "\tebo: " << modelData.subModels[i].ebo << std::endl;
		}

		glBindVertexArray(0);

		setupVertexAttributes(modelData);

		return modelData;
	}

	void Model::setupVertexAttributes(const ModelData& modelData) {
		glBindVertexArray(modelData.vao);
		glBindBuffer(GL_ARRAY_BUFFER, modelData.vbo);

		// TODO: Wouldl like to handle this better, look at ogre3d. For now just used fixed positions

		// vertPosition
		GLint location = 0;
		glEnableVertexAttribArray(location);
		glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, sizeof(engine::Vertex), (GLvoid*)offsetof(engine::Vertex, position));

		// vertNormal
		location = 1;
		glEnableVertexAttribArray(location);
		glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, sizeof(engine::Vertex), (GLvoid*)offsetof(engine::Vertex, normal));

		// vertTangent
		location = 2;
		glEnableVertexAttribArray(location);
		glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, sizeof(engine::Vertex), (GLvoid*)offsetof(engine::Vertex, tangent));

		// vertTexCoord
		location = 3;
		glEnableVertexAttribArray(location);
		glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, sizeof(engine::Vertex), (GLvoid*)offsetof(engine::Vertex, uv));

		std::cout << "vao: " << modelData.vao << std::endl;
		std::cout << "\tvbo: " << modelData.vbo << std::endl;
		for (int j = 0; j < modelData.subModels.size(); j++) {
			std::cout << "\tebo: " << modelData.subModels[j].ebo << std::endl;
		}

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}