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
		glm::mat4 identityMatrix{ 1.0f };
        
    public:
        Model(std::vector<core::Mesh> meshes) : meshes(meshes), modelMatrix(1.0f) {};

        void render(GLenum drawMode) const;

		// Transformation functions
        void position(const glm::vec3& pPosition);
        void translate(const glm::vec3& translation);
        void rotate(const glm::vec3& axis, float radians);
        void scale(const glm::vec3& scale);

        glm::mat4 getModelMatrix() const;

		// Material accessors
        std::shared_ptr<Material> material;
        const std::shared_ptr<Material>& getMaterial() const;
        void setMaterial(std::shared_ptr<Material> mat);

        //void ImGuiRender() {
        //    if (material) {
        //        material->onGui();
        //    }
        //
		//}
    };

};