#pragma once

#include "material.h"
#include <string>
#include <glad/glad.h>

namespace core {

    class LitMaterial : public Material {
    public:
        LitMaterial(std::shared_ptr<Shader> shader, glm::vec3 baseColor = glm::vec3(1.0f),
            float roughness = 1.0f,
            float metallic = 0.0f);

        LitMaterial(std::shared_ptr<Shader> shader, const std::string& albedoPath,
            const std::string& normalPath,
            const std::string& roughnessPath = "",
            const std::string& metallicPath = "");
        

        ~LitMaterial();

        void bind() override;

        LitMaterial& setBaseColor(glm::vec3 color);
        LitMaterial& setRoughness(float r);
        LitMaterial& setMetallic(float m);

        LitMaterial& setAlbedoTexture(const std::string& path);
        LitMaterial& setNormalTexture(const std::string& path);
        LitMaterial& setRoughnessTexture(const std::string& path);
        LitMaterial& setMetallicTexture(const std::string& path);

        void debug(int meshIndex) const;

    private:
        glm::vec3 m_baseColor{ 1.0f };
        float m_roughness = 1.0f;
        float m_metallic = 0.0f;

        GLuint m_albedo = 0;
        GLuint m_normal = 0;
        GLuint m_roughnessMap = 0;
        GLuint m_metallicMap = 0;

        GLuint loadTexture(const std::string& path);
    };

} // namespace core