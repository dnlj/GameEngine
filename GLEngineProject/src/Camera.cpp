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
	static float moveSpeed		= 4.0f;
	static float sensitivity	= 15.0f;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
		moveSpeed =  8.0f;
	} else {
		moveSpeed =  4.0f;
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
	
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	// TODO: This borks when you dont have vsync on, this is problem
	mouseX = mouseX * sensitivity * dt;
	mouseY = mouseY * sensitivity * dt;

	glm::quat pitch	= glm::angleAxis(glm::radians((float)mouseY), getRight());
	glm::quat yaw	= glm::angleAxis(glm::radians((float)mouseX), up);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) && !glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
		camRoll += 0.5f * dt;
	} else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) && !glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
		camRoll -= 0.5f * dt;
	}

	orientation = orientation * pitch * yaw;

	glfwSetCursorPos(window, 0.0, 0.0);
}

glm::quat Camera::getOrientation() const {
	return orientation;
}

glm::mat4 Camera::getViewMatrix() const {
	return glm::mat4_cast(orientation * glm::angleAxis(camRoll, getForward())) * glm::translate(glm::mat4(), -position);
}
