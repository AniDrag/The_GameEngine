#pragma once
#pragma once
#include <glad/glad.h>
#include <unordered_map>
#include <string>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
namespace core {
    class Shader
    {
    public:
        unsigned int ID;

        Shader(GLuint vertexShader, GLuint fragmentShader);
        ~Shader() = default;

        void use() const;
        void end() const;

        void setProperty(const std::string& name, bool value) const;
        void setProperty(const std::string& name, int value) const;
        void setProperty(const std::string& name, float value) const;
        void setProperty(const std::string& name, const glm::vec2& value) const;
        void setProperty(const std::string& name, const glm::vec3& value) const;
        void setProperty(const std::string& name, const glm::mat4& value) const;

    private:
        mutable std::unordered_map<std::string, int> uniformCache;
        int getUniformLocation(const std::string& name) const;

    };
}