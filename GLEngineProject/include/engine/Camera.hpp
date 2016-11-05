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
			/**
			 * Constructor.
			 * 
			 * @param fov The fov to set Camera::fov to.
			 * @param aspect The aspect to set Camera::aspect to.
			 * @param nearZ The nearZ to set Camera::nearZ to.
			 * @param farZ The farZ to set Camera::farZ to.
			 */
			Camera(float fov, float aspect, float nearZ, float farZ);

			/**
			 * Deconstructor.
			 */
			~Camera();

			/**
			 * Adds @p position to the current position.
			 * 
			 * @param[in] position The position to add to the current position.
			 * @return void
			 */
			void addPosition(glm::vec3 position);

			/**
			 * Sets the position of the Camera to @p position.
			 *
			 * @param[in] position The position to set the Camera to.
			 * @return void
			 */
			void setPosition(glm::vec3 position);

			/**
			 * Sets the orientatio of the Camera to @p orientation.
			 *
			 * @param[in] orientation The orientation to set the Camera to.
			 * @return void
			 */
			void setOrientation(glm::quat orientation);

			/**
			 * Sets the field of view of the Camera to @p fov.
			 *
			 * @param[in] fov The field of view to use for the Camera.
			 * @return void
			 */
			void setFOV(float fov);

			/**
			 * Sets the near Z clipping plane distance for the Camera to @p nearZ.
			 *
			 * @param[in] nearZ The near Z clipping plane distance to use for the Camera.
			 * @return void
			 */
			void setNearZ(float nearZ);

			/**
			 * Sets the far Z clipping plane distance for the Camera to @p farZ.
			 *
			 * @param[in] farZ The far Z clipping plane distance to use for the Camera.
			 * @return void
			 */
			void setFarZ(float farZ);

			/**
			 * Sets the aspect ratio for the Camera to @p aspect.
			 *
			 * @param[in] aspect The aspect ratio to use for the Camera.
			 * @return void
			 */
			void setAspect(float aspect);

			/**
			 * Gets the position of the Camera.
			 * 
			 * @return The position of the Camera.
			 */
			glm::vec3 getPosition() const;

			/**
			 * Gets the orientation of the Camera.
			 *
			 * @return The orientation of the Camera.
			 */
			glm::quat getOrientation() const;

			/**
			 * Gets the field of view of the Camera.
			 *
			 * @return The field of view of the Camera.
			 */
			float getFOV() const;

			/**
			 * Gets the near Z clipping plane distance of the Camera.
			 *
			 * @return The near Z clipping plane distance of the Camera.
			 */
			float getNearZ() const;

			/**
			 * Gets the far Z clipping plane distance of the Camera.
			 *
			 * @return The far Z clipping plane distance of the Camera.
			 */
			float getFarZ() const;
			/**
			 * Gets the aspect ratio of the Camera.
			 *
			 * @return The aspect ratio of the Camera.
			 */
			float getAspect() const;
			
			/**
			 * Gets the normalized world space right vector of the Camera.
			 * 
			 * @return The normalized world space right vector of the Camera.
			 */
			glm::vec3 getRight() const;

			/**
			 * Gets the normalized world space up vector of the Camera.
			 *
			 * @return The normalized world space up vector of the Camera.
			 */
			glm::vec3 getUp() const;

			/**
			 * Gets the normalized world space forward vector of the Camera.
			 *
			 * @return The normalized world space forward vector of the Camera.
			 */
			glm::vec3 getForward() const;

			/**
			 * Gets the view matrix representing the Camera.
			 * 
			 * @return The view matrix representing the Camera.
			 */
			glm::mat4 getViewMatrix() const;

			/**
			 * Gets the projection matrix for the Camera.
			 * 
			 * @return The projection matrix for the Camera
			 */
			glm::mat4 getProjectionMaterix() const;

			/**
			 * Sets the orientation of the Camera to look at @p point.
			 *
			 * @return void
			 */
			void lookAt(glm::vec3 point);

			/**
			 * Handles all movement of the Camera.
			 * 
			 * @return void
			 */
			void handleInput(GLFWwindow *window, float dt);

		private:
			/** The field of view for the Camera. */
			float fov;

			/** The aspect ratio for the Camera. */
			float aspect;

			/** The near Z clipping plane for the Camera. */
			float nearZ;

			/** The far Z clipping plane for the Camera. */
			float farZ;

			/** The position of the Camera. */
			glm::vec3 position;

			/** The world space up vector to use for the Camera. */
			glm::vec3 up;

			/** The orientation of the Camera. */
			glm::quat orientation;
	};
}