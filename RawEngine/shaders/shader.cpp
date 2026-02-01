#include "shader.h"
#include <unordered_map>
#include <glm/gtc/type_ptr.hpp>
namespace core {
    Shader::Shader(GLuint vertexShader, GLuint fragmentShader) { // It does  i did it before and was fien,
      
        int success;
        char infoLog[512];
        ID = glCreateProgram();
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragmentShader);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            printf("Error! Making Shader Program: %s\n", infoLog);
		}
    }

    void Shader::use() const
    {
        glUseProgram(ID);// initialize shader that will be used.
    }
    void Shader::end() const {
        glBindVertexArray(0);
    }

    int Shader::getUniformLocation(const std::string& name) const
    {
        auto it = uniformCache.find(name);
        if (it != uniformCache.end())
            return it->second;

        int location = glGetUniformLocation(ID, name.c_str());
        uniformCache[name] = location;
        return location;
    }

    void Shader::setProperty(const std::string& name, bool value) const
    {
        glUniform1i(getUniformLocation(name), value);
    }
    void Shader::setProperty(const std::string& name, int value) const
    {
        glUniform1i(getUniformLocation(name), value);
    }
    void Shader::setProperty(const std::string& name, float value) const
    {
        glUniform1f(getUniformLocation(name), value);
    }
    void Shader::setProperty(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(
            getUniformLocation(name), 1, glm::value_ptr(value)
        );
    }
    void Shader::setProperty(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
    }
    void Shader::setProperty(const std::string& name, const glm::mat4& value) const
    {
        glUniformMatrix4fv(
            getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value)
        );
    }
}
