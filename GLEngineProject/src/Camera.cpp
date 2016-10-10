#include "Camera.h"


Camera::Camera() : up(0.0f, 1.0f, 0.0f), camRoll(0.0f) {
}

Camera::~Camera() {
}

void Camera::addPosition(glm::vec3 pos) {
	setPosition(position + pos);
}

void Camera::setPosition(glm::vec3 pos) {
	position = pos;
}

void Camera::setOrientation(glm::quat orient) {
	orientation = orient;
}

glm::vec3 Camera::getPosition() const {
	return position;
}

glm::vec3 Camera::getRight() const {
	return glm::inverse(getOrientation()) * glm::vec3(1.0f, 0.0f, 0.0f);
}

glm::vec3 Camera::getUp() const {
	return glm::inverse(getOrientation()) * glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 Camera::getForward() const {
	return glm::inverse(getOrientation()) * glm::vec3(0.0f, 0.0f, -1.0f);
}

void Camera::handleInput(GLFWwindow *window, float dt) {
	static double storedMouseX = 0.0f;
	static double storedMouseY = 0.0f;
	static bool mouseWasDown = false;
	static float sensitivity = 15.0f;
	static float moveSpeed = 4.0f;

	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_4)) {
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
			moveSpeed =  16.0f;
		} else {
			moveSpeed =  8.0f;
		}

		int w = glfwGetKey(window, GLFW_KEY_W);
		int a = glfwGetKey(window, GLFW_KEY_A);
		int s = glfwGetKey(window, GLFW_KEY_S);
		int d = glfwGetKey(window, GLFW_KEY_D);

		if (w && !s) {
			addPosition(getForward() * moveSpeed * dt);
		}
		if (a && !d) {
			addPosition(getRight() * -moveSpeed * dt);
		}
		if (s && !w) {
			addPosition(getForward() * -moveSpeed * dt);
		}
		if (d && !a) {
			addPosition(getRight() * moveSpeed * dt);
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			addPosition(getUp() * moveSpeed * dt);
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
			addPosition(getUp() * -moveSpeed * dt);
		}

		if (!mouseWasDown) {
			glfwSetCursorPos(window, 0.0, 0.0);
			mouseX = 0.0f;
			mouseY = 0.0f;
		}

		mouseWasDown = true;

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// TODO: This borks when you dont have vsync on, this is problem
		float deltaX = static_cast<float>(mouseX * sensitivity * dt);
		float deltaY = static_cast<float>(mouseY * sensitivity * dt);

		glm::quat pitch	= glm::angleAxis(glm::radians(deltaY), getRight());
		glm::quat yaw	= glm::angleAxis(glm::radians(deltaX), up);

		//if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) && !glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
		//	camRoll += 0.5f * dt;
		//} else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) && !glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
		//	camRoll -= 0.5f * dt;
		//}

		orientation = orientation * pitch * yaw;

		glfwSetCursorPos(window, 0.0, 0.0);
	} else {
		if (mouseWasDown) {
			glfwSetCursorPos(window, storedMouseX, storedMouseY);
		}

		mouseWasDown = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		storedMouseX = mouseX;
		storedMouseY = mouseY;
	}
}

glm::quat Camera::getOrientation() const {
	return orientation;
}

glm::mat4 Camera::getViewMatrix() const {
	return glm::mat4_cast(orientation * glm::angleAxis(camRoll, getForward())) * glm::translate(glm::mat4(), -position);
}
