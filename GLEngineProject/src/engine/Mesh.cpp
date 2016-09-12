#include <engine/Mesh.hpp>

namespace engine {
	Mesh::Mesh(engine::index idx) : index{idx} {
	}

	Mesh::~Mesh() {
	}

	// TODO: Eventually this render function should be removed and everything that needs to be rendered should be
	//			sorted by material/texture to reduce gl calls
	void Mesh::render() {
		const MeshData &data = meshes[index]; // TODO: Since this is a reference could this cause problems if the vector re-allocates?
		glBindVertexArray(data.vao);

		// TODO: need to make this handle multiple materials

		for (int i = 0; i < data.subMeshes.size(); i++) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.subMeshes[i].ebo);
			glDrawElements(GL_TRIANGLES, data.subMeshes[i].count, GL_UNSIGNED_INT, nullptr);
		}

		glBindVertexArray(0);
	}

	bool Mesh::operator==(const Mesh &mesh) const {
		return index == mesh.index;
	}

	////////////////////////////////////////////////////////////////
	// Static Stuff
	////////////////////////////////////////////////////////////////
	std::vector<MeshData> Mesh::meshes;
	std::unordered_map<std::string, engine::index> Mesh::pathLookup;

	// TODO: getting some memory warning stuff
	// TODO: Should get rid of scale and texture scale, that should not be part of the vbo/Mesh it shoudl be part of the transform
	Mesh Mesh::loadMesh(const std::string &path, const float scale, const float textureScale) {

		{
			auto it = pathLookup.find(path);
			if (it != pathLookup.end()) {
				return Mesh{it->second};
			}
		}

		Assimp::Importer importer;

		// TODO: Need to go over all theres params and see exactly what each does and if there needs to be any
		//			error checking or anything done with them.
		const aiScene *scene = importer.ReadFile(path,
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


		MeshData meshData = setupGLObjects(vertices, indices);
		std::cout << "\tMaterials: " << scene->mNumMaterials << std::endl; // TODO: Temp for testing, need to make  sure multiple materials work before removing

		// TODO: Need to add material to meshData

		engine::index index = static_cast<engine::index>(meshes.size());
		pathLookup[path] = index; // TODO: This path lookup is untested
		meshes.push_back(meshData);
		return Mesh{index};
	}

	MeshData Mesh::setupGLObjects(const std::vector<Vertex> &vertices, const std::vector<std::vector<GLuint>> &indices) {
		MeshData meshData{};
		meshData.subMeshes.resize(indices.size());


		glGenVertexArrays(1, &meshData.vao);
		glBindVertexArray(meshData.vao);

		glGenBuffers(1, &meshData.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, meshData.vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		// TODO: Temp for debugging
		std::cout << "vao: " << meshData.vao << std::endl;
		std::cout << "\tvbo: " << meshData.vbo << std::endl;


		for (unsigned int i = 0; i < indices.size(); i++) {
			glGenBuffers(1, &meshData.subMeshes[i].ebo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.subMeshes[i].ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(GLuint), &indices[i][0], GL_STATIC_DRAW);

			meshData.subMeshes[i].count = static_cast<GLsizei>(indices[i].size());

			// TODO: Temp for debugging
			std::cout << "\tebo: " << meshData.subMeshes[i].ebo << std::endl;
		}


		glBindVertexArray(0);
		return meshData;
	}

	void Mesh::cleanup() {
		for (const MeshData& mesh : meshes) { // TODO: since this is a reference to a vector element could this cause problems if the vector re-allocates?
			glDeleteVertexArrays(1, &mesh.vao);
			glDeleteBuffers(1, &mesh.vbo);

			for (auto meshData : mesh.subMeshes) {
				glDeleteBuffers(1, &meshData.ebo);
			}
		}

		meshes = {};
		meshes.shrink_to_fit();

		pathLookup = {};
		pathLookup.rehash(11);
	}
}
