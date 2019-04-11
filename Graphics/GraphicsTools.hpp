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

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

class GraphicsTools{
public:
    // Load shaders and create a shader program
    static GLuint loadShaders(const char * vertex_file_path,const char * fragment_file_path);
    
    // Load a texture. If the texture contains alpha, alpha = true.
    static GLuint loadTexture(const char* filepath, bool alpha);
    
    // Get the width and height of the primary display.
    static void getResolution(int* displayWidth, int* displayHeight);
    
private:
    GraphicsTools() { }
};

#endif
