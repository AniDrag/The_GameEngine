#pragma once

#include "texture.h"
namespace core {
	class Material {
	private:
		//albedo/base color, normal, roughness, metallic
		Texture albedoTex;
		Texture normalTex;
		Texture roughnessTex;
		Texture metallicTex;

	public :
		void bind(const Shader& shader) const;
	};
}