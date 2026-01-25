#pragma once
#include <vector>
#include <memory>
#include "model.h"
#include "camera.h"

namespace core {

    class Scene
    {
    public:
		//Scene() = default;
        void addModel(std::shared_ptr<Model> model);
        void render();

    private:
        std::vector<std::shared_ptr<Model>> models;

        unsigned int sceneFBO = 0;
        unsigned int sceneColorTex = 0;
		GLuint drawMode = GL_TRIANGLES;

        std::shared_ptr<Shader> postProcessShader;
    };

}
