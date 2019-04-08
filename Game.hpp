#ifndef Game_hpp
#define Game_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Graphics/Font.hpp"
#include "Graphics/Song.hpp"
#include "Graphics/GraphicsTools.hpp"

#include "midifile/MidiTrack.hpp"

struct songItem{
    std::string name;
    std::string filepath;
    int track;
    int bpm;
};

enum GameState{
    MAIN_MENU,
    SONG_SELECT,
	SONG_SETTINGS,
    SONG_ACTIVE,
    SETTINGS
};

class Game{
    
public:
    Game();
    ~Game();
    
    void init(int displayWidth, int displayHeight);
    void render();
    
    bool playerInput[128];
    GLboolean Keys[1024];
    GameState State;
    
private:
    // Display data
    int screenWidth, screenHeight;
    GLuint colorShader, textureShader;
    
    // Song data
    std::vector<songItem> songs;
    Song* activeSong;
    MidiTrack* activeTrack;
	int activeBPM;
    
    
    // Menu data
    int activeElement;
    
    // Font data
    GLuint textShader;
    Font* standardFont;
    
    // Render functions
    void renderSongMenu();
    void renderSong();
    void renderMainMenu();
    void renderSettings();
	void renderSongSettings();
	void displaySongPercent();
    
};

#endif
