#include "shader.h"
#include <GL/glew.h>
#include "../defs.h"
#include "../util/file.h"
#include <iostream>
#include "../globals.h"

#define U_LOCATION getUniformLocation(u_Name)

Shader::Shader(unsigned int id)
    : _id(id), _valid(true)
{
}

Shader::Shader()
    : _id(-1), _valid(false)
{
}

Shader::~Shader()
{
    if (_valid)
    {
        GLCALL(glDeleteProgram(_id));
    }
}

unsigned int Shader::compileShader(GLenum type, const std::string &fileSource)
{
    GLCALL(GLuint shader = glCreateShader(type));
    std::string fileString = loadTextFile(fileSource);
    const char *source = fileString.c_str();
    GLCALL(glShaderSource(shader, 1, &source, nullptr));
    GLCALL(glCompileShader(shader));
    int result;
    GLCALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE)
    {
        if (type == GL_VERTEX_SHADER)
            std::cout << "[ERROR] Could not compile vertex shader: " << fileSource << std::endl;
        else
            std::cout << "[ERROR] Could not compile fragment shader" << fileSource <<std::endl;
    }
    return shader;
}

Shader* Shader::load(const std::string &vertexSource, const std::string &fragmentSource)
{
    GLCALL(GLuint id = glCreateProgram());
    unsigned int vs = Shader::compileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fs = Shader::compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLCALL(glAttachShader(id, vs));
    GLCALL(glAttachShader(id, fs));
    GLCALL(glLinkProgram(id));
    GLCALL(glValidateProgram(id));
    GLCALL(glDeleteShader(vs));
    GLCALL(glDeleteShader(fs));
    Shader* shader = new Shader(id);
    shader->_vertexSource = vertexSource;
    return shader;
}

bool Shader::bind() const
{
    if (g_boundShaderID == _id)
    {
        return true;
    }
    else if (_valid)
    {
	    GLCALL(glUseProgram(_id));
        g_boundShaderID = _id;
        return true;
    }
    else
    {
        std::cout << "[ERROR] Invalid shader!" << std::endl;
        return false;
    }
}

void Shader::setUniform1i(const std::string &u_Name, int v0)
{
    bind();
    GLCALL(glUniform1i(U_LOCATION, v0));
}

void Shader::setUniform1f(const std::string &u_Name, float v0)
{
    bind();
    GLCALL(glUniform1f(U_LOCATION, v0));
}

void Shader::setUniform2f(const std::string &u_Name, float v0, float v1)
{
    bind();
    GLCALL(glUniform2f(U_LOCATION, v0, v1));
}

void Shader::setUniform3f(const std::string &u_Name, float v0, float v1, float v2)
{
    bind();
    GLCALL(glUniform3f(U_LOCATION, v0, v1, v2));
}

void Shader::setUniform4f(const std::string &u_Name, float v0, float v1, float v2, float v3)
{
    bind();
    GLCALL(glUniform4f(U_LOCATION, v0, v1, v2, v3));
}

void Shader::setUniformMat3(const std::string& u_Name, const Mat3& mat)
{
    bind();
    GLCALL(glUniformMatrix3fv(U_LOCATION, 1, GL_TRUE, &mat.units[0]));
}

int Shader::getUniformLocation(const std::string &u_Name)
{
    if (_uniformLocations.find(u_Name) != _uniformLocations.end())
    {
        return _uniformLocations[u_Name];
    }

    GLCALL(int location = glGetUniformLocation(_id, u_Name.c_str()));
    if (location == -1)
    {
        std::cout << "[ERROR] Could not find uniform " << u_Name << std::endl;
        return -1;
    }
    _uniformLocations[u_Name] = location;
    return location;
}