#include <engine/ModelData.hpp>

namespace engine {
	void ModelData::deleteData() {
		std::cout << "Delete model: " << vbo << "\n";
		
		for (auto data : subModels) {
			glDeleteBuffers(1, &data.ebo);
		}
		
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}
}