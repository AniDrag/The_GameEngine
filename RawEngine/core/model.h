#pragma once

#include <vector>
#include <glm/ext/matrix_float4x4.hpp>
#include "mesh.h"
#include "material.h"
#include"../shaders/shader.h"
#include <unordered_map>
#include <memory>
namespace core {
    class Model {
    private:
        std::vector<core::Mesh> meshes;
        glm::mat4 modelMatrix;
        std::shared_ptr<Shader> shader;
        core::Material material;
    public:
        Model(std::vector<core::Mesh> meshes) : meshes(meshes), modelMatrix(1) {}

        void render(GLenum drawMode);
        void translate(glm::vec3 translation);
        void position(glm::vec3 position);
        void rotate(glm::vec3 axis, float radians);
        void scale(glm::vec3 scale);
        glm::mat4 getModelMatrix() const;
        core::Material getMaterial() const;
        void setShader() const;
    };

   
}