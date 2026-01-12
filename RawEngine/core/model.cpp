#include "model.h"
#include <glm/gtc/matrix_transform.hpp>
//#include "Camera.h"

namespace core {
    void Model::render(GLenum drawMode) {
        for (int i = 0; i < meshes.size(); ++i) {
            meshes[i].render(drawMode);
        }
    }
    // use Identity matrix and multiply it by translated pos. so even in update it will stay at X position
    void Model::position(glm::vec3 position) {
        modelMatrix = glm::translate(glm::mat4(0.1f), position); // model matrix = 1 * position // very simple terms
    }

    void Model::translate(glm::vec3 translation) {
        modelMatrix = glm::translate(modelMatrix, translation); // modelmatrix = modelMatrix * position -> Grows number in update.
    }

    void Model::rotate(glm::vec3 axis, float radians) {
        modelMatrix = glm::rotate(modelMatrix, radians, axis);
    }

    void Model::scale(glm::vec3 scale) {
        modelMatrix = glm::scale(modelMatrix, scale);
    }

    glm::mat4 Model::getModelMatrix() const {
        return this->modelMatrix;
    }
}