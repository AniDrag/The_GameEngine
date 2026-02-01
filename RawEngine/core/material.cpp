#include"material.h"
#include"texture.h"
namespace core{

	
	Material::Material(std::string pAlbedo, std::string pNormal, float pRoughness, float pMetallic){
		Texture albedoTex(pAlbedo);
		Texture normalTex(pNormal);
		albedo = albedoTex.getId();
		normal = normalTex.getId();
		roughness = pRoughness;
		metallic = pMetallic;
		albedoColor = glm::vec3(1.0f);
	}
	Material::Material(glm::vec3 pAlbedoColor, float pRoughness, float pMetallic){
		albedoColor = pAlbedoColor;
		roughness = pRoughness;
		metallic = pMetallic;
		albedo = -1;
		normal = -1;
	}

	void Material::bind(const Shader& shader) const {
		shader.setProperty("baseColor", albedoColor);
		shader.setProperty("roughness", roughness);
		shader.setProperty("metallic", metallic);

		//open GL bind textures?
		if (albedo >= 0) {
			glActiveTexture(GL_TEXTURE0);
			shader.setProperty("albedoTex", 0);
			glBindTexture(GL_TEXTURE_2D, albedo);
		}
		if (normal >= 0) {
			glActiveTexture(GL_TEXTURE1);
			shader.setProperty("normalTex", 1);
			glBindTexture(GL_TEXTURE_2D, normal);
		}
		
	}

	void Material::addNormalTexture(std::string pNormalPath) {
		if (normal >= 0) {
			// remove current normal texture
			glDeleteTextures(1, &normal);
		}
		Texture normalTex(pNormalPath);
		normal = normalTex.getId();
	}
	void Material::addAlbedoTexture(std::string pAlbedoPath) {
		if (albedo >= 0) {
			// remove current albedo texture
			glDeleteTextures(1, &albedo);
		}
		Texture albedoTex(pAlbedoPath);
		albedo = albedoTex.getId();
	}

}