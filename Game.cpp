#include "Game.hpp"
#include <iostream>
#include <fstream>
// Constructor. Should be declared globally in main. Can not contain GL-related code before the window is initialized.
Game::Game(){
    State = MAIN_MENU;
    midiin = new MidiInputReader();
}
// Destructor
Game::~Game(){
    // Delete dynamically allocated objects
    delete standardFont;
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
    
    // Projection matrix: FoV, Aspect Ratio, Display range (0.1 unit <-> 100 units)
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)displayWidth / (float)displayHeight, 0.1f, 100.0f);
    // Camera matrix
    glm::mat4 view = glm::lookAt(
                       glm::vec3(0,-0.8,7), // Camera position
                       glm::vec3(0,-0.8,0),  // The point the camera looks at
                       glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                       );
    viewProjection = projection * view;
    
    // Orthograpic camera:
    ortho = projection = glm::ortho(0.0f, static_cast<GLfloat>(displayWidth), 0.0f, static_cast<GLfloat>(displayHeight));

    
    // Default active element in menus
    activeElement = 0;
    
    // Add songs to the song list. Name, filepath, main track, BPM.
    songs.push_back( songItem{ "Twinkle", "MusicLibrary/twinkle.mid", 1, 100, 0.29, "Easy" } );
    songs.push_back( songItem{ "Brother Jacob", "MusicLibrary/jakob.mid", 0, 100, 0.19, "Easy" } );
    songs.push_back( songItem{ "Piano Man", "MusicLibrary/pianoman.mid", 1, 100, 2.50, "Easy" } );
    songs.push_back( songItem{ "Crab Rave", "MusicLibrary/crab_rave.mid", 0, 125 , 0.46, "Easy"} );
    songs.push_back( songItem{ "Mountain King", "MusicLibrary/mountainking.mid", 1, 100, 1.23, "Easy" } );
    songs.push_back( songItem{ "Levels", "MusicLibrary/levels.mid", 0, 128 , 0.14, "Easy"} );
    songs.push_back( songItem{ "Impromptu", "MusicLibrary/impromptu.mid", 0, 168, 0.57, "Easy" } );
    songs.push_back( songItem{ "Megalovania", "MusicLibrary/megalovania.mid", 0, 120 , 1.50, "Easy"} );
    songs.push_back( songItem{ "Pirates of the Caribbean", "MusicLibrary/pirates.mid", 0, 120 , 0.57, "Easy"} );
    songs.push_back( songItem{ "Can't Help Falling in Love", "MusicLibrary/cant_help_falling_in_love.mid", 0, 120 , 1.27, "Easy"} );
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
    standardFont = new Font("Graphics/Fonts/Orbitron-Black.ttf", textShader, screenWidth, screenHeight);

    logo = new TextureQuad("Graphics/Images/KeySlayerLogo.png", 1789 * 0.5, 786 * 0.5, glm::vec3(screenWidth/2, screenHeight/2, 0.0f), textureShader, true, ortho);
}


//-------------------------------------------//
//----------------- RENDER ------------------//
//-------------------------------------------//

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
    if(State == LEADERBOARD){
        renderLeaderboard();
    }
    // Draw Title
    logo->render();
}




//-------------------------------------------//
//--------------- MAIN MENU -----------------//
//-------------------------------------------//

void Game::renderMainMenu(){
    
    // Check player Input
    if(Keys[GLFW_KEY_ENTER]){
        // Switch to song state
        State = SONG_SELECT;
        
        logo->scale(0.3f);
        logo->position( glm::vec3(screenWidth-200, screenHeight-100, 1.0f) );
        
        // Reset active element. Next menu should start at element 0.
        activeElement = 0;
        
        Keys[GLFW_KEY_ENTER] = GL_FALSE;
    }
    
    // Draw list header
    standardFont->setScale(1.0f);
    standardFont->setColor(glm::vec3(0.015f, 0.517f, 1.0f));
    standardFont->renderText("Press Enter", screenWidth/2 -180, screenHeight/2 - 300);
    
}

//-------------------------------------------//
//--------------- SONG MENU -----------------//
//-------------------------------------------//

// Render function for the song menu
void Game::renderSongMenu(){
    // Check player Input
    if(Keys[GLFW_KEY_RIGHT]){
        // Switch to song state
        State = SONG_SETTINGS;
        activeBPM = songs[activeElement].bpm;
		defaultBPM = songs[activeElement].bpm;
		i = 0;
        // Reset active element. Next menu should start at element 0.
        //activeElement = 0;
        
        Keys[GLFW_KEY_RIGHT] = GL_FALSE;
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
    standardFont->setScale(1.0f);
    standardFont->setColor(glm::vec3(0.015f, 0.517f, 1.0f));
    standardFont->renderText("SONGS", 20, screenHeight - screenHeight/10);
    
    // Draw song list
    float sin = glm::sin(0.3*glfwGetTime());
    sin *= sin;
    standardFont->setColor(glm::vec3(sin*0.827f + (1-sin)*0.015f, sin*0.023f + (1-sin)*0.517f, 1.0f));
    
    int listLocation = activeElement/8;
    for(int i = 0; i < songs.size(); i++){
        // Highlight active element in list
        if(i >= listLocation*8 && i  < (listLocation+1)*8){
            if(i == activeElement){
                standardFont->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
                standardFont->renderText(songs[i].name, 20, screenHeight - (i%8+2)*(screenHeight/10));
                standardFont->setColor(glm::vec3(sin*0.827f + (1-sin)*0.015f, sin*0.023f + (1-sin)*0.517f, 1.0f));
            }
            standardFont->renderText(songs[i].name, 20, screenHeight - (i%8+2)*(screenHeight/10));
        }
    }

	// Create an output string stream
	std::ostringstream streamObj;
	//Add double to stream
	streamObj << std::fixed << std::setprecision(2) << songs[activeElement].duration;
	std::string activeElementDuration = streamObj.str();

	standardFont->setColor(glm::vec3(sin*0.827f + (1 - sin)*0.015f, sin*0.023f + (1 - sin)*0.517f, 1.0f));
	standardFont->renderText(songs[activeElement].name, screenWidth / 3+200, screenHeight - 216);
	standardFont->renderText(songs[activeElement].difficulty, screenWidth / 3 + 400, screenHeight - 316);
	standardFont->renderText(activeElementDuration + "M", screenWidth / 3+200, screenHeight - 316);

	//DRAW leaderboard
	standardFont->setScale(1.0f);
	standardFont->setColor(glm::vec3(0.015f, 0.517f, 1.0f));
	standardFont->renderText("Leaderboard", screenWidth / 3 + 200, screenHeight - 416);
	std::fstream readerFile;
	readerFile.open("Leaderboards/" + songs[activeElement].name + ".txt", std::ios::in);
	std::string line;
	int i = 0;
	while (std::getline(readerFile, line)) {
		standardFont->renderText(line, screenWidth / 3 + 200, screenHeight - 416 - i);
		i = i + 50;
	}
}



//-------------------------------------------//
//------------- SONG SETTINGS ---------------//
//-------------------------------------------//




void Game::renderSongSettings() {
    
    /*if (Keys[GLFW_KEY_ENTER])
    {
        State = SONG_ACTIVE;
        
        // Load new selected song
        delete activeTrack;
        delete activeSong;
        activeTrack = new MidiTrack(songs[activeElement].filepath, songs[activeElement].track, activeBPM);
        activeSong = new Song(*activeTrack, colorShader, textureShader, viewProjection);
        // Lengthens the start of track notes. Has to be done after creation of Song.
        activeTrack->setStartOffset(0.1);
        // Reset time
        glfwSetTime(0);
        
        Keys[GLFW_KEY_ENTER] = GL_FALSE;
    }*/
    
    if (Keys[GLFW_KEY_LEFT])
    {
        State = SONG_SELECT;
		difficulty = "NORMAL";
		hands = "BOTH";
		durationMulti = 1;
        Keys[GLFW_KEY_LEFT] = GL_FALSE;
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
 
    standardFont->setScale(1.0f);

    /////////////////////////////////
	float sin = glm::sin(0.3*glfwGetTime());
	sin *= sin;
    
        if(Keys[GLFW_KEY_DOWN]){
            i++;
            if(i >3){
                i = 0;
				j = 0;
            }
            Keys[GLFW_KEY_DOWN] = GL_FALSE;
        }
        if(Keys[GLFW_KEY_UP]){
            i--;
            if(i <0){
                i = 3;
				j = 0;
            }
            Keys[GLFW_KEY_UP] = GL_FALSE;
        }
        if(i == 0){
			
		standardFont->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
		standardFont->renderText("Speed: " + difficulty, screenWidth / 3+200, screenHeight - 450);
        standardFont->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        
		
            if (Keys[GLFW_KEY_RIGHT]) {

				j++;

				if (j == 1)
				{
					activeBPM = defaultBPM*0.75;
					difficulty = "SLOWER";
					durationMulti = (1.0/0.75);
				}
				else if (j == 2)
				{
					activeBPM = defaultBPM*0.5;
					difficulty = "SLOWEST";
					durationMulti = 2;
				}
				else if (j > 2) {
					activeBPM = defaultBPM;
					difficulty = "NORMAL";
					durationMulti = 1;
					j = 0;
				}
				
                Keys[GLFW_KEY_RIGHT] = GL_FALSE;
            }
        
        }
    
        if(i == 1){
			standardFont->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
			standardFont->renderText("Instrument: " + currentInstrument, screenWidth / 3+200, screenHeight - 550);
            standardFont->setColor(glm::vec3(0.8f, 0.1f, 0.2f));
        
            if (Keys[GLFW_KEY_RIGHT]){
                soundfont++;
                if(soundfont >3){
                    soundfont = 0;
                }
                Keys[GLFW_KEY_RIGHT] = GL_FALSE;
            }
                                    
        }
		if (i == 2)
		{
			standardFont->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
			standardFont->renderText("Hands: " + hands, screenWidth / 3+200, screenHeight - 650);
			standardFont->setColor(glm::vec3(1.0f, 1.0f, 1.0f));


			if (Keys[GLFW_KEY_RIGHT]) {

				j++;

				if (j == 1){

					hands = "RIGHT";
				}
				else if (j == 2)
				{
					
					hands = "LEFT";
				}
				else if (j > 2) {
					
					hands = "BOTH";
					j = 0;
				}

				Keys[GLFW_KEY_RIGHT] = GL_FALSE;
			}
		}
		if (i == 3) {
			standardFont->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
			standardFont->renderText("PLAY", screenWidth / 3 + 200, screenHeight - 750);
			if (Keys[GLFW_KEY_RIGHT])
			{
				State = SONG_ACTIVE;

				// Load new selected song
				delete activeTrack;
				delete activeSong;
				activeTrack = new MidiTrack(songs[activeElement].filepath, songs[activeElement].track, activeBPM);
				activeSong = new Song(*activeTrack, colorShader, textureShader);
				// Lengthens the start of track notes. Has to be done after creation of Song.
				std::cout << activeTrack->note(activeTrack->size() - 1)->end;
				activeTrack->setStartOffset(0.1);
				// Reset time
				glfwSetTime(0);

				Keys[GLFW_KEY_RIGHT] = GL_FALSE;
			}
		}
    
    /*if(Keys[GLFW_KEY_ENTER]){
        i = 4;
        Keys[GLFW_KEY_ENTER] = false;
    }*/

	// Create an output string stream
	std::ostringstream streamObj;
	//Add double to stream
	streamObj << std::fixed << std::setprecision(2) << songs[activeElement].duration*durationMulti;
	std::string activeElementDuration = streamObj.str();

	
	standardFont->setColor(glm::vec3(sin*0.827f + (1 - sin)*0.015f, sin*0.023f + (1 - sin)*0.517f, 1.0f));
	standardFont->renderText(songs[activeElement].name, screenWidth / 3 + 200, screenHeight - 216);
	standardFont->renderText(songs[activeElement].difficulty, screenWidth / 3 + 400, screenHeight - 316);
	standardFont->renderText(activeElementDuration + "M", screenWidth / 3+200, screenHeight - 316);
    standardFont->renderText("Speed: " + difficulty, screenWidth/3+200, screenHeight-450);
    standardFont->renderText("Instrument: " + currentInstrument, screenWidth/3+200, screenHeight-550);
	standardFont->renderText("Hands: " + hands, screenWidth / 3+200, screenHeight - 650);
	standardFont->renderText("PLAY", screenWidth / 3+200, screenHeight - 750);
  

    
    // Draw list header
    standardFont->setScale(1.0f);
    standardFont->setColor(glm::vec3(0.3f, 0.7f, 0.9f));
    standardFont->renderText("SONG OPTIONS", 20, screenHeight - screenHeight / 10);
    
    /////////////////////////////////////////
    
    // Draw song list
    standardFont->setColor(glm::vec3(0.3,0.3,0.3));
    int listLocation = activeElement/8;
    for(int i = 0; i < songs.size(); i++){
        // Highlight active element in list
        if(i >= listLocation*8 && i  < (listLocation+1)*8){
            if(i==activeElement){
				standardFont->setColor(glm::vec3(sin*0.827f + (1 - sin)*0.015f, sin*0.023f + (1 - sin)*0.517f, 1.0f));
                standardFont->renderText(songs[activeElement].name, 20, screenHeight - (i%8+2)*(screenHeight/10));
                standardFont->setColor(glm::vec3(0.3,0.3,0.3));
            }
            standardFont->renderText(songs[i].name, 20, screenHeight - (i%8+2)*(screenHeight/10));
        }
    }
    
}

//-------------------------------------------//
//--------------- LEADERBOARD -----------------//
//-------------------------------------------//
void Game:: renderLeaderboard(){
	if (Keys[GLFW_KEY_BACKSPACE]) {
		// Switch to song state
		State = SONG_SETTINGS;
		Keys[GLFW_KEY_BACKSPACE] = GL_FALSE;
	}
    standardFont->setScale(1.0f);
    standardFont->setColor(glm::vec3(0.015f, 0.517f, 1.0f));
    standardFont->renderText(songs[activeElement].name +" Leaderboard", screenWidth/2 -180, screenHeight/2 + 400);
    std::fstream readerFile;
    readerFile.open("Leaderboards/" + songs[activeElement].name +".txt", std::ios::in);
    std:: string line;
    int i = 0;
    while(std::getline(readerFile, line)){
        standardFont->renderText(line, screenWidth/2 -180, screenHeight/2 +350- i);
        i= i + 50;
    }

}


//-------------------------------------------//
//-----------------  SONG -------------------//
//-------------------------------------------//

void Game::renderSong(){
    //Update user input
    if (!debugMode) {
        midiin->getUserInput();
        for (int i = 0; i < 127; i++) playerInput[i] = midiin->playerInput[i];
    }
    
    if(Keys[GLFW_KEY_ENTER]){
        // Switch to song state
        State = POST_GAME;
        Keys[GLFW_KEY_ENTER] = GL_FALSE;
    }
    activeSong->updateNotes(matchingKeys);
    activeSong->updatePiano(playerInput);
    activeSong->render();
    
    displaySongPercent();
    
    if (activeTrack->note(activeTrack->size() - 1)->end < glfwGetTime() - 5)
    {
        State = POST_GAME;
    }
    
    // Update the current notes array. The notes in the track that should currently be played.
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
    
    // Render score and multiplier
    standardFont->setScale(0.5f);
    standardFont->setColor(glm::vec3(0.3f, 0.7f, 0.9f));
    standardFont->renderText("SCORE", 20, screenHeight - 80);
    standardFont->renderText(std::to_string(score.getScore()), 20, screenHeight - 110);
    standardFont->setColor(glm::vec3(0.6f, 0.4f, 0.8f));
    standardFont->renderText("MULTIPLIER", 20, screenHeight - 150);
    
    //Change color of multiplier depending on multiplier
    glm::vec3 multiColor;
    switch (score.getMultiplier())
    {
        case 1:
            multiColor = glm::vec3(1.0f, 1.0f, 1.0f);
            break;
        case 2:
            multiColor = glm::vec3(1.0f, 1.0f, 0.0f);
            break;
        case 3:
            multiColor = glm::vec3(0.0f, 0.5f, 0.0f);
            break;
        case 4:
            multiColor = glm::vec3(0.5f, 0.0f, 0.5f);
            break;
        default:
            break;
    }
    standardFont->setColor(multiColor);
    standardFont->renderText(std::to_string(score.getMultiplier()), 20, screenHeight - 180);
}

//-------------------------------------------//
//--------------- POST GAME -----------------//
//-------------------------------------------//
void Game:: renderPostGame(){
    
    if (Keys[GLFW_KEY_ENTER]){
        leaderboardHandler();
        State = SONG_SELECT;
        glfwSetTime(0);
        activeElement = 0;
        score.reset();
		playerName = "";
        
        Keys[GLFW_KEY_ENTER] = GL_FALSE;
    }
    if (Keys[GLFW_KEY_X]){
        leaderboardHandler();
        State = LEADERBOARD;
        glfwSetTime(0);
        activeElement = 0;
        score.reset();
        
        Keys[GLFW_KEY_X] = GL_FALSE;
    }
    
    standardFont->setColor(glm::vec3(0.6f, 0.4f, 0.8f));
    standardFont->setScale(1.0f);
    standardFont->renderText("Post Game Screen", 20, screenHeight - 100);
    standardFont->renderText("Score: " + std:: to_string(score.getScore()), 20, screenHeight - 300);
	standardFont->renderText("Notes Hit: " + std::to_string(notesHit()) +"%", 20, screenHeight - 500);

	
	if (Keys[GLFW_KEY_UP]) {
		if (alfaBet > 'A')
		{
			alfaBet--;
		}
		Keys[GLFW_KEY_UP] = GL_FALSE;
	}
	else if (Keys[GLFW_KEY_DOWN]) {
		if (alfaBet < 'Z' )
		{
			alfaBet++;
		}
		Keys[GLFW_KEY_DOWN] = GL_FALSE;
	}
	std::string s(1, alfaBet);
	if (playerName.size() < 11)
	{
		if (Keys[GLFW_KEY_RIGHT])
		{
			playerName += s;
			Keys[GLFW_KEY_RIGHT] = GL_FALSE;
			alfaBet = 'A';
		}
	}
	standardFont->renderText("Name: " + playerName + s, 20, screenHeight - 700);
	
}



//-------------------------------------------//
//------------- OTHER FUNCTIONS -------------//
//-------------------------------------------//

void Game:: leaderboardHandler(){
    std::fstream file;
    std:: string line;
    file.open("Leaderboards/"+ songs[activeElement].name + ".txt",std::ios_base::app);
    file << std::endl << playerName << ": " <<score.getScore() ;
    
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

int Game::notesHit()
{
    int total = activeTrack->size();
    int hit = activeTrack->countTriggeredNotes();
    
    double notesHitPercent = ((double)hit / (double)total)*100;
    
    return notesHitPercent;
}
