#include "lit_material.h"
#include "../core/texture.h"

#include <iostream>
#include <glad/glad.h>

namespace core {

    // ---------------- TEXTURE LOAD ----------------

    GLuint LitMaterial::loadTexture(const std::string& path)
    {
        if (path.empty())
            return 0;

        Texture tex(path);

        GLuint id = tex.getId();

        if (id == 0)
            std::cerr << "[PBRMaterial] Failed to load texture: " << path << "\n";

        return id;
    }

    // ---------------- CONSTRUCTORS ----------------

    LitMaterial::LitMaterial(std::shared_ptr<Shader> shader, glm::vec3 baseColor, float roughness, float metallic)
        : Material(shader),            // call base constructor with the shared_ptr
        m_baseColor(baseColor),
        m_roughness(roughness),
        m_metallic(metallic)
    {
    }

    LitMaterial::LitMaterial(std::shared_ptr<Shader> shader, const std::string& albedoPath,
        const std::string& normalPath,
        const std::string& roughnessPath,
		const std::string& metallicPath) 
        :Material(shader),
        m_albedo(loadTexture(albedoPath)),
        m_normal(loadTexture(normalPath)),
        m_roughnessMap(loadTexture(roughnessPath)),
        m_metallicMap(loadTexture(metallicPath))
    { }

    // ---------------- DESTRUCTOR ----------------

    LitMaterial::~LitMaterial()
    {
        if (m_albedo) glDeleteTextures(1, &m_albedo);
        if (m_normal) glDeleteTextures(1, &m_normal);
        if (m_roughnessMap) glDeleteTextures(1, &m_roughnessMap);
        if (m_metallicMap) glDeleteTextures(1, &m_metallicMap);
    }

    // ---------------- BIND ----------------
    void core::Material::bind()
    {
        shader->setProperty("baseColor", m_baseColor);
        shader->setProperty("roughness", m_roughness);
        shader->setProperty("metallic", m_metallic);

        // Albedo
        if (m_albedo)
        {
            shader->setProperty("albedoTex", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_albedo);
        }

        // Normal
        if (m_normal)
        {
            shader->setProperty("normalMap", 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_normal);
        }

        // Roughness
        if (m_roughnessMap)
        {
            shader->setProperty("roughMap", 2);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, m_roughnessMap);
        }

        // Metallic
        if (m_metallicMap)
        {
            shader->setProperty("metallicMap", 3);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, m_metallicMap);
        }

        glActiveTexture(GL_TEXTURE0);
    }

    // ---------------- SETTERS ----------------

    LitMaterial& LitMaterial::setBaseColor(glm::vec3 color)
    {
        m_baseColor = color;
        return *this;
    }

    LitMaterial& LitMaterial::setRoughness(float r)
    {
        m_roughness = r;
        return *this;
    }

    LitMaterial& LitMaterial::setMetallic(float m)
    {
        m_metallic = m;
        return *this;
    }

    LitMaterial& LitMaterial::setAlbedoTexture(const std::string& path)
    {
        if (m_albedo) glDeleteTextures(1, &m_albedo);
        m_albedo = loadTexture(path);
        return *this;
    }

    LitMaterial& LitMaterial::setNormalTexture(const std::string& path)
    {
        if (m_normal) glDeleteTextures(1, &m_normal);
        m_normal = loadTexture(path);
        return *this;
    }

    LitMaterial& LitMaterial::setRoughnessTexture(const std::string& path)
    {
        if (m_roughnessMap) glDeleteTextures(1, &m_roughnessMap);
        m_roughnessMap = loadTexture(path);
        return *this;
    }

    LitMaterial& LitMaterial::setMetallicTexture(const std::string& path)
    {
        if (m_metallicMap) glDeleteTextures(1, &m_metallicMap);
        m_metallicMap = loadTexture(path);
        return *this;
    }

    // ---------------- DEBUG ----------------

    void LitMaterial::debug(int meshIndex) const
    {
        std::cout << "\n===== PBR Material Debug [Mesh " << meshIndex << "] =====\n";

        std::cout << "BaseColor: "
            << m_baseColor.x << ", "
            << m_baseColor.y << ", "
            << m_baseColor.z << "\n";

        std::cout << "Roughness: " << m_roughness << "\n";
        std::cout << "Metallic: " << m_metallic << "\n";

        std::cout << "Albedo: " << (m_albedo ? "ON" : "OFF") << "\n";
        std::cout << "Normal: " << (m_normal ? "ON" : "OFF") << "\n";
        std::cout << "RoughnessMap: " << (m_roughnessMap ? "ON" : "OFF") << "\n";
        std::cout << "MetallicMap: " << (m_metallicMap ? "ON" : "OFF") << "\n";
    }

} // namespace core