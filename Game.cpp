#include "Game.hpp"
#include <iostream>
#include <fstream>
// Constructor. Should be declared globally in main. Can not contain GL-related code before the window is initialized.
Game::Game(){
    State = MAIN_MENU;
}



// Destructor
Game::~Game(){
    // Delete dynamically allocated objects
    delete standardFont;
    delete songFont;
    delete activeSong;
    delete activeTrack;
    
    // Delete shader programs
    glDeleteProgram(colorShader);
    glDeleteProgram(textureShader);
    glDeleteProgram(textShader);
}

// Initialize the game. Must be done after window is created.
void Game::init(int displayWidth, int displayHeight){
    // Set resolution
    screenWidth = displayWidth;
    screenHeight = displayHeight;
    
    
    // Default active element in menus
    activeElement = 0;
    
    // Add songs to the song list. Name, filepath, main track, BPM.
    songs.push_back( songItem{ "Twinkle", "MusicLibrary/twinkle.mid", 1, 100, 3.32, "HARD" } );
    songs.push_back( songItem{ "Piano Man", "MusicLibrary/pianoman.mid", 1, 100, 2.32, "Easy" } );
    songs.push_back( songItem{ "Crab Rave", "MusicLibrary/crab_rave.mid", 0, 125 , 1.32, "Easy"} );
    songs.push_back( songItem{ "Mountain King", "MusicLibrary/mountainking.mid", 1, 100, 10.32, "Easy" } );
    songs.push_back( songItem{ "Levels", "MusicLibrary/levels.mid", 0, 128 , 5.22, "Easy"} );
    songs.push_back( songItem{ "Impromptu", "MusicLibrary/impromptu.mid", 0, 168, 5.12, "Easy" } );
    songs.push_back( songItem{ "sex", "MusicLibrary/levels.mid", 0, 128, 7.32, "Easy" } );
    songs.push_back( songItem{ "sju", "MusicLibrary/levels.mid", 0, 128, 1.11 , "Easy"} );
    songs.push_back( songItem{ "atta", "MusicLibrary/levels.mid", 0, 128 , 1.52, "Easy"} );
    songs.push_back( songItem{ "nio", "MusicLibrary/levels.mid", 0, 128 , 10.32, "Easy"} );
    songs.push_back( songItem{ "tio", "MusicLibrary/levels.mid", 0, 128 , 10.32, "Easy"} );
    songs.push_back( songItem{ "elva", "MusicLibrary/levels.mid", 0, 128 , 10.32, "Easy"} );
    songs.push_back( songItem{ "tolv", "MusicLibrary/levels.mid", 0, 128 , 10.32, "Easy"} );
    songs.push_back( songItem{ "tretton", "MusicLibrary/levels.mid", 0, 128 , 10.32, "Easy"} );
    songs.push_back( songItem{ "fjorton", "MusicLibrary/levels.mid", 0, 128 , 10.32, "Easy"} );
    songs.push_back( songItem{ "femton", "MusicLibrary/levels.mid", 0, 128 , 10.32, "Easy"} );
    songs.push_back( songItem{ "sexton", "MusicLibrary/levels.mid", 0, 128 , 10.32, "Easy"} );
    
    // Load shaders
    // Text shader is a shared shader used in the class Font.
    textShader = GraphicsTools::loadShaders( "Graphics/Shaders/TextVertexShader.vertexshader", "Graphics/Shaders/TextFragmentShader.fragmentshader" );
    // Color shader is mainly used in the class Song for rendering the notes.
    colorShader = GraphicsTools::loadShaders( "Graphics/Shaders/ColorVertexShader.vertexshader", "Graphics/Shaders/ColorFragmentShader.fragmentshader" );
    // Texture shader is used for all textured objects. The background is rendered with this shader.
    textureShader = GraphicsTools::loadShaders( "Graphics/Shaders/TextureVertexShader.vertexshader", "Graphics/Shaders/TextureFragmentShader.fragmentshader" );
    
    // Initialize the standard font for the game
    standardFont = new Font("Graphics/Fonts/neon.ttf", textShader, screenWidth, screenHeight);
    songFont = new Font("Graphics/Fonts/neon.ttf", textShader, screenWidth, screenHeight);
    
}


//----- RENDER FUNCTIONS -----//

// The main rendering function. Calls other rendering functions depending on what game state is active.
void Game::render(){
    if(State == MAIN_MENU){
        renderMainMenu();
    }
    if(State == SONG_ACTIVE){
        // Render the song
        renderSong();
    }
    if(State == SONG_SELECT){
        // Render the song menu
        renderSongMenu();
    }
	if (State == SONG_SETTINGS) {
		renderSongSettings();
	}
    if(State == POST_GAME){
        renderPostGame();
    }
    globalScore = score.getScore();

}



void Game::renderSong(){
    // Check player Input
    if(Keys[GLFW_KEY_ENTER]){
        // Switch to song state
        State = POST_GAME;
        Keys[GLFW_KEY_ENTER] = GL_FALSE;
    }
	if (Keys[GLFW_KEY_9]) {
		// Switch to song state
        State =POST_GAME;

		Keys[GLFW_KEY_9] = GL_FALSE;
	}
    activeSong->updateNotes(matchingKeys);
    activeSong->renderBackground();
    activeSong->renderNotes();
    activeSong->renderPiano();
    activeSong->updatePiano(playerInput);

	displaySongPercent();

	if (activeTrack->note(activeTrack->size() - 1)->end < glfwGetTime() - 5)
	{
		State = POST_GAME;
	}
    
    // Update ther current notes array. The notes in the track that should currently be played.
    activeTrack->updateCurrentNotes(currentNotes, glfwGetTime() - 2.5f);
    // Check if the player input matches with current notes. Update matchingKeys.
    for(int i = 0; i < 128; i++){
        if(playerInput[i] && currentNotes[i])
            matchingKeys[i] = true;
        else
            matchingKeys[i] = false;
    }
    
    // Update score
    score.scoreNotes(activeTrack, currentNotes, playerInput, glfwGetTime(), 0.03f);
    // Stop streak if player missed note
	if (activeTrack->missedNotes(glfwGetTime(), 0.5)) {
		if (score.getStreakScore() > noteStreak) {
			noteStreak = score.getStreakScore();
		}
		score.stopStreak();
	}
    
    // Render score and multiplier
    standardFont->setScale(0.5f);
    standardFont->setColor(glm::vec3(0.3f, 0.7f, 0.9f));
    standardFont->renderText("SCORE", 20, screenHeight - 80);
    standardFont->renderText(std::to_string(score.getScore()), 20, screenHeight - 110);
    standardFont->setColor(glm::vec3(0.6f, 0.4f, 0.8f));
    standardFont->renderText("MULTIPLIER", 20, screenHeight - 150);
    standardFont->renderText(std::to_string(score.getMultiplier()), 20, screenHeight - 180);
}



// Render function for the song menu
void Game::renderSongMenu(){
    // Check player Input
    if(Keys[GLFW_KEY_ENTER]){
        // Switch to song state
        State = SONG_SETTINGS;
		activeBPM = songs[activeElement].bpm;
        
        // Reset active element. Next menu should start at element 0.
        //activeElement = 0;
        
        Keys[GLFW_KEY_ENTER] = GL_FALSE;
    }
    else if(activeElement > 0 && Keys[GLFW_KEY_UP]) {
        // Go up the list
        activeElement--;
        Keys[GLFW_KEY_UP] = GL_FALSE;
    }
    else if(activeElement < songs.size()-1 && Keys[GLFW_KEY_DOWN]) {
        // Go down the list
        activeElement++;
        Keys[GLFW_KEY_DOWN] = GL_FALSE;
    }

    
    // Draw list header
    standardFont->setScale(2.0f);
    standardFont->setColor(glm::vec3(0.3f, 0.7f , 0.9f));
    standardFont->renderText("SONGS", 20, screenHeight - screenHeight/10);
    
    // Draw song list
    
    
    int addOn = 0;
    if(activeElement >= 9){
        addOn = -9;
    }
    for(int i = 0; i < songs.size(); i++){
        // Highlight active element in list
        songFont->setScale(1.7f);
        songFont->setColor(glm::vec3(0.45 ,0.04, 0.5664 *-sin(glfwGetTime()) ));
        if(i==activeElement){
            songFont->setColor(glm::vec3(0.85 + sin(glfwGetTime())/4 ,0.00, 0.2 ));
            songFont->renderText(songs[i].name, 20, screenHeight - (i+2 + addOn)*(screenHeight/10));
            songFont->setColor(glm::vec3(0.45 ,0.04, 0.5664 * -sin(glfwGetTime()) ));
        }
        songFont->renderText(songs[i].name, 20, screenHeight - (i+2 + addOn)*(screenHeight/10));
    }
}



void Game::renderMainMenu(){
    // Check player Input
    if(Keys[GLFW_KEY_ENTER]){
        // Switch to song state
        State = SONG_SELECT;
        
        // Reset active element. Next menu should start at element 0.
        activeElement = 0;
        
        Keys[GLFW_KEY_ENTER] = GL_FALSE;
    }
    
    // Draw list header
    standardFont->setScale(1.0f);
    standardFont->setColor(glm::vec3(0.3f, 0.7f, 0.9f));
    standardFont->renderText("Press Enter", screenWidth/2 -180, screenHeight/2);
    standardFont->setColor(glm::vec3(1.0f, 0.0f, 0.0f));
}

void Game::renderSongSettings() {
   
	if (Keys[GLFW_KEY_ENTER])
	{
		State = SONG_ACTIVE;

		// Load new selected song
		delete activeTrack;
		delete activeSong;
		activeTrack = new MidiTrack(songs[activeElement].filepath, songs[activeElement].track, activeBPM);
		activeSong = new Song(*activeTrack, colorShader, textureShader);
		// Reset time
		glfwSetTime(0);

		Keys[GLFW_KEY_ENTER] = GL_FALSE;
	}
    
    if (Keys[GLFW_KEY_9]) {
        soundfont++;
        if(soundfont >3){
            soundfont = 0;
        }
        Keys[GLFW_KEY_9] = GL_FALSE;
    }
   else if (Keys[GLFW_KEY_8]) {
       soundfont--;
       if(soundfont <0){
           soundfont=3 ;
       }
       Keys[GLFW_KEY_8] = GL_FALSE;
	}
	else if (Keys[GLFW_KEY_UP]) {

		activeBPM++;
		Keys[GLFW_KEY_UP] = GL_FALSE;
	}
	else if (Keys[GLFW_KEY_DOWN] && activeBPM > 0) {

		activeBPM--;
		Keys[GLFW_KEY_DOWN] = GL_FALSE;

	}
    
    if(soundfont==0){
        currentInstrument = "Grand Piano";
    }else if(soundfont ==1 ){
        currentInstrument = "Supersaw Synthesizer";
    }else if(soundfont == 2){
        currentInstrument = "Brass Synthesizer";
    }else if (soundfont == 3){
        currentInstrument = "Brighton Synthesizer";
    }
    // Create an output string stream
    std::ostringstream streamObj;
    //Add double to stream
    streamObj << std::fixed <<std::setprecision(2)<< songs[activeElement].duration;
    std::string activeElementDuration = streamObj.str();
    
    standardFont->setScale(1.7f);
	standardFont->setColor(glm::vec3(0.00 ,0.85 + sin(glfwGetTime())/4, 0.2 ));
	standardFont->renderText("BPM:" + std::to_string(activeBPM), screenWidth/3, screenHeight-200);
    standardFont->renderText("Instrument: " + currentInstrument, screenWidth/3, screenHeight-300);
    standardFont->setColor(glm::vec3(0.8f, 0.1f, 0.2f));
    standardFont->renderText("Song Duration:" + activeElementDuration + "M", screenWidth/3, screenHeight-400);
    standardFont->renderText("Song Difficulty:" + songs[activeElement].difficulty, screenWidth/3, screenHeight-500 );


	// Draw list header
	standardFont->setScale(2.0f);
	standardFont->setColor(glm::vec3(0.3f, 0.7f, 0.9f));
	standardFont->renderText("SONG OPTIONS", 20, screenHeight - screenHeight / 10);
    
    
    int addOn = 0;
    if(activeElement >= 9){
        addOn = -9;
    }
    for(int i = 0; i < songs.size(); i++){
        if(i==activeElement){
            songFont->setColor(glm::vec3(0.85 + sin(glfwGetTime())/4 ,0.00, 0.2 ));
            songFont->renderText(songs[activeElement].name, 20, screenHeight - (i+2 + addOn)*(screenHeight/10));
            songFont->setColor(glm::vec3(0.3,0.3,0.3));
        }
        songFont->renderText(songs[i].name, 20, screenHeight - (i+2 + addOn)*(screenHeight/10));
    }
}

void Game:: renderPostGame(){
    standardFont->setScale(2.0f);
    standardFont->renderText("Post Game Screen", screenWidth/2-400, screenHeight - 100);
    standardFont->renderText("Score: " + std:: to_string(score.getScore()), screenWidth/2-400, screenHeight - 300);
	standardFont->renderText("Note Streak: " + std::to_string(noteStreak), screenWidth / 2-400, screenHeight - 400);
	standardFont->renderText("Notes Hit: ", screenWidth / 2 - 400, screenHeight - 500);
}


void Game::displaySongPercent() {

	int percent = 0;
	if (glfwGetTime() > 2.5) {

		double time = activeTrack->note(activeTrack->size() - 1)->end;
		double dummy = 100 * (glfwGetTime() - 2.5) / time;
		percent = (int)(dummy);
	}
	if (percent >= 100)
		percent = 100;

	standardFont->setScale(0.4f);
	standardFont->setColor(glm::vec3(0.6f, 0.0f, 0.0f));
	standardFont->renderText(std::to_string(percent), 20, screenHeight - 30);
	standardFont->renderText(" % of song completed", 55, screenHeight - 30);
}

int Game:: returnSoundfont(){
    return soundfont;
}
