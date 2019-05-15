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
#include "Graphics/ParticleSystem.hpp"

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
    TITLE_SCREEN,
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
    
    // Time variables
    GLfloat deltaTime;
    GLfloat frameTime = 0.0f;
    GLfloat inactive = 0.0f;
    
    // Queues for MidiPlayer
    std::queue<int> playerToBeTurnedOn, playerToBeTurnedOff;
    
    // Switches input to computer keyboard
	bool debugMode;

private:
    std::vector <std::pair<int, std::string>> scoreVector;
    void updateScoreVector();
    // Textures Quads
    TextureQuad *sunset, *mountain1, *mountain2, *mountain3, *birds, *clouds1, *clouds2, *clouds3;
    TextureQuad *logo, *strikeBar, *sparkle;
    GLfloat barPosition = -2.6f;
    
    
    // Particle System
    GLuint particleShader;
    ParticleSystem *particles, *bonusParticles;
    
    
    // Display data
    int screenWidth, screenHeight;
    glm::mat4 ortho, projection, view;
    GLuint colorShader, textureShader;
    
    
    // Song data
    std::vector<songItem> songs;
    Song *activeSong;
    MidiTrack *activeTrack;
	int activeBPM, defaultBPM;
    
    std::string difficulty = "NORMAL";
    std::string hands = "BOTH";
    std::vector<std::string> soundfonts;
    int activeSoundfont = 0;
    
    
    // Logic arrays for the game mechanics
    bool currentNotes[128];
    
    //Midi in
    MidiInputReader *midiin;


    // Menu data
    int activeMenuItem_SongSelect;
    int activeMenuItem_SongSettings;
	int activeMenuItem_Settings = 0;

	double durationMulti = 1;

	//Stats data
	char currentLetter = ' ';
	int letterInPlayerName = 0;
	std::string playerName;
    
    // Font data
    GLuint textShader;
    Font *standardFont;
    Font *songFont;
    
    // Render functions
    void renderSongMenu();
    void renderSong();
    void renderMainMenu();
	void renderSongSettings();
	void displaySongPercent();
    void renderPostGame();
    void renderLeaderboard();
    void renderBackground();
    
	int notesHit();
    void leaderboardHandler();
//    bool sortScore(const pair<std::string, double> &a, const pair<std::string, double> &b)

};

#endif
