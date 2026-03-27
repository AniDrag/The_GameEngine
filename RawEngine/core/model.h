#pragma once

#include <vector>
#include <glm/ext/matrix_float4x4.hpp>
#include "mesh.h"
#include "../Materials/material.h"
#include"../shaders/shader.h"
#include <memory>
namespace core {
    class Model {
    private:
        std::vector<core::Mesh> meshes;
        glm::mat4 modelMatrix{ 1.0f };
        
    public:
        std::shared_ptr<Shader> shader;
        std::shared_ptr<Material> material;
        Model(std::vector<core::Mesh> meshes) : meshes(meshes), modelMatrix(1) {};

        void render(GLenum drawMode) const;
        void position(glm::vec3& pPosition);
        void translate(glm::vec3& translation);
        void rotate(glm::vec3& axis, float radians);
        void scale(glm::vec3& scale);
        glm::mat4 getModelMatrix() const;
        const std::shared_ptr<Material>& getMaterial() const {
            return material;
        }
        void setMaterial(std::shared_ptr<Material> mat) { material = mat; }
        void attachShader( std::shared_ptr<Shader>& shader);
    };
};