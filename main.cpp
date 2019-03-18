// Libraries for console messages
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

// Graphics libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
using namespace glm;

#include "Graphics/stb_image.h"
#include "Graphics/shader.hpp"
#include "Graphics/note.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "Graphics/stb_image.h"

// MidiFile
#include "midifile/MidiFile.h"
#include "midifile/MidiTrack.hpp"
using namespace smf;

//MidiPlayer
#include "MidiPlayer/minisdl_audio.h"
#define TSF_IMPLEMENTATION
#include "MidiPlayer/tsf.h"
#define TML_IMPLEMENTATION
#include "MidiPlayer/tml.h"

#include <vector>


/* ––– DECLARATIONS FOR MIDIPLAYER –– */
// Holds the global instance pointer
static tsf* g_TinySoundFont;
// Holds global MIDI playback state
static double g_Msec = -2700;               //current playback time
static tml_message* g_MidiMessage;  //next message to be played
static void AudioCallback(void* data, Uint8 *stream, int len);



int main(void) {

    /* –– PLAY MIDI */
    tml_message* TinyMidiLoader = NULL;
    // Define the desired audio output format we request
    SDL_AudioSpec OutputAudioSpec;
    OutputAudioSpec.freq = 44100;
    OutputAudioSpec.format = AUDIO_F32;
    OutputAudioSpec.channels = 2;
    OutputAudioSpec.samples = 4096;
    OutputAudioSpec.callback = AudioCallback;
    // Initialize the audio system
    SDL_AudioInit(TSF_NULL);
    // Load MIDI
    TinyMidiLoader = tml_load_filename("MusicLibrary/grieg_mountain_king.mid");
    //Set up the global MidiMessage pointer to the first MIDI message
    g_MidiMessage = TinyMidiLoader;
    // Load the SoundFont from a file
    g_TinySoundFont = tsf_load_filename("MusicLibrary/kawai.sf2");
    // Set the SoundFont rendering output mode
    tsf_set_output(g_TinySoundFont, TSF_STEREO_INTERLEAVED, OutputAudioSpec.freq, 0.0f);
    // Request the desired audio output format
    SDL_OpenAudio(&OutputAudioSpec, TSF_NULL);
    // Start the actual audio playback here
    // The audio thread will begin to call our AudioCallback function
    SDL_PauseAudio(0);
    //Wait until the entire MIDI file has been played back (until the end of the linked message list is reached)
    //while (g_MidiMessage != NULL) SDL_Delay(100);
    //tsf_close(g_TinySoundFont);
    //tml_free(TinyMidiLoader);
    /* –– */


    MidiTrack track = MidiTrack("MusicLibrary/grieg_mountain_king.mid", 1);
    
    std::vector<GLfloat> vertex_data(track.size()*12);
    std::vector<GLfloat> color_data(track.size()*12);
    std::vector<GLuint> index_data(track.size()*6);
    
    GLfloat vertex_array_data[track.size()*12];
    GLfloat color_array_data[track.size()*12];
    GLuint index_array_data[track.size()*6];
    
    

    for(int i = 0; i < track.size(); i++){
        note n_i = note(track.note(i)->keyNumber, (GLfloat)(track.note(i)->start) / track.tps(), (GLfloat)(track.note(i)->start + track.note(i)->duration) / track.tps());
        
        // Vertex 1
        vertex_data.push_back( n_i.left() );
        vertex_data.push_back( n_i.start() );
        vertex_data.push_back( 0.0f );
        // Vertex 2
        vertex_data.push_back( n_i.right() );
        vertex_data.push_back( n_i.start() );
        vertex_data.push_back( 0.0f );
        // Vertex 3
        vertex_data.push_back( n_i.left() );
        vertex_data.push_back( n_i.end() );
        vertex_data.push_back( 0.0f );
        // Vertex 4
        vertex_data.push_back( n_i.right() );
        vertex_data.push_back( n_i.end() );
        vertex_data.push_back( 0.0f );
        
        for (int vertex = 0; vertex < 4; vertex++){
            color_data.push_back( n_i.color()[0] );
            color_data.push_back( n_i.color()[1] );
            color_data.push_back( n_i.color()[2] );
        }
        
        index_data.push_back( i*4 );
        index_data.push_back( i*4 +1 );
        index_data.push_back( i*4 +2 );
        index_data.push_back( i*4 +2 );
        index_data.push_back( i*4 +1 );
        index_data.push_back( i*4 +3 );
        
        
        vertex_array_data[i*12] = vertex_array_data[i*12 +6] = n_i.left();
        vertex_array_data[i*12 +3] = vertex_array_data[i*12 +9] = n_i.right();
        vertex_array_data[i*12 +1] = vertex_array_data[i*12 +4] = n_i.start();
        vertex_array_data[i*12 +7] = vertex_array_data[i*12 +10] = n_i.end() - 0.02;
        vertex_array_data[i*12 +2] = vertex_array_data[i*12 +5] = vertex_array_data[i*12 +8] = vertex_array_data[i*12 +11] = 0.0f;

        for (int vertex = 0; vertex < 4; vertex++){
            color_array_data[i*12 + vertex*3] = n_i.color()[0];
            color_array_data[i*12 + vertex*3+1] = n_i.color()[1];
            color_array_data[i*12 + vertex*3+2] = n_i.color()[2];
        }

        index_array_data[i*6] = i*4;
        index_array_data[i*6 +1] = index_array_data[i*6 +4] = i*4 +1;
        index_array_data[i*6 +2] = index_array_data[i*6 +3] = i*4 +2;
        index_array_data[i*6 +5] = i*4 +3;
    }


    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

    // Create a windowed mode window and its OpenGL context
    GLFWwindow *window;
    window = glfwCreateWindow(1280, 800, "PianoMan", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to open GLFW window");
        glfwTerminate();
        return -1;
    }

    // Initialize GLEW (Make the window's context current)
    glfwMakeContextCurrent(window);
    if(glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // Set computer keyboard as input. Needed for ESC key, to close window.
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Dark green background
    glClearColor(0.0f, 0.4f, 0.0f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Generate and bind VAO
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL programs from the shaders
    GLuint colorShader = LoadShaders( "Graphics/Shaders/ColorVertexShader.vertexshader", "Graphics/Shaders/ColorFragmentShader.fragmentshader" );
    GLuint textureShader = LoadShaders( "Graphics/Shaders/TextureVertexShader.vertexshader", "Graphics/Shaders/TextureFragmentShader.fragmentshader" );


    GLfloat backgroundwidth = 8.0f;
    GLfloat backgroundheight = 4.5f;

    GLfloat background_vertex_array_data[] = {
        -backgroundwidth, -backgroundheight, -0.1f,
        backgroundwidth, -backgroundheight, -0.1f,
        -backgroundwidth, backgroundheight, -0.1f,
        backgroundwidth, backgroundheight, -0.1f
    };

    static const GLuint background_index_array_data[] = {
        0,1,2,2,1,3
    };

    static const GLfloat backgound_uv[] = {
        0.0f, 1.0f,   // top-left corner
        1.0f, 1.0f,  // top-right corner
        0.0f, 0.0f,  // lower-left corner
        1.0f, 0.0f,  // lower-right corner

    };

    // Create texture and set parameters for the UV-mapping
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Import texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load("Graphics/Images/syntesia_bakgrund-01.png", &width, &height, &nrChannels, 0);
    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // Handle for
    GLuint TextureID  = glGetUniformLocation(textureShader, "myTextureSampler");

    // Get a handle for our "MVP" uniform
    GLuint MVPcolor = glGetUniformLocation(colorShader, "MVP");
    GLuint MVPtexture = glGetUniformLocation(textureShader, "MVP");


    // Create buffer for UV
    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backgound_uv), backgound_uv, GL_STATIC_DRAW);

    // Create vertex buffer
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_array_data), vertex_array_data, GL_STATIC_DRAW);
    /*
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_data.size(), &vertex_data[0], GL_STATIC_DRAW);
    */
    
    // Create background vertex buffer
    GLuint background_vertexbuffer;
    glGenBuffers(1, &background_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, background_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(background_vertex_array_data), background_vertex_array_data, GL_STATIC_DRAW);

    // Vreate color buffer
    
    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_array_data), color_array_data, GL_STATIC_DRAW);
    /*
    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, color_data.size(), &color_data[0], GL_STATIC_DRAW);
    */
    
    // Generate buffers for the indices
    
    GLuint indexbuffer;
    glGenBuffers(1, &indexbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_array_data), index_array_data, GL_STATIC_DRAW);
    /*
    GLuint indexbuffer;
    glGenBuffers(1, &indexbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data.size() * sizeof(GLfloat), &index_data[0], GL_STATIC_DRAW);
    */
    
    //background_indexbuffer
    GLuint background_indexbuffer;
    glGenBuffers(1, &background_indexbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, background_indexbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(background_index_array_data), background_index_array_data, GL_STATIC_DRAW);


    //********** Render loop ********** //

    // Loop until the user closes the window
    do{
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Projection matrix : 45∞ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        mat4 Projection = perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
        // Or, for an ortho camera:
        //mat4 Projection = ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

        // Camera matrix
        mat4 View = lookAt(
                           vec3(0,0,10), // Camera position
                           vec3(0,0,0),  // The point the camera looks at
                           vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                           );

        // Model matrix : an identity matrix (model will be at the origin)
        mat4 Model = mat4(1.0f);
        // Our ModelViewProjection : multiplication of our 3 matrices
        mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around


        // Use texture shader for background
        glUseProgram(textureShader);

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MVPtexture, 1, GL_FALSE, &MVP[0][0]);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        // Set our "myTextureSampler" sampler to use Texture Unit 0
        glUniform1i(TextureID, 0);

        // Attribute buffer 1: vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, background_vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Attribute buffer 2: UVs
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, background_indexbuffer);

        // Draw the triangles
        glDrawElements(GL_TRIANGLES, sizeof(background_index_array_data), GL_UNSIGNED_INT, (void*)0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);


        // Use color shader
        glUseProgram(colorShader);

        mat4 noteTranslation = Projection * View * translate(mat4(1.0f), vec3(0.0f, -glfwGetTime(), 0.0f)) * Model;

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MVPcolor, 1, GL_FALSE, &noteTranslation[0][0]);


        // Attribute buffer 1: vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


        // Attribute buffer 2: colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

        // Draw the notes
        glDrawElements(GL_TRIANGLES, track.size()*6, GL_UNSIGNED_INT, (void*)0 );

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // Swap front and back buffers
        glfwSwapBuffers(window);
        // Poll for and process events
        glfwPollEvents();

        SDL_Delay(30); //Play sound
    }
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window));

    // Cleanup VBO
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(colorShader);

    glfwTerminate();
    return 0;
}

static void AudioCallback(void* data, Uint8 *stream, int len)
{
    //Number of samples to process
    int SampleBlock, SampleCount = (len / (2 * sizeof(float))); //2 output channels
    for (SampleBlock = TSF_RENDER_EFFECTSAMPLEBLOCK; SampleCount; SampleCount -= SampleBlock, stream += (SampleBlock * (2 * sizeof(float))))
    {
        //We progress the MIDI playback and then process TSF_RENDER_EFFECTSAMPLEBLOCK samples at once
        if (SampleBlock > SampleCount) SampleBlock = SampleCount;

        //Loop through all MIDI messages which need to be played up until the current playback time
        for (g_Msec += SampleBlock * (1000.0 / 44100.0); g_MidiMessage && g_Msec >= g_MidiMessage->time; g_MidiMessage = g_MidiMessage->next)
        {
            switch (g_MidiMessage->type)
            {
                case TML_PROGRAM_CHANGE: //channel program (preset) change (special handling for 10th MIDI channel with drums)
                    tsf_channel_set_presetnumber(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->program, (g_MidiMessage->channel == 9));
                    break;
                case TML_NOTE_ON: //play a note
                    tsf_channel_note_on(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->key, g_MidiMessage->velocity / 127.0f);
                    break;
                case TML_NOTE_OFF: //stop a note
                    tsf_channel_note_off(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->key);
                    break;
                case TML_PITCH_BEND: //pitch wheel modification
                    tsf_channel_set_pitchwheel(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->pitch_bend);
                    break;
                case TML_CONTROL_CHANGE: //MIDI controller messages
                    tsf_channel_midi_control(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->control, g_MidiMessage->control_value);
                    break;
            }
        }

        // Render the block of audio samples in float format
        tsf_render_float(g_TinySoundFont, (float*)stream, SampleBlock, 0);
    }
}
