#include "scene.h"
#include <imgui.h>

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
                auto shader = model->material->shader;
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
                //model->material->bind();

                model->render(drawMode);
            }
        }
    

        void Scene::ImGuiRender()
        {
        // ------------------------------------------------
        // Camera
        // ------------------------------------------------
            if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {

                //auto& cam = activeScene->sceneCamera;

                ImGui::Checkbox("Enable Rotation", &sceneCamera.enableRotation);
                ImGui::SliderFloat("Mouse Sensitivity", &sceneCamera.mouseSensitivity, 0.001f, 0.2f, "%.4f");

                ImGui::Separator();
                ImGui::Text("Position");
                ImGui::DragFloat3("##CamPos", &sceneCamera.position.x, 0.05f);

            }

            // ------------------------------------------------
            // Lighting
            // ------------------------------------------------
            if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen)) {

                ImGui::Text("Directional Light");


                ImGui::DragFloat3(
                    "Direction",
                    &lightDirection.x,
                    0.05f, -1.0f, 1.0f
                );

                ImGui::ColorEdit3(
                    "Color",
                    &lightColor.x
                );

                ImGui::SliderFloat(
                    "Intensity",
                    &lightIntensity,
                    0.01f, 1.0f
                );

            }

            // ------------------------------------------------
            // Render Settings
            // ------------------------------------------------
            if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen)) {
                const char* drawModes[] = {
                    "Triangles", "Wireframe", "Points",
                    "Line Strip", "Line Loop",
                    "Triangle Strip", "Triangle Fan"
                };

                static int currentDrawMode = 0;

                if (ImGui::Combo("Draw Mode", &currentDrawMode, drawModes, IM_ARRAYSIZE(drawModes))) {
                    switch (currentDrawMode) {
                    case 0: drawMode = GL_TRIANGLES; break;
                    case 1: drawMode = GL_LINES; break;
                    case 2: drawMode = GL_POINTS; break;
                    case 3: drawMode = GL_LINE_STRIP; break;
                    case 4: drawMode = GL_LINE_LOOP; break;
                    case 5: drawMode = GL_TRIANGLE_STRIP; break;
                    case 6: drawMode = GL_TRIANGLE_FAN; break;
                    }
                }

            }
		}
}