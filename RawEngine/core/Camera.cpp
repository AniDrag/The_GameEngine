#include "Camera.h"

#include <cstdio>        // for printf
#include <glm/gtc/matrix_transform.hpp> // for glm::lookAt
#include <imgui.h>

namespace core {

    Camera::Camera() {
        view = glm::mat4(1);

        position = glm::vec3(0.0f, 0.0f, 10.0f);
        lookPivot = glm::vec3(0.0f, 0.0f, 0.0f);
        globalUp = glm::vec3(0.0f, 1.0f, 0.0f);

        forward = glm::vec3(0, 0, -1);//glm::normalize(position - lookPivot);
        right = glm::normalize(glm::cross(globalUp, forward));
        up = glm::cross(forward, right);

        mouseX = 0;
        mouseY = 0;
        yaw = -90.0f;    // looking down -Z
        pitch = 0.0f;
        mouseSensitivity = 0.004f;
        enableRotation = true;

        // deltatimers;
        finishFrameTime = 0;
        deltaTime = 0;
        currentTime = 0;

    }

    glm::mat4 Camera::GetViewMatrix() const {
        return view;
    }
    glm::mat4 Camera::GetProjectionMatrix(float aspectRatio, float nearPlane, float farPlane) const {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
	}
   // void Camera::CameraMovement(GLFWwindow* window) {
   //     // Recalculate the direction vectors ONCE per frame
   //
   //     forward = glm::normalize(lookPivot - position);
   //     right = glm::normalize(glm::cross(forward, globalUp));
   //     up = glm::normalize(glm::cross(right, forward));
   //
   //
   //     // W/S - forward/backward
   //     if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
   //         position += 0.01f * forward;
   //         printf("Cam y:%f\n", position.z);
   //     }
   //     else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
   //         position -= 0.01f * forward;
   //         printf("Cam y:%f\n", position.z);
   //     }
   //
   //     // A/D - strafe left/right
   //     if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
   //         position -= 0.01f * right;
   //         printf("Cam y:%f\n", position.x);
   //     }
   //     else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
   //         position += 0.01f * right;
   //         printf("Cam y:%f\n", position.x);
   //     }
   //
   //
   //     // Q/E - up/down
   //     if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
   //         position += 0.01f * up;
   //         printf("Cam y:%f\n", position.y);
   //     }
   //     else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
   //         position -= 0.01f * up;
   //         printf("Cam y:%f\n", position.y);
   //     }
   //
   //     // Update lookPivot and view matrix
   //     lookPivot = position + forward;
   //     view = glm::lookAt(position, lookPivot, up);
   // }
    void Camera::CameraMovement(GLFWwindow* window) {
        // Process inputs
        ProcessInput(window);

        // Calculate forward direction from yaw and pitch
        glm::vec3 dir;
        dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        dir.y = sin(glm::radians(pitch));
        dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        // Recalculate basis vectors
        forward = glm::normalize(dir);
        right = glm::normalize(glm::cross(forward, globalUp));
        up = glm::normalize(glm::cross(right, forward));

        // Update lookPivot and view matrix
        lookPivot = position + forward;
        view = glm::lookAt(position, lookPivot, up);

    }
    void Camera::ProcessInput(GLFWwindow* window) {
        // --------- time and deltaTime ---------
        finishFrameTime = glfwGetTime();
        deltaTime = static_cast<float>(finishFrameTime - currentTime);
        currentTime = finishFrameTime;
        // ---------- Keyboard movement ----------

        const float speed = 5.0f * deltaTime;

        
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            position += speed * forward;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            position -= speed * forward;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            position -= speed * right;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            position += speed * right;

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            position += speed * globalUp;

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            position -= speed * globalUp;

        // ---------- Mouse input ----------
        //efficiency sake so we dont calculate for no reason repetedly a non movable camera
        if (enableRotation) {
            static double lastX = 0.0, lastY = 0.0;
            static bool firstMouse = true;

            glfwGetCursorPos(window, &mouseX, &mouseY);

            if (firstMouse) {
                lastX = mouseX;
                lastY = mouseY;
                firstMouse = false;
            }

            float dx = static_cast<float>(mouseX - lastX);
            float dy = static_cast<float>(lastY - mouseY); // inverted Y

            lastX = mouseX;
            lastY = mouseY;

            // FIX: Use += to accumulate rotation, not = to set it
            yaw += dx * mouseSensitivity;
            pitch += dy * mouseSensitivity;

            // Constrain pitch to prevent flip
            if (pitch > 89.0f) pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;
        }
        // ---------- TAB key edge detection ----------
        static bool tabWasDown = false;
        bool tabIsDown = glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS;

        if (tabIsDown && !tabWasDown) {
            enableRotation = !enableRotation;

            // Update cursor mode
            glfwSetInputMode(
                window,
                GLFW_CURSOR,
                enableRotation ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL
            );
            ImGui::GetIO().MouseDrawCursor = !enableRotation;

        }
        tabWasDown = tabIsDown;
    }
}