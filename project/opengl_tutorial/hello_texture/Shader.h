#ifndef __SHADER_H__
#define __SHADER_H__
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
  public:
  // programe id
  unsigned int ID;

  // constructor
  Shader(const GLchar *vertexPath, const GLchar* fragmentPath);

    // use programe
    void use();

    // uniform tools funcs
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
};
#endif