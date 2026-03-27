// pbr_material.h
#pragma once
#include "material.h"
#include <string>
#include <optional>

namespace core {

    class PBRMaterial : public Material {
    public:
        // Constructor from scalar values (no textures)
        PBRMaterial(glm::vec3 baseColor = glm::vec3(1.0f),
            float roughness = 1.0f,
            float metallic = 0.0f);

        // Constructor with texture paths
        PBRMaterial(const std::string& albedoPath,
            const std::string& normalPath,
            const std::string& roughnessPath = "",
            const std::string& metallicPath = "");

        void bind(const Shader& shader) const override;

        // Setters (chainable)
        PBRMaterial& setBaseColor(glm::vec3 color);
        PBRMaterial& setRoughness(float r);
        PBRMaterial& setMetallic(float m);
        PBRMaterial& setAlbedoTexture(const std::string& path);
        PBRMaterial& setNormalTexture(const std::string& path);
        PBRMaterial& setRoughnessTexture(const std::string& path);
        PBRMaterial& setMetallicTexture(const std::string& path);

    private:
        glm::vec3 m_baseColor{ 1.0f };
        float m_roughness = 1.0f;
        float m_metallic = 0.0f;

        GLuint m_albedo = 0;
        GLuint m_normal = 0;
        GLuint m_roughnessMap = 0;
        GLuint m_metallicMap = 0;

        // Helper to load texture and return GLuint (or 0 on failure)
        GLuint loadTexture(const std::string& path);
    };

} // namespace core