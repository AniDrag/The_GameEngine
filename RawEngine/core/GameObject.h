#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "core/model.h"
namespace core { class Model; }

namespace core {

    class GameObject {
    private:
        std::shared_ptr<core::Model> model;   
        GLuint textureID = 0;                 
    public:
        GameObject() = default;

        
        explicit GameObject(std::shared_ptr<core::Model> mdl, GLuint tex = 0)
            : model(std::move(mdl)), textureID(tex) {
        }

        
        GameObject(const GameObject&) = delete;
        GameObject& operator=(const GameObject&) = delete;

        // allow moving
        GameObject(GameObject&&) = default;
        GameObject& operator=(GameObject&&) = default;

        // model access
        void assignModel(std::shared_ptr<core::Model> mdl) { model = std::move(mdl); }
        std::shared_ptr<core::Model> getModel() const { return model; }

        // texture access
        void setTexture(GLuint tex) { textureID = tex; }
        GLuint getTexture() const { return textureID; }

        // main rendering call (implemented in GameObject.cpp)
        void render(GLuint shaderProgram,
            GLenum drawmode,
            const glm::mat4& view,
            const glm::mat4& proj) const;    
    };
};

