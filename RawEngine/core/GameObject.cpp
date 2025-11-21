#include "GameObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include "core/mesh.h"

namespace core {
    void GameObject::render(GLuint shaderProgram, const GLenum drawmode, const glm::mat4& view, const glm::mat4& proj) const {
        if (!model) return;
        glm::mat4 testModelMat = model->getModelMatrix();
        GLint textureModelUniform = glGetUniformLocation(shaderProgram, "mvpMatrix");

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(textureModelUniform, 1, GL_FALSE, glm::value_ptr(proj * view * testModelMat));

        // bind texture if present and shader has sampler 'text'
        if (textureID != 0) {
            GLint texLoc = glGetUniformLocation(shaderProgram, "mvpMatrix");
            if (texLoc >= 0) {
                glActiveTexture(GL_TEXTURE0);
                glUniform1i(texLoc, 0); // texture unit 0
                glBindTexture(GL_TEXTURE_2D, textureID);
            }
        }

        this->model->render(drawmode);
        if (textureID != 0) {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}