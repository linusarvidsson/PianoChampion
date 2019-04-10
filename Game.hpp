#ifndef Game_hpp
#define Game_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <iomanip>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Graphics/Font.hpp"
#include "Graphics/Song.hpp"
#include "Graphics/GraphicsTools.hpp"

#include "midifile/MidiTrack.hpp"
#include "GameSystems/ScoreHandler.hpp"

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
	SONG_END,
    SETTINGS,
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
    
    std::queue<int> playerToBeTurnedOn;
    std::queue<int> playerToBeTurnedOff;
    
private:
    // Display data
    int screenWidth, screenHeight;
    GLuint colorShader, textureShader;
    
    // Song data
    ScoreHandler score;
    std::vector<songItem> songs;
    Song* activeSong;
    MidiTrack* activeTrack;
	int activeBPM;
    
    // Logic arrays for the game mechanics
    bool currentNotes[128];
    bool matchingKeys[128];
    
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
	void renderSongEnd();
	void displaySongPercent();
    
};

#endif
