#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/vec3.hpp>
#include<glm/gtx/normalize_dot.hpp>
#include <glm/ext/matrix_float4x4.hpp>
namespace core{

	class Camera{
	private:
		glm::mat4 camMVP;
	public:
		

		glm::vec3 globalUp;
		glm::vec3 position;
		glm::vec3 lookPivot;
		glm::vec3 forward;
		glm::vec3 right;
		glm::vec3 up;

		//Initialization
		Camera() {
			position = glm::vec3(0.0f, 0.0f, 10.0f);
			lookPivot = glm::vec3(0.0f, 0.0f, 0.0f);
			forward = glm::vec3(0, 0, -1);//glm::normalize(position - lookPivot);
			globalUp = glm::vec3(0.0f, 1.0f, 0.0f);
			right = glm::normalize(glm::cross(globalUp, forward));
			up = glm::cross(forward, right);
			camMVP;
		}

		glm::mat4 GetModelViewMatrix() const;
	};
}

