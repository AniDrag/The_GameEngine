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
                if (!shader || !model->material) continue;

                shader->use();
                // set common uniforms (camera, MVP, lighting) – these are assumed to exist in the shader
                shader->setProperty("cameraPosition", sceneCamera.position);
                shader->setProperty("mvpMatrix", projection * view * model->getModelMatrix());
                shader->setProperty("lightPosition", lightDirection);
                shader->setProperty("ambientLightColor", lightColor);
                shader->setProperty("ambientLightIntensity", lightIntensity);
                shader->setProperty("uBloomThreshold", ppBloomThreshold); // is inside a FBO shader
                shader->setProperty("modelMatrix", model->getModelMatrix());

                // Bind material‑specific uniforms and textures
                model->material->bind(*shader);

                model->render(drawMode);
            }
        }
    
}