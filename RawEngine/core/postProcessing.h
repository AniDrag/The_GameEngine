#pragma once
#include <vector>
#include <memory>
#include "model.h"
#include "camera.h"
#include "../shaders/shader.h"

namespace core {

    class PostProcessing
    {
    public:
        // constructor needs window to query size
        PostProcessing(GLFWwindow* window, GLuint textureBuffer);

        void addShader(std::shared_ptr<Shader> Shader);
        void render();
        void ImGuiRender();


        // window / screen size
        glm::vec2 screenSize{ 800.0f, 600.0f };

    private:
        GLFWwindow* window = nullptr;

    };

}
#pragma once
