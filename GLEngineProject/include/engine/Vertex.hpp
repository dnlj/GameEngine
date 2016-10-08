#pragma once

#include <glm/glm.hpp>


namespace engine {
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec2 uv;
	};
}