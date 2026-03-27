#pragma once

#include <memory>
#include <string>
#include <optional>
#include "texture.h"
#include "glm/vec3.hpp"
#include "../shaders/shader.h"

namespace core {

    class Material {
    public:
        virtual ~Material() = default;

        // Bind material data (textures, uniforms) to the given shader
        virtual void bind(const Shader& shader) const = 0;
    };
} // namespace core