#include "scene.h"

namespace core
{
    void Scene::render()
    {
        // 1. render scene to framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (const auto& model : models)
            model->render(GL_TRIANGLES);

        // 2. post-process
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        postProcessShader->use();
        glBindTexture(GL_TEXTURE_2D, sceneColorTex);

        //renderFullscreenQuad();
    }
    void Scene::addModel(std::shared_ptr<Model> model)
    {
        models.push_back(std::move(model));
    }
}