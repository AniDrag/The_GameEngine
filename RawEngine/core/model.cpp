#include "model.h"
#include <glm/gtc/matrix_transform.hpp>
#include "material.h"
namespace core {
    void Model::render(GLenum drawMode) const{

       //if (!shader) return;
       //shader->use();
       //shader->setProperty("modelMatrix", modelMatrix);
       //material.bind(*shader);

        for (auto& mesh : meshes)
            mesh.render(drawMode);
        for (int i = 0; i < meshes.size(); ++i) {
            meshes[i].render(drawMode); // meshes is a type modifier not compatible with the member function core::Mesh::Render
        }
    }
    // use Identity matrix and multiply it by translated pos. so even in update it will stay at X position
    void Model::position(glm::vec3& position) {
        modelMatrix = glm::translate(glm::mat4(1), position); // model matrix = 1 * position // very simple terms
    }

    void Model::translate(glm::vec3& translation) {
        modelMatrix = glm::translate(modelMatrix, translation); // modelmatrix = modelMatrix * position -> Grows number in update.
    }

    void Model::rotate(glm::vec3& axis, float radians) {
        modelMatrix = glm::rotate(modelMatrix, radians, axis);
    }

    void Model::scale(glm::vec3& scale) {
        modelMatrix = glm::scale(modelMatrix, scale);
    }

    glm::mat4 Model::getModelMatrix() const {
        return this->modelMatrix;
    }
	void Model::attachShader(const std::shared_ptr<Shader>& shader) const {
        this->shader;
    }
   
}