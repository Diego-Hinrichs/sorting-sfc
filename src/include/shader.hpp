#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

struct Shader {
public:
    Shader();

    void CreateFromString(const char* vertexCode, const char* fragmentCode);
    void CreateFromFiles(const char* vertexLocation, const char* fragmentLocation);

    std::string ReadFile(const char* fileLocation);

    GLuint GetProjectionLocation();
    GLuint GetModelLocation();
    GLuint GetViewLocation();
    GLuint GetSizeLocation();
    GLuint GetColorLocation();
    
    void UseShader();
    void ClearShader();

    ~Shader();
private:
    GLuint shaderID, uniformProjection, uniformModel, uniformView, uniformPointSize, color;
    void CompileShader(const char* vertexCode, const char* fragmentCode);
    void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);
};

#endif