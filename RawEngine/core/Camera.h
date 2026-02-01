#pragma once
#include <glm/vec3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <GLFW/glfw3.h>
namespace core {

	class Camera {
	private:
		glm::mat4 view;
	public:

		glm::vec3 globalUp;
		glm::vec3 position;
		glm::vec3 lookPivot;
		glm::vec3 forward;
		glm::vec3 right;
		glm::vec3 up;

		//rotation stuff
		double mouseX, mouseY;
		float yaw,pitch;
		float mouseSensitivity;
		bool enableRotation = true;

		float fov = 45.0f;

		//deltatimers;
		double finishFrameTime;
		double	deltaTime;
		double	currentTime;



		//Initialization
		Camera();
		glm::mat4 GetViewMatrix() const;
		glm::mat4 GetProjectionMatrix(float aspectRatio, float nearPlane, float farPlane) const;
		void CameraMovement(GLFWwindow* window);
		void ProcessInput(GLFWwindow* window);
	};
}

