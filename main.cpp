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
#include "MidiPlayer/tml.h"
#include "MidiPlayer/sfPlayer.hpp"

// Other Libraries
#include "Game.hpp"
#include <vector>
#include <queue>
#include <fstream>

Game KeySlayer = Game();
sfPlayer* soundfont;
static void audioCallback(void* data, Uint8 *stream, int len);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
bool prevPlayerInput[128] = {false};
double globalScore = 0; 

int main(void) {
    
    // --- Initialize soundfont --- //
    soundfont = new sfPlayer(audioCallback, "MusicLibrary/kawai.sf2");
    tsf_channel_set_presetnumber(soundfont->soundfont, 0, 0, false); //Piano
    tsf_channel_set_presetnumber(soundfont->soundfont, 1, 1, false); //Super saw
    tsf_channel_set_presetnumber(soundfont->soundfont, 2, 2, false); //Synth brass
    tsf_channel_set_presetnumber(soundfont->soundfont, 3, 3, false); //Brighton synth
    
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
        for (int i = 0; i < 128; i++){
            if (!prevPlayerInput[i] && KeySlayer.playerInput[i])
                KeySlayer.playerToBeTurnedOn.push(i);
            if (prevPlayerInput[i] && !KeySlayer.playerInput[i])
                KeySlayer.playerToBeTurnedOff.push(i);
            prevPlayerInput[i] = KeySlayer.playerInput[i];
        }
        
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
    KeySlayer.inactive = 0.0f;
    
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
	if (key == GLFW_KEY_6)
	{
		if (action == GLFW_PRESS) {
			if (KeySlayer.debugMode) KeySlayer.debugMode = false;
			else KeySlayer.debugMode = true;
		}
	}
    if(key == GLFW_KEY_2 && octave < 8){
        if (action == GLFW_PRESS)
            octave++;
    }
    if(key == GLFW_KEY_1 && octave > -1){
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

static void audioCallback(void* data, Uint8 *stream, int len)
{
    //auto time = std::chrono::steady_clock::now();
    //double current_time = (double)std::chrono::duration_cast<std::chrono::milliseconds>(time - start_time).count() / 1000;
    //current_time -= 3.8;
    
    /*reader->getUserInput();
    
    // Turn on notes according to player input
    for (int i = 0; i < reader->toBeTurnedOn.size(); i++) {
        int key = reader->toBeTurnedOn[i];
        if (midiFile[key]) {
            tsf_channel_note_on(soundfont->soundfont, 0, key, 0.7);
        } else {
            tsf_channel_note_on(soundfont->soundfont, 0, key, 0.05);
        }
    } reader->toBeTurnedOn.clear();
    
    // Turn off notes according to player input
    for (int i = 0; i < reader->toBeTurnedOff.size(); i++) {
        int key = reader->toBeTurnedOff[i];
        tsf_channel_note_off(soundfont->soundfont, 2, key);
    } reader->toBeTurnedOff.clear();
    
    if (metronome_changed) {
        if (metronome_midi[75]) {
            tsf_channel_note_on(soundfont->soundfont, 2, 70, 0.4);
        } else {
            tsf_channel_note_off(soundfont->soundfont, 2, 70);
        }
    }
    
    // Turn on left hand notes
    left_hand->updateQueues(last_time-0.05, current_time-0.05);
    if (current_time < 70000) { //current_time is initialized to 729000 something on start which causes all notes to play and it's nasty
        for (int i = 0; i < left_hand->toBeTurnedOn.size(); i++) {
            int key = left_hand->toBeTurnedOn[i];
            tsf_channel_note_on(soundfont->soundfont, 0, key, 0.4);
        }
        for (int i = 0; i < left_hand->toBeTurnedOff.size(); i++) {
            int key = left_hand->toBeTurnedOff[i];
            tsf_channel_note_off(soundfont->soundfont, 0, key);
        }
    }
     */
    //last_time = current_time;
    
    while (!KeySlayer.playerToBeTurnedOn.empty()){
        int key = KeySlayer.playerToBeTurnedOn.front();
        float velocity = 0.1;
        if (KeySlayer.matchingKeys[key]) velocity = 0.7;
        tsf_channel_note_on(soundfont->soundfont, KeySlayer.returnSoundfont(), key, velocity);
        //std::cout << KeySlayer.playerToBeTurnedOn.front() << " ";
        KeySlayer.playerToBeTurnedOn.pop();
    }
    
    while (!KeySlayer.playerToBeTurnedOff.empty()){
        tsf_channel_note_off(soundfont->soundfont, KeySlayer.returnSoundfont(), KeySlayer.playerToBeTurnedOff.front());
        //std::cout << KeySlayer.playerToBeTurnedOff.front() << " ";
        KeySlayer.playerToBeTurnedOff.pop();
    }

    //Number of samples to process
    int SampleBlock, SampleCount = (len / (2 * sizeof(float))); //2 output channels
    for (SampleBlock = TSF_RENDER_EFFECTSAMPLEBLOCK; SampleCount; SampleCount -= SampleBlock, stream += (SampleBlock * (2 * sizeof(float))))
    {
        if (SampleBlock > SampleCount) SampleBlock = SampleCount;
        // Render the block of audio samples in float format
        tsf_render_float(soundfont->soundfont, (float*)stream, SampleBlock, 0);
    }
}
