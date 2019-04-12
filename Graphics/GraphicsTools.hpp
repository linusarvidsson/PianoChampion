#ifndef GraphicsTools_hpp
#define GraphicsTools_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

#include <stdlib.h>
#include <string.h>
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "texture.h"

#include "stb_image.h"

class GraphicsTools{
public:	
	// Resource storage
	static std::map<std::string, Shader>    Shaders;
	static std::map<std::string, Texture2D> Textures;
	// Loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
	static Shader   LoadShaderBreakout(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name);
	// Retrieves a stored sader
	static Shader   GetShader(std::string name);
	// Retrieves a stored texture
	static Texture2D GetTexture(std::string name);


    static GLuint loadShaders(const char * vertex_file_path,const char * fragment_file_path);
    static GLuint loadTexture(const char* filepath);
    static void getResolution(int* displayWidth, int* displayHeight);
	static Shader loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const GLchar *gShaderFile);
private:
    GraphicsTools() { }
};

#endif
