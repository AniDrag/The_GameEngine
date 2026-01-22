#include"material.h"

namespace core{

	Material::Material(std::string albedoTexPath, std::string normalTexPath, float roughness, float metallic){
		this->albedo(&albedoTexPath);
		this->normal(&normalTexPath);
		this->roughness = roughness;
		this->metallic = metallic;
	}
	Material::Material(glm::vec3 albedoColor, float roughness, float metallic){
		this->albedoColor = albedoColor;
		this->roughness = roughness;
		this->metallic = metallic;
	}

	void Material::bind(const Shader& shader) const {
			shader.setProperty("baseColor", albedoColor);
			shader.setProperty("roughness", roughness);
			shader.setProperty("metallic", metallic);

			albedo.bind(0);
			shader.setProperty("albedoTex", 0);

			normal.bind(1);
			shader.setProperty("normalTex", 1);
		
	}

}