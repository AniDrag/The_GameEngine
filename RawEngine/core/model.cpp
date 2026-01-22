#include "model.h"
#include <glm/gtc/matrix_transform.hpp>
//#include "Camera.h"

namespace core {
    void Model::render(GLenum drawMode) {
        shader->use();
        material.bind(*shader);
        shader->setProperty("u_Model", modelMatrix);

        for (auto& mesh : meshes)
            mesh.render(drawMode);
        for (int i = 0; i < meshes.size(); ++i) {
            meshes[i].render(drawMode);
        }
        //idk if this is here
        //glUseProgram(shader);
        //glUniform3fv(lightPosUniform, 1, glm::value_ptr(lightPos));
        //glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(projection * view * suzanne.getModelMatrix()));
        //glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(suzanne.getModelMatrix()));
    }
    // use Identity matrix and multiply it by translated pos. so even in update it will stay at X position
    void Model::position(glm::vec3 position) {
        modelMatrix = glm::translate(glm::mat4(1), position); // model matrix = 1 * position // very simple terms
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