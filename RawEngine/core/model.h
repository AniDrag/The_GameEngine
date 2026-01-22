#pragma once

#include <vector>
#include <glm/ext/matrix_float4x4.hpp>
#include "mesh.h"
#include "material.h"
#include"../shaders/shader.h"
#include <memory>
namespace core {
    class Model {
    private:
        std::vector<core::Mesh> meshes;
        glm::mat4 modelMatrix{ 1.0f };
        std::shared_ptr<Shader> shader;
    public:
        Model(std::vector<core::Mesh> meshes) : meshes(meshes), modelMatrix(1) {}
        core::Material material;

        void render(GLenum drawMode) const;
        void position(glm::vec3& position);
        void translate(glm::vec3& translation);
        void rotate(glm::vec3& axis, float radians);
        void scale(glm::vec3& scale);
        glm::mat4 getModelMatrix() const;
        const core::Material& getMaterial() const {
            return material;
        }
        void attachShader(const std::shared_ptr<Shader>& shader) const;
    }
}