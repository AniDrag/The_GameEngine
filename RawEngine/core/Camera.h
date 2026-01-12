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
		float yaw;
		float pitch;
		float mouseSensitivity;
		bool enableRotation = true;

		//deltatimers;
		float finishFrameTime;
		float	deltaTime;
		float	currentTime;

		float buttonTimer;


		//Initialization
		Camera();

		glm::mat4 GetViewMatrix() const;
		void CameraMovement(GLFWwindow* window);
	};
}

