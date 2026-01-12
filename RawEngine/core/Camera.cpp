#include "Camera.h"

#include <cstdio>        // for printf
#include <glm/gtc/matrix_transform.hpp> // for glm::lookAt
#include <imgui.h>
namespace core {

    Camera::Camera() {
        position = glm::vec3(0.0f, 0.0f, 10.0f);
        lookPivot = glm::vec3(0.0f, 0.0f, 0.0f);
        globalUp = glm::vec3(0.0f, 1.0f, 0.0f);

        forward = glm::vec3(0, 0, -1);//glm::normalize(position - lookPivot);
        right = glm::normalize(glm::cross(globalUp, forward));
        up = glm::cross(forward, right);

        yaw = -90.0f;    // looking down -Z
        pitch = 0.0f;
        mouseSensitivity = 0.1f;

        view = glm::mat4(1);
    }

    glm::mat4 Camera::GetViewMatrix() const {
        return view;
    }
   //void Camera::CameraMovement(GLFWwindow* window) {
   //    // Recalculate the direction vectors ONCE per frame
   //    
   //    forward = glm::normalize(lookPivot - position);
   //    right = glm::normalize(glm::cross(forward, globalUp));
   //    up = glm::normalize(glm::cross(right, forward));
   //
   //
   //    // W/S - forward/backward
   //    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
   //        position += 0.01f * forward;
   //        printf("Cam y:%f\n", position.z);
   //    }
   //    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
   //        position -= 0.01f * forward;
   //        printf("Cam y:%f\n", position.z);
   //    }
   //
   //    // A/D - strafe left/right
   //    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
   //        position -= 0.01f * right;
   //        printf("Cam y:%f\n", position.x);
   //    }
   //    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
   //        position += 0.01f * right;
   //        printf("Cam y:%f\n", position.x);
   //    }
   //
   //
   //    // Q/E - up/down
   //    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
   //        position += 0.01f * up;
   //        printf("Cam y:%f\n", position.y);
   //    }
   //    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
   //        position -= 0.01f * up;
   //        printf("Cam y:%f\n", position.y);
   //    }
   //
   //    // Update lookPivot and view matrix
   //    lookPivot = position + forward;
   //    view = glm::lookAt(position, lookPivot, up);
   //}
    void Camera::CameraMovement(GLFWwindow* window) {
        // --------- time and deltaTime ---------
        finishFrameTime = glfwGetTime();
        deltaTime = static_cast<float>(finishFrameTime - currentTime);
        currentTime = finishFrameTime;

        // ---------- Mouse input ----------
        static double lastX = 0.0, lastY = 0.0;
        static bool firstMouse = true;

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        if (firstMouse) {
            lastX = mouseX;
            lastY = mouseY;
            firstMouse = false;
        }

        float dx = float(mouseX - lastX);
        float dy = float(lastY - mouseY); // invert Y

        lastX = mouseX;
        lastY = mouseY;

        const float sensitivity = 0.0025f;

        float yaw = dx * sensitivity;
        float pitch = dy * sensitivity;

        // ---------- Direction from camera to pivot ----------
        glm::vec3 dir = glm::normalize(lookPivot - position);

        // ---------- Build camera basis ----------
        right = glm::normalize(glm::cross(dir, globalUp));
        up = glm::normalize(glm::cross(right, dir));

        // ---------- YAW (world up) ----------
        dir = glm::normalize(
            glm::vec3(
                glm::rotate(glm::mat4(1.0f), -yaw, globalUp)
                * glm::vec4(dir, 0.0f)
            )
        );

        // ---------- PITCH (camera right) ----------
        glm::vec3 pitched =
            glm::vec3(
                glm::rotate(glm::mat4(1.0f), pitch, right)
                * glm::vec4(dir, 0.0f)
            );

        // Prevent vertical flip
        float verticalDot = glm::dot(glm::normalize(pitched), globalUp);
        if (verticalDot < 0.99f && verticalDot > -0.99f)
            dir = glm::normalize(pitched);

        // ---------- Apply rotation ----------
        if (enableRotation) {
            forward = dir;
            lookPivot = position + forward;
        }

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

        // ---------- Re-sync pivot after movement ----------
        lookPivot = position + forward;

        // ---------- View matrix ----------
        view = glm::lookAt(position, lookPivot, up);
    }


    

}

