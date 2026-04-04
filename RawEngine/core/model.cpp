#include "model.h"
#include <glm/gtc/matrix_transform.hpp>
#include "../Materials/material.h"
namespace core {

    //Model::Model(std::vector<Mesh> meshes)
    //    : meshes(std::move(meshes)), modelMatrix(1.0f) {
    //}
    void Model::render(GLenum drawMode) const {
		material->bind();
        for (const auto& mesh : meshes) {
            mesh.render(drawMode);
        }
    }

    void Model::position(const glm::vec3& pPosition) {
        modelMatrix = glm::translate(identityMatrix, pPosition);
    }

    void Model::translate(const glm::vec3& translation) {
        modelMatrix = glm::translate(modelMatrix, translation);
    }

    void Model::rotate(const glm::vec3& axis, float radians) {
        modelMatrix = glm::rotate(modelMatrix, radians, axis);
    }

    void Model::scale(const glm::vec3& s) {
        modelMatrix = glm::scale(modelMatrix, s);
    }

    glm::mat4 Model::getModelMatrix() const {
        return modelMatrix;
    }

    const std::shared_ptr<Material>& Model::getMaterial() const {
        return material;
    }

    void Model::setMaterial(std::shared_ptr<Material> mat) {
        material = std::move(mat);
    }
    
   
}