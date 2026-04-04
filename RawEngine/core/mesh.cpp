#include "mesh.h"

namespace core {
    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices) 
        : vertices(vertices), indices(indices) {
        setupBuffers();
    }

    void Mesh::setupBuffers() {

        // Creates OpenGL buffers:
        glGenVertexArrays(1, &VAO);// Vertex Array Object
        glGenBuffers(1, &VBO);// Vertex Buffer Object
        glGenBuffers(1, &EBO);// Element Buffer Object

        glBindVertexArray(VAO); // ORIGINALLy it was here

        // Uploads data to GPU:
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // Vertex data (positions, normals, UVs)
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(sizeof(Vertex) * vertices.size()), 
            &vertices[0], 
            GL_STATIC_DRAW);


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);// Indices (triangle definitions)
        // Send the triangle point indices to the GPU:
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizei>(sizeof(unsigned int) * indices.size()), 
            &indices[0],
            GL_STATIC_DRAW);


        glEnableVertexAttribArray(0);

        //glBindVertexArray(VAO); // DOES IT WORK IF WE MOVE IT HERE? it renders nothing
       
        // Sets up vertex attribute pointers:
        // The information below is stored in the currently bound VAO!
        //               determine location=x in shader:   step size (8 floats) 
        //                    | vec dimension                  |        offset inside Vertex struct
        //                       |                                         |
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangents));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangents));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, uv));
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

     Mesh Mesh::generateQuad() {
        std::vector<Vertex> vertexVector;
        vertexVector.reserve(6);

        glm::vec3 n(0.0f, 0.0f, 1.0f);
        glm::vec3 t(1.0f, 0.0f, 0.0f);
        glm::vec3 b(0.0f, 1.0f, 0.0f);

        vertexVector.emplace_back(glm::vec3(-1.0f, -1.0f, 0.0f), n, glm::vec2(0.0f, 0.0f), t, b);
        vertexVector.emplace_back(glm::vec3( 1.0f, -1.0f, 0.0f), n, glm::vec2(1.0f, 0.0f), t, b);
        vertexVector.emplace_back(glm::vec3(-1.0f,  1.0f, 0.0f), n, glm::vec2(0.0f, 1.0f), t, b);

        vertexVector.emplace_back(glm::vec3( 1.0f, -1.0f, 0.0f), n, glm::vec2(1.0f, 0.0f), t, b);
        vertexVector.emplace_back(glm::vec3( 1.0f,  1.0f, 0.0f), n, glm::vec2(1.0f, 1.0f), t, b);
        vertexVector.emplace_back(glm::vec3(-1.0f,  1.0f, 0.0f), n, glm::vec2(0.0f, 1.0f), t, b);

        std::vector<GLuint> indices = { 0, 1, 2, 3, 4, 5 };

        return Mesh(std::move(vertexVector), std::move(indices));
    }

    void Mesh::render(GLenum& drawMode) const {
        glBindVertexArray(VAO);
        glDrawElements(drawMode, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    }
}