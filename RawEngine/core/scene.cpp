#include "scene.h"

namespace core
{
        Scene::Scene(GLFWwindow * window)
            : window(window)
        {
            int w, h;
            glfwGetFramebufferSize(window, &w, &h);
            screenSize = glm::vec2((float)w, (float)h);
            sceneCamera.position = glm::vec3(0.0f, 0.0f, 5.0f);
            sceneCamera.lookPivot = glm::vec3(0.0f);
        }

        void Scene::addModel(std::shared_ptr<Model> model)
        {
            models.push_back(std::move(model));
			modelIndex++;
        }

        void Scene::render()
        {
            // update screen size every frame (window resize safe)
            int w, h;
            glfwGetFramebufferSize(window, &w, &h);
            screenSize = glm::vec2((float)w, (float)h);

            sceneCamera.CameraMovement(window);
            glm::mat4 view = sceneCamera.GetViewMatrix();
			//Safety cuz i get null cheks, too fast fot the computer i guess 
            float aspect = 1.0f;
            if (screenSize.y > 0.0f)
            {
                aspect = screenSize.x / screenSize.y;
            }
            glm::mat4 projection = sceneCamera.GetProjectionMatrix(aspect, 0.1f, 100.0f); // ratio, near plane, far plane

            for (const auto& model : models)
            {
                // model MUST already have a shader attached
                auto shader = model->shader;
                if (!shader) continue;

                shader->use();

                // camera
                shader->setProperty("cameraPosition", sceneCamera.position);
                glm::mat4 mvp = projection * view * model->getModelMatrix();
                shader->setProperty("mvpMatrix", mvp);

                //global lighting
                shader->setProperty("lightPosition", lightDirection);
                shader->setProperty("ambientLightColor", lightColor);
                shader->setProperty("ambientLightIntensity", lightIntensity);

                // 2️ Per-model uniform
                model->shader->setProperty("modelMatrix", model->getModelMatrix());

                // 3️ Bind material (textures + material uniforms)
                model->material.bind(*shader);
                model->render(drawMode);
            }
        }
    
}