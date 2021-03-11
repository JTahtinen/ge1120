#pragma once
#include <GL/glew.h>
#include <string>
#include <unordered_map>
#include "../math/mat3.h"

class Shader
{
    unsigned int _id;
    bool _valid;
    std::string _vertexSource;
    std::unordered_map<std::string, int> _uniformLocations;

    public:
    Shader();
    ~Shader();
    static Shader* load(const std::string& vertexSource, const std::string& fragmentSource);
    bool bind() const;
    void setUniform1i(const std::string& u_Name, int v0);
    void setUniform1f(const std::string& u_Name, float v0);
    void setUniform2f(const std::string& u_Name, float v0, float v1);
    void setUniform3f(const std::string& u_Name, float v0, float v1, float v2);
    void setUniform4f(const std::string& u_Name, float v0, float v1, float v2, float v3);
    void setUniformMat3(const std::string& u_Name, const Mat3& mat); 
    private:    
    Shader(unsigned int id);
    static unsigned int compileShader(GLenum type, const std::string& fileSource);
    int getUniformLocation(const std::string& u_Name);
};
