#include "Shader.hpp"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

void checkCompileErrors(uint32_t shader, const std::string& type) {
    int32_t success;
    char infoLog[512];
    if (type != "program") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (not success) {
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "[Error] Failed to compile " << type << " shader: " << infoLog << '\n';
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (not success) {
            glGetProgramInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "[Error] Failed to link shader program: " << infoLog << '\n';
        }
    }
}

Shader::Shader() : m_id(0) {}

Shader::Shader(const char* vertexPath, const char* fragmentPath) : m_id(0) {
    load(vertexPath, fragmentPath);
}

Shader::~Shader() {
    glDeleteProgram(m_id);
}

void Shader::load(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& error) {
        std::cerr << "[Error] Failed to load shader file: " << error.what() << '\n';
    }
    const char* vertexShaderCode = vertexCode.c_str();
    const char* fragmentShaderCode = fragmentCode.c_str();

    uint32_t vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShaderCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "vertex");
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShaderCode, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "fragment");
    m_id = glCreateProgram();
    glAttachShader(m_id, vertex);
    glAttachShader(m_id, fragment);
    glLinkProgram(m_id);
    checkCompileErrors(m_id, "program");
    glDetachShader(m_id, vertex);
    glDetachShader(m_id, fragment);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() const {
    glUseProgram(m_id);
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(const std::string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(m_id, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(m_id, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(m_id, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& value) const {
    glUniformMatrix2fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& value) const {
    glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &value[0][0]);
}
