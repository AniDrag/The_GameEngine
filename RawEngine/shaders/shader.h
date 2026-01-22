#pragma once
#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include <vector>

class Shader
{
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void use() const;

    void setProperty(const std::string& name, bool value) const;
    void setProperty(const std::string& name, int value) const;
    void setProperty(const std::string& name, float value) const;
    void setProperty(const std::string& name, const glm::vec2& value) const;
    void setProperty(const std::string& name, const glm::vec3& value) const;
    void setProperty(const std::string& name, const glm::mat4& value) const;

private:
    mutable std::unordered_map<std::string, int> uniformCache;
    int getUniformLocation(const std::string& name) const {
        if (uniformCache.contains(name))
            return uniformCache[name];

        int location = glGetUniformLocation(ID, name.c_str());
        uniformCache[name] = location;
        return location;
    };
};