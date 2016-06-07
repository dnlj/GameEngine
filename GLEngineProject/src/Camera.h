#ifndef CAMERA_H
#define CAMERA_H

// Includes
#include <iostream>

// GLFW
#include <GLFW\glfw3.h>

// GLM
#define GLM_FORCE_RADIANS
#include <glm\glm.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtx\transform.hpp>

class Camera {
	public:
		Camera();
		~Camera();
		
		void addPosition(glm::vec3 pos);
		void setPosition(glm::vec3 pos);
		void setOrientation(glm::quat orient);
		glm::vec3 Camera::getPosition() const;

		void handleInput(GLFWwindow *window, float dt);

		glm::vec3 getRight() const;
		glm::vec3 getUp() const;
		glm::vec3 getForward() const;

		glm::quat Camera::getOrientation() const;

		glm::mat4 getViewMatrix() const;

	private:
		float camRoll;

		glm::vec3 position;
		glm::vec3 up;
		glm::quat orientation;
};

#endif // CAMERA_H