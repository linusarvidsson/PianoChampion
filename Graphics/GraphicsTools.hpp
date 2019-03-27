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

class GraphicsTools{
public:
    static GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
    static void getResolution(int* displayWidth, int* displayHeight);
private:
    GraphicsTools() { }
};

#endif
