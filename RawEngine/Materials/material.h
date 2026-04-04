#pragma once

#include <memory>
#include <string>
#include "glm/vec3.hpp"
#include "../shaders/shader.h"

namespace core {

    class Material {
    public:
        std::shared_ptr<Shader> shader;

        Material(std::shared_ptr<Shader> shader) : shader(shader) {}

        virtual ~Material() = default;

        virtual void bind();

        virtual void onGui() {}
    };

} 