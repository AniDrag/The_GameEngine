#include "material.h"
#include "texture.h"
#include <glad/glad.h>

namespace core {

    // Helper to load a texture, returns 0 on failure
    GLuint Material::loadTexture(const std::string& path) {
        if (path.empty()) return 0;
        Texture tex(path);
        return tex.getId();
    }

    // Constructor from parameter struct
    Material::Material(const MaterialParams& params)
        : m_baseColor(params.baseColor)
        , m_roughness(params.roughness)
        , m_metallic(params.metallic)
    {
        if (params.albedoPath.has_value())
            m_albedo = loadTexture(params.albedoPath.value());
        if (params.normalPath.has_value())
            m_normal = loadTexture(params.normalPath.value());
        if (params.roughnessPath.has_value())
            m_roughnessMap = loadTexture(params.roughnessPath.value());
        if (params.metallicPath.has_value())
            m_metallicMap = loadTexture(params.metallicPath.value());
        // ao path can be handled similarly if needed
    }

    // Constructor from simple colors
    Material::Material(glm::vec3 pBaseColor, float pRoughness, float pMetallic)
        : m_baseColor(pBaseColor)
        , m_roughness(pRoughness)
        , m_metallic(pMetallic) { }

    // Setters – chainable
    Material& Material::setBaseColor(glm::vec3 color) {
        m_baseColor = color;
        return *this;
    }

    Material& Material::setRoughness(float r) {
        m_roughness = r;
        return *this;
    }

    Material& Material::setMetallic(float m) {
        m_metallic = m;
        return *this;
    }

    Material& Material::setAlbedoTexture(const std::string& path) {
        if (m_albedo) glDeleteTextures(1, &m_albedo);
        m_albedo = loadTexture(path);
        return *this;
    }

    Material& Material::setNormalTexture(const std::string& path) {
        if (m_normal) glDeleteTextures(1, &m_normal);
        m_normal = loadTexture(path);
        return *this;
    }

    Material& Material::setRoughnessTexture(const std::string& path) {
        if (m_roughnessMap) glDeleteTextures(1, &m_roughnessMap);
        m_roughnessMap = loadTexture(path);
        return *this;
    }

    Material& Material::setMetallicTexture(const std::string& path) {
        if (m_metallicMap) glDeleteTextures(1, &m_metallicMap);
        m_metallicMap = loadTexture(path);
        return *this;
    }

    void Material::bind(const Shader& shader) const {
        // Scalar uniforms
        shader.setProperty("baseColor", m_baseColor);
        shader.setProperty("roughness", m_roughness);
        shader.setProperty("metallic", m_metallic);

        // Albedo texture (unit 0)
        if (m_albedo) {
            glActiveTexture(GL_TEXTURE0);
            shader.setProperty("albedoTex", 0);
            glBindTexture(GL_TEXTURE_2D, m_albedo);
        }
        else {
            // Optional: unbind or use a default white texture
        }

        // Normal map (unit 1)
        if (m_normal) {
            glActiveTexture(GL_TEXTURE1);
            shader.setProperty("normalMap", 1);
            glBindTexture(GL_TEXTURE_2D, m_normal);
        }
        else {
            // If no normal map, you might want to bind a default normal (0.5,0.5,1)
        }

        // Roughness map (unit 2) – if present
        if (m_roughnessMap) {
            glActiveTexture(GL_TEXTURE2);
            shader.setProperty("roughMap", 2);
            shader.setProperty("useRoughMap", 1);   // enable texture sampling
            glBindTexture(GL_TEXTURE_2D, m_roughnessMap);
        }
        else {
            shader.setProperty("useRoughMap", 0);   // use scalar roughness
        }

        // Metallic map (unit 3) – if present (add to shader if needed)
        if (m_metallicMap) {
            glActiveTexture(GL_TEXTURE3);
            shader.setProperty("metallicMap", 3);
            shader.setProperty("useMetallicMap", 1);
            glBindTexture(GL_TEXTURE_2D, m_metallicMap);
        }
        else {
            shader.setProperty("useMetallicMap", 0);
        }

        // Remember to restore active texture if needed
        glActiveTexture(GL_TEXTURE0);
    }

} // namespace core