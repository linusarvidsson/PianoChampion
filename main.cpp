// Libraries for console messages
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Graphics libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Graphics/GraphicsTools.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "Graphics/stb_image.h"

// MidiPlayer libraries
#include "MidiPlayer/minisdl_audio.h"
#define TSF_IMPLEMENTATION
#include "MidiPlayer/tsf.h"
#define TML_IMPLEMENTATION
#include "MidiPlayer/tml.h"

// Other Libraries
#include "Game.hpp"
#include <vector>


Game KeySlayer = Game();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);


int main(void) {

    //----- Window Initialization -----//
    
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
	 
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

    // Create a windowed mode window and its OpenGL context
    int screenWidth, screenHeight;
    GraphicsTools::getResolution(&screenWidth, &screenHeight);
    
    GLFWwindow *window;
    //window = glfwCreateWindow(screenWidth, screenHeight, "Piano Champion", glfwGetPrimaryMonitor(), NULL);
    window = glfwCreateWindow(screenWidth, screenHeight, "Piano Champion", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to open GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Define the viewport dimensions
    glViewport(0, 0, screenWidth, screenHeight);
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }
    
    //Set computer keyboard as input. Needed for ESC key, to close window.
    //glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetKeyCallback(window, key_callback);
    
    // Enable culling
    glEnable(GL_CULL_FACE);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    // Blend function for text background transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    


    //----- SETUP -----//
    KeySlayer.init(screenWidth, screenHeight);
    

    do{
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Render
        KeySlayer.render();
        
        // Swap front and back buffers
        glfwSwapBuffers(window);
    }
    while (!glfwWindowShouldClose(window));


    glfwTerminate();
    return 0;
}


int octave = 4;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            KeySlayer.Keys[key] = GL_TRUE;
        
        else if (action == GLFW_RELEASE)
            KeySlayer.Keys[key] = GL_FALSE;
        
    }
    
    if(key == GLFW_KEY_UP && octave < 8){
        if (action == GLFW_PRESS)
            octave++;
    }
    if(key == GLFW_KEY_DOWN && octave > -1){
        if (action == GLFW_PRESS)
            octave--;
    }
    
    if(key >= 65 && key <= 90){
        if(key == GLFW_KEY_A) {
            if (action == GLFW_PRESS)
                KeySlayer.playerInput[12 + octave*12] = true;
            else if (action == GLFW_RELEASE)
                KeySlayer.playerInput[12 + octave*12] = false;
        }
        if(key == GLFW_KEY_W) {
            if (action == GLFW_PRESS)
                KeySlayer.playerInput[13 + octave*12] = true;
            else if (action == GLFW_RELEASE)
                KeySlayer.playerInput[13 + octave*12] = false;
        }
        if(key == GLFW_KEY_S) {
            if (action == GLFW_PRESS)
                KeySlayer.playerInput[14 + octave*12] = true;
            else if (action == GLFW_RELEASE)
                KeySlayer.playerInput[14 + octave*12] = false;
        }
        if(key == GLFW_KEY_E) {
            if (action == GLFW_PRESS)
                KeySlayer.playerInput[15 + octave*12] = true;
            else if (action == GLFW_RELEASE)
                KeySlayer.playerInput[15 + octave*12] = false;
        }
        if(key == GLFW_KEY_D) {
            if (action == GLFW_PRESS)
                KeySlayer.playerInput[16 + octave*12] = true;
            else if (action == GLFW_RELEASE)
                KeySlayer.playerInput[16 + octave*12] = false;
        }
        if(key == GLFW_KEY_F) {
            if (action == GLFW_PRESS)
                KeySlayer.playerInput[17 + octave*12] = true;
            else if (action == GLFW_RELEASE)
                KeySlayer.playerInput[17 + octave*12] = false;
        }
        if(key == GLFW_KEY_T) {
            if (action == GLFW_PRESS)
                KeySlayer.playerInput[18 + octave*12] = true;
            else if (action == GLFW_RELEASE)
                KeySlayer.playerInput[18 + octave*12] = false;
        }
        if(key == GLFW_KEY_G) {
            if (action == GLFW_PRESS)
                KeySlayer.playerInput[19 + octave*12] = true;
            else if (action == GLFW_RELEASE)
                KeySlayer.playerInput[19 + octave*12] = false;
        }
        if(key == GLFW_KEY_Y) {
            if (action == GLFW_PRESS)
                KeySlayer.playerInput[20 + octave*12] = true;
            else if (action == GLFW_RELEASE)
                KeySlayer.playerInput[20 + octave*12] = false;
        }
        if(key == GLFW_KEY_H) {
            if (action == GLFW_PRESS)
                KeySlayer.playerInput[21 + octave*12] = true;
            else if (action == GLFW_RELEASE)
                KeySlayer.playerInput[21 + octave*12] = false;
        }
        if(key == GLFW_KEY_U) {
            if (action == GLFW_PRESS)
                KeySlayer.playerInput[22 + octave*12] = true;
            else if (action == GLFW_RELEASE)
                KeySlayer.playerInput[22 + octave*12] = false;
        }
        if(key == GLFW_KEY_J) {
            if (action == GLFW_PRESS)
                KeySlayer.playerInput[23 + octave*12] = true;
            else if (action == GLFW_RELEASE)
                KeySlayer.playerInput[23 + octave*12] = false;
        }
        if(key == GLFW_KEY_K) {
            if (action == GLFW_PRESS)
                KeySlayer.playerInput[24 + octave*12] = true;
            else if (action == GLFW_RELEASE)
                KeySlayer.playerInput[24 + octave*12] = false;
        }
        if(key == GLFW_KEY_O) {
            if (action == GLFW_PRESS)
                KeySlayer.playerInput[25 + octave*12] = true;
            else if (action == GLFW_RELEASE)
                KeySlayer.playerInput[25 + octave*12] = false;
        }
        if(key == GLFW_KEY_L) {
            if (action == GLFW_PRESS)
                KeySlayer.playerInput[26 + octave*12] = true;
            else if (action == GLFW_RELEASE)
                KeySlayer.playerInput[26 + octave*12] = false;
        }
        if(key == GLFW_KEY_P) {
            if (action == GLFW_PRESS)
                KeySlayer.playerInput[27 + octave*12] = true;
            else if (action == GLFW_RELEASE)
                KeySlayer.playerInput[27 + octave*12] = false;
        }
    }
    
}
