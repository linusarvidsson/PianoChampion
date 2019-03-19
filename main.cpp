// Libraries for console messages
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
//using namespace std;

// Graphics libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Graphics/shader.hpp"
#include "Graphics/note.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "Graphics/stb_image.h"

// MidiFile libraries
#include "midifile/MidiFile.h"
#include "midifile/MidiTrack.hpp"

// MidiPlayer libraries
#include "MidiPlayer/minisdl_audio.h"
#define TSF_IMPLEMENTATION
#include "MidiPlayer/tsf.h"
#define TML_IMPLEMENTATION
#include "MidiPlayer/tml.h"

// Other Libraries
#include <vector>


//----- DECLARATIONS FOR MIDIPLAYER -----//

// Holds the global instance pointer
static tsf* g_TinySoundFont;
// Holds global MIDI playback state
static double g_Msec = -3200;       //current playback time
static tml_message* g_MidiMessage;  //next message to be played
static void AudioCallback(void* data, Uint8 *stream, int len);
void get_resolution(int* w, int* h) {
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	*w = mode->width;
	*h = mode->height;
}


int main(void) {

    //----- Note Data -----//

    // Read track from a MIDI-file to get note data
    MidiTrack track = MidiTrack("MusicLibrary/pianoman.mid", 1, 100);

    std::vector<glm::vec3> noteVertices;
    noteVertices.reserve(track.size()*4);
    std::vector<glm::vec3> noteColors;
    noteColors.reserve(track.size()*4);
    std::vector<GLuint> noteIndices;
    noteIndices.reserve(track.size()*6);

    for(int i = 0; i < track.size(); i++){
        note n_i = note(track.note(i)->keyNumber, (GLfloat)(track.note(i)->start) / track.tps(), (GLfloat)(track.note(i)->start + track.note(i)->duration) / track.tps());

        // Vertex 1
        noteVertices.push_back( glm::vec3(n_i.left(), n_i.start(), 0.0f) );
        // Vertex 2
        noteVertices.push_back( glm::vec3(n_i.right(), n_i.start(), 0.0f) );
        // Vertex 3
        noteVertices.push_back( glm::vec3(n_i.left(), n_i.end(), 0.0f) );
        // Vertex 4
        noteVertices.push_back( glm::vec3(n_i.right(), n_i.end(), 0.0f) );

        // Color
        for (int vertex = 0; vertex < 4; vertex++){
            noteColors.push_back( glm::vec3(n_i.color()[0], n_i.color()[1], n_i.color()[2]) );
        }

        // Indexing
        noteIndices.push_back( i*4 );
        noteIndices.push_back( i*4 +1 );
        noteIndices.push_back( i*4 +2 );
        noteIndices.push_back( i*4 +2 );
        noteIndices.push_back( i*4 +1 );
        noteIndices.push_back( i*4 +3 );
    }





    //----- Initialization OpenGL -----//

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
	int w, h;
	get_resolution(&w,&h);

    GLFWwindow *window;
    window = glfwCreateWindow(w, h, "Piano Champion", glfwGetPrimaryMonitor(), NULL);
    if (!window) {
        fprintf(stderr, "Failed to open GLFW window");
        glfwTerminate();
        return -1;
    }

    // Initialize GLEW
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





    //----- Background Data -----//

    GLfloat backgroundwidth = 5.45f;
    GLfloat backgroundheight = 5.0f;

    static const GLfloat backgroundVertices[] = {
        -backgroundwidth, -backgroundheight, -0.01f,
        backgroundwidth, -backgroundheight, -0.01f,
        -backgroundwidth, backgroundheight, -0.01f,
        backgroundwidth, backgroundheight, -0.01f
    };

    static const GLuint backgroundIndices[] = {
        0,1,2,2,1,3
    };

    static const GLfloat backgroundUV[] = {
        0.0f, 1.0f,   // top-left corner
        1.0f, 1.0f,  // top-right corner
        0.0f, 0.0f,  // lower-left corner
        1.0f, 0.0f,  // lower-right corner

    };

    // Create texture and set parameters for the UV-mapping
    GLuint backgroundTexture;
    glGenTextures(1, &backgroundTexture);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Import texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load("Graphics/Images/pianoklaviatur.png", &width, &height, &nrChannels, 0);
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

    // Get a handle for the "MVP" uniform
    GLuint MVPcolor = glGetUniformLocation(colorShader, "MVP");
    GLuint MVPtexture = glGetUniformLocation(textureShader, "MVP");





    //----- Background Buffers -----//

    // Create vertex buffer
    GLuint backgroundVertexBuffer;
    glGenBuffers(1, &backgroundVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, backgroundVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);

    // Create UV buffer
    GLuint backgroundUVBuffer;
    glGenBuffers(1, &backgroundUVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, backgroundUVBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundUV), backgroundUV, GL_STATIC_DRAW);

    // Create index buffer
    GLuint backgroundElementBuffer;
    glGenBuffers(1, &backgroundElementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backgroundElementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(backgroundIndices), backgroundIndices, GL_STATIC_DRAW);





    //----- Note Buffers -----//

    // Create vertex buffer
    GLuint noteVertexBuffer;
    glGenBuffers(1, &noteVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, noteVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, noteVertices.size() * sizeof(glm::vec3), &noteVertices.front(), GL_STATIC_DRAW);

    // Create color buffer
    GLuint noteColorBuffer;
    glGenBuffers(1, &noteColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, noteColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, noteColors.size() * sizeof(glm::vec3), &noteColors.front(), GL_STATIC_DRAW);


    // Generate index buffer
    GLuint noteElementBuffer;
    glGenBuffers(1, &noteElementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, noteElementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, noteIndices.size() * sizeof(GLuint), &noteIndices.front(), GL_STATIC_DRAW);



    //----- Camera Setup -----//

    // Projection matrix : 45∞ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    // Or, for an ortho camera:
    //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

    // Camera matrix
    glm::mat4 View = glm::lookAt(
                                 glm::vec3(0,0,10), // Camera position
                                 glm::vec3(0,0,0),  // The point the camera looks at
                                 glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                                 );

    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model = glm::mat4(1.0f);
    // ModelViewProjection
    glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

	
	//----- PLAY MIDI -----//

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
	TinyMidiLoader = tml_load_filename("MusicLibrary/pianoman.mid");
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



    //---------- Render loop ----------//

    do{
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //----- Render Textured Objects -----//

        // Use texture shader for background
        glUseProgram(textureShader);

        // Send MVP transformation to the currently bound shader
        glUniformMatrix4fv(MVPtexture, 1, GL_FALSE, &MVP[0][0]);

        // Bind texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);
        // Set "myTextureSampler" sampler to use Texture Unit 0
        glUniform1i(TextureID, 0);

        // Attribute buffer 1: vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, backgroundVertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        // Attribute buffer 2: UVs
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, backgroundUVBuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backgroundElementBuffer);

        // Draw the textured elements
        glDrawElements(GL_TRIANGLES, sizeof(backgroundIndices), GL_UNSIGNED_INT, NULL);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);


        //----- Render Colored Objects -----//

        // Use color shader
        glUseProgram(colorShader);

        // MVP for notes. Translates notes with time.
        glm::mat4 noteTranslation = Projection * View * translate(glm::mat4(1.0f), glm::vec3(0.0f, -glfwGetTime(), 0.0f)) * Model;

        // Send the transformation to the currently bound shader,
        glUniformMatrix4fv(MVPcolor, 1, GL_FALSE, &noteTranslation[0][0]);

        // Attribute buffer 1: vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, noteVertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        // Attribute buffer 2: colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, noteColorBuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, noteElementBuffer);

        // Draw the colored elements
        glDrawElements(GL_TRIANGLES, (GLsizei)noteIndices.size(), GL_UNSIGNED_INT, NULL);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);


        //----- Render -----//

        // Swap front and back buffers
        glfwSwapBuffers(window);
        // Poll for and process events
        glfwPollEvents();

        // Play sound
        SDL_Delay(30);

    }
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window));
    // Loop until user closes the window


    // Cleanup VBO
    glDeleteBuffers(1, &noteVertexBuffer);
    glDeleteBuffers(1, &noteColorBuffer);
    glDeleteBuffers(1, &noteElementBuffer);
    glDeleteBuffers(1, &backgroundVertexBuffer);
    glDeleteBuffers(1, &backgroundUVBuffer);
    glDeleteBuffers(1, &backgroundElementBuffer);

    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(colorShader);
    glDeleteProgram(textureShader);

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
