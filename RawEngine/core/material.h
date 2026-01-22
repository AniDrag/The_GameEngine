#pragma once

#include <memory>
#include "texture.h"
#include "glm/vec3.hpp"
#include "../shaders/shader.h"
namespace core {
	class Material {
    public:
		Material() = default;
        Material(Texture albedo, Texture normal, float roughness, float metallic);
		Material(glm::vec3 albedoColor, float roughness, float metallic);

        glm::vec3 albedoColor{ 1.0f };
        float roughness = 1.0f;
        float metallic = 0.0f;

        std::shared_ptr<Texture> albedo;
        std::shared_ptr<Texture> normal;

        void bind(const Shader& shader) const;
    };
}