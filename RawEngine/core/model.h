#pragma once

#include <vector>
#include <glm/ext/matrix_float4x4.hpp>
#include "mesh.h"

namespace core {
    class Model {
    private:
        std::vector<core::Mesh> meshes;
        glm::mat4 modelMatrix;
        // TODO: cache shader
        // TODO: store object properties like object color & texture
    public:
        Model(std::vector<core::Mesh> meshes) : meshes(meshes), modelMatrix(1) {}

        void render(GLenum drawMode);
        void translate(glm::vec3 translation);
        void position(glm::vec3 position);
        void rotate(glm::vec3 axis, float radians);
        void scale(glm::vec3 scale);
        glm::mat4 getModelMatrix() const;
    };

    // Add that here and load it here. then use
    //glUseProgram(modelShaderProgram);
    //glUniform3fv(lightPosUniform, 1, glm::value_ptr(lightPos));
    //glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(projection* view* suzanne.getModelMatrix()));
    //glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(suzanne.getModelMatrix()));
    //suzanne.render(drawMode);
}