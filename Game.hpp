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
#include "MidiPlayer/cmidiin.h"

#include <iostream>
#include <fstream>
#include <sstream>

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
    POST_GAME,
    LEADERBOARD
    
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
    glm::mat4 projection, view;
    GLuint colorShader, textureShader;
    
    // Song data
    std::vector<songItem> songs;
    Song* activeSong;
    MidiTrack* activeTrack;
    std:: string currentInstrument = "Piano";
	int activeBPM;
	int defaultBPM;
    
    // Logic arrays for the game mechanics
    bool currentNotes[128];
    
    //Midi in
    MidiInputReader *midiin;

	int i = 0;
	
    // Menu data
    int activeElement;
	int j = 0;
	std::string difficulty = "NORMAL";

	//Stats data
	char alfaBet = 'A';
	std::string playerName;
    
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
    void renderLeaderboard();
    
	int notesHit();
    void leaderboardHandler();

};

#endif
