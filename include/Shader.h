#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragmentPath);
    void use();
    void setInt(const std::string &name,const int value) const;
    void setMat4(const std::string &name,const float* value) const;
private:
    void checkCompileErrors(unsigned int shader, std::string type);
};
