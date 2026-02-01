#include "model.h"
#include <glm/gtc/matrix_transform.hpp>
#include "material.h"
namespace core {
    void Model::render(GLenum drawMode) const{

        //if (!shader || !material ) return;

        // 1️ Use shader
		//shader->use(); //called by scene to input camera + lights once per model if they use different shaders.

        // 2️ Per-model uniform
        shader->setProperty("modelMatrix", modelMatrix); 

        // 3️ Bind material (textures + material uniforms)
        material.bind(*shader);

        // 4️ Draw meshes
        for (const auto& mesh : meshes)
        {
            mesh.render(drawMode);
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
    void Model::attachShader(std::shared_ptr<Shader>& pShader) {
        shader = pShader; // store shader for THIS model
    }
    
   
}