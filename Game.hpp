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
#include "Graphics/TextureQuad.hpp"

#include "midifile/MidiTrack.hpp"
#include "GameSystems/ScoreHandler.hpp"
#include "Midiplayer/cmidiin.h"

struct songItem{
    std::string name;
    std::string filepath;
    int track;
    int bpm;
    double duration;
    std::string difficulty;
};

enum GameState{
    MAIN_MENU,
    SONG_SELECT,
	SONG_SETTINGS,
    SONG_ACTIVE,
    POST_GAME
    
};

class Game{
    
public:
    Game();
    ~Game();
    
    void init(int displayWidth, int displayHeight);
    void render();
    int returnSoundfont();
    
    bool playerInput[128];
    bool matchingKeys[128];
    GLboolean Keys[1024];
    GameState State;
    double globalScore= 2;
    
    ScoreHandler score;
    int soundfont = 0;
    
    // Queues for MidiPlayer
    std::queue<int> playerToBeTurnedOn;
    std::queue<int> playerToBeTurnedOff;
    
	bool debugMode;

private:
    // Textures
    TextureQuad* logo;
    
    // Display data
    int screenWidth, screenHeight;
    GLuint colorShader, textureShader;
    std:: string currentInstrument = "Piano";
    
    // Song data
    std::vector<songItem> songs;
    Song* activeSong;
    MidiTrack* activeTrack;
	int activeBPM;
	int noteStreak = 0;
    
    // Logic arrays for the game mechanics
    bool currentNotes[128];
    
    //Midi in
    MidiInputReader *midiin;
	
    
    // Menu data
    int activeElement;
    
    // Font data
    GLuint textShader;
    Font* standardFont;
    Font* songFont;
    
    // Render functions
    void renderSongMenu();
    void renderSong();
    void renderMainMenu();
	void renderSongSettings();
	void displaySongPercent();
    void renderPostGame();
	int notesHit();
};

#endif
