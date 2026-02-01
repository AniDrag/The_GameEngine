#pragma once
#include <vector>
#include <memory>
#include "model.h"
#include "camera.h"

namespace core {

    class Scene
    {
    public:
        // constructor needs window to query size
        Scene(GLFWwindow* window);

        void addModel(std::shared_ptr<Model> model);
        void render();

        // camera owned by scene
        Camera sceneCamera;

        // window / screen size
        glm::vec2 screenSize{ 800.0f, 600.0f };

        // light attributes (simple, extensible later)
        glm::vec3 lightDirection{ 1.0f, -1.0f, 0.0f };
        glm::vec3 lightColor{ 1.0f, 1.0f, 1.0f };
        float lightIntensity{ 0.1f };

        GLuint drawMode = GL_TRIANGLES;
		int modelIndex = 0;// amount of models
		
    private:
        GLFWwindow* window = nullptr;
        std::vector<std::shared_ptr<Model>> models;

    };

}
