#pragma once


#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangents;
    glm::vec3 bitangents;
    glm::vec2 uv;

    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv, glm::vec3 tan, glm::vec3 btan) : position(position), normal(normal), uv(uv),tangents(tan), bitangents(btan) {}
};