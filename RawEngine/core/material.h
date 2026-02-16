#pragma once

#include <memory>
#include "texture.h"
#include "glm/vec3.hpp"
#include "../shaders/shader.h"
namespace core {
    class Material {
    public:
        Material() = default;
        Material(std::string pAlbedo, std::string pNormal, float pRoughness, float pMetallic);

        Material(glm::vec3 pBaseColor, float pRoughness, float pMetallic);

        glm::vec3 baseColor{ 1.0f };
        float roughness = 1.0f;
        float metallic = 0.0f;

        GLuint albedo = 0;
        GLuint normal = 0;

        void bind(const Shader& shader) const;

        void addNormalTexture(std::string pNormalPath);
        void addAlbedoTexture(std::string pAlbedoPath);
        void addAlbedoTexture(GLuint index);
	};
}