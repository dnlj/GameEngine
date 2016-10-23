#pragma once

// GLFW
#include <GLFW\glfw3.h>

// GLM
#define GLM_FORCE_RADIANS
#include <glm\glm.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtx\transform.hpp>

namespace engine {
	class Camera {
		public:
			Camera(float fov, float aspect, float nearZ, float farZ);
			~Camera();

			void addPosition(glm::vec3 position);
			void setPosition(glm::vec3 position);
			void setOrientation(glm::quat orientation);
			void setFOV(float fov);
			void setNearZ(float nearZ);
			void setFarZ(float farZ);
			void setAspect(float aspect);

			glm::vec3 getPosition() const;
			glm::quat getOrientation() const;
			float getFOV() const;
			float getNearZ() const;
			float getFarZ() const;
			float getAspect() const;
			
			glm::vec3 getRight() const;
			glm::vec3 getUp() const;
			glm::vec3 getForward() const;

			glm::mat4 getViewMatrix() const;
			glm::mat4 getProjectionMaterix() const;

			void lookAt(glm::vec3 point);
			void handleInput(GLFWwindow *window, float dt);

		private:
			float fov;
			float aspect;
			float nearZ;
			float farZ;

			glm::vec3 position;
			glm::vec3 up;
			glm::quat orientation;
	};
}