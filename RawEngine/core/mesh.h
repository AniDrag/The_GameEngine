#pragma once

#include <vector>
#include <glad/glad.h>
#include "vertex.h"

namespace core {
    class Mesh {
    private:
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        GLuint VAO; // 
        GLuint VBO; // Vertex bufer object Stores model data
        GLuint EBO; // stores indicies for redrawing array
    public:
        Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);

        void render(GLenum& drawMode) const;
        static Mesh generateQuad();
    private:
        void setupBuffers();
    };
}