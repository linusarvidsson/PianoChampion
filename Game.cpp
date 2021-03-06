#include "Game.hpp"
#include <iostream>
#include <fstream>
#include <utility>

// Constructor. Should be declared globally in main. Can not contain GL-related code before the window is initialized.
Game::Game(){
    State = TITLE_SCREEN;
    midiin = new MidiInputReader();
}

// Destructor
Game::~Game(){
    // Delete dynamically allocated objects
    delete standardFont;
    delete neonFont;
    delete activeSong;
    delete activeTrack;
    
    delete logo;
    delete sign;
    delete signBorder;
    delete gradient;
    delete sparkle;
    delete strikeBar;
    delete arrow;
    delete progressBar;
    delete progressBorder;
    
    delete sunset;
    delete mountain1;
    delete mountain2;
    delete mountain3;
    delete birds;
    delete clouds1;
    delete clouds2;
    delete clouds3;
    
    delete midiin;
    
    // Delete shader programs
    glDeleteProgram(colorShader);
    glDeleteProgram(textureShader);
    glDeleteProgram(textShader);
    glDeleteProgram(particleShader);
}   

// Initialize the game. Must be done after window is created.
void Game::init(int displayWidth, int displayHeight){
    // Set resolution
    screenWidth = displayWidth;
    screenHeight = displayHeight;
    
    // Projection matrix: FoV, Aspect Ratio, Display range (0.1 unit <-> 100 units)
    projection = glm::perspective(glm::radians(45.0f), (float)displayWidth / (float)displayHeight, 0.1f, 100.0f);
    // Camera matrix
    view = glm::lookAt(
                       glm::vec3(0,-0.8,7), // Camera position
                       glm::vec3(0,-0.8,0),  // The point the camera looks at
                       glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                       );
    
    // Orthograpic camera:
    ortho = glm::ortho(0.0f, static_cast<GLfloat>(displayWidth), 0.0f, static_cast<GLfloat>(displayHeight));

    
    // Default active element in menus
    activeMenuItem_SongSelect = 0;
    
    // Add songs to the song list. Name, filepath, main track, BPM.
    songs.push_back( songItem{ "Twinkle", "MusicLibrary/twinkle.mid", 1, 100, 0.29, "Easy" } );
    songs.push_back( songItem{ "Broder Jakob", "MusicLibrary/jakob.mid", 0, 100, 0.19, "Easy" } );
    songs.push_back( songItem{ "Piano Man", "MusicLibrary/pianoman.mid", 1, 100, 2.50, "Hard" } );
    songs.push_back( songItem{ "Crab Rave", "MusicLibrary/crab_rave.mid", 0, 125 , 0.46, "Hard"} );
    songs.push_back( songItem{ "Mountain King", "MusicLibrary/mountainking.mid", 1, 100, 1.23, "Easy" } );
    songs.push_back( songItem{ "Levels", "MusicLibrary/levels.mid", 0, 128 , 0.14, "Easy"} );
    songs.push_back( songItem{ "Impromptu", "MusicLibrary/impromptu.mid", 0, 168, 0.57, "Impossible" } );
    songs.push_back( songItem{ "Megalovania", "MusicLibrary/megalovania.mid", 0, 120 , 1.50, "Medium"} );
    songs.push_back( songItem{ "He's a Pirate", "MusicLibrary/pirates.mid", 0, 120 , 0.57, "Easy"} );
    songs.push_back( songItem{ "Can't Help Falling in Love", "MusicLibrary/cant_help_falling_in_love.mid", 0, 120 , 1.27, "Medium"} );
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
    
    // Create vector with names of available soundfonts
    soundfonts.push_back("Grand Piano");
    soundfonts.push_back("Supersaw Synth");
    soundfonts.push_back("Brass Synth");
    soundfonts.push_back("Brighton Synth");
    
    
    // Load shaders
    // Text shader is a shared shader used in the class Font.
    textShader = GraphicsTools::loadShaders( "Graphics/Shaders/TextVertexShader.vertexshader", "Graphics/Shaders/TextFragmentShader.fragmentshader" );
    // Color shader is mainly used in the class Song for rendering the notes.
    colorShader = GraphicsTools::loadShaders( "Graphics/Shaders/ColorVertexShader.vertexshader", "Graphics/Shaders/ColorFragmentShader.fragmentshader" );
    // Texture shader is used for all textured objects. The background is rendered with this shader.
    textureShader = GraphicsTools::loadShaders( "Graphics/Shaders/TextureVertexShader.vertexshader", "Graphics/Shaders/TextureFragmentShader.fragmentshader" );
    // Particle shader. Only used for the particle system.
    particleShader = GraphicsTools::loadShaders( "Graphics/Shaders/ParticleVertexShader.vertexshader", "Graphics/Shaders/ParticleFragmentShader.fragmentshader" );
    
    // Initialize the standard font for the game
    standardFont = new Font("Graphics/Fonts/Beon.ttf", textShader, screenWidth, screenHeight);
    neonFont = new Font("Graphics/Fonts/Beon.ttf", textShader, screenWidth, screenHeight);
    
    // Background textures
    sunset = new TextureQuad("Graphics/Images/sunset.png", 19.2f, 10.8f, glm::vec3(0.0f, 0.0f, -3.0f), textureShader, false, projection, view);
    mountain1 = new TextureQuad("Graphics/Images/mountain_1.png", 19.0f, 10.0f, glm::vec3(0.0f, -1.5f, -2.4f), textureShader, true, projection, view);
    mountain2 = new TextureQuad("Graphics/Images/mountain_2.png", 13.47f * 0.8f, 7.39f * 0.8f, glm::vec3(-1.8f, 0.6f, -2.7f), textureShader, true, projection, view);
    mountain3 = new TextureQuad("Graphics/Images/mountain_3.png", 9.6f * 0.9f, 6.52f * 0.9f, glm::vec3(4.0f, 0.8f, -2.6f), textureShader, true, projection, view);
    birds = new TextureQuad("Graphics/Images/birds.png", 10.24f * 0.3f, 6.83f * 0.3f, glm::vec3(0.0f, 0.5f, -2.65f), textureShader, true, projection, view);
    clouds1 = new TextureQuad("Graphics/Images/clouds_1.png", 10.58f * 0.4f, 4.7f * 0.4f, glm::vec3(3.3f, 1.5f, -2.9f), textureShader, true, projection, view);
    clouds2 = new TextureQuad("Graphics/Images/clouds_2.png", 8.26f * 0.8f, 3.53f * 0.8f, glm::vec3(-3.0f, 0.0f, -2.49f), textureShader, true, projection, view);
    clouds3 = new TextureQuad("Graphics/Images/clouds_3.png", 10.53f * 0.4f, 6.23f * 0.4f, glm::vec3(5.0f, -0.5f, -2.44f), textureShader, true, projection, view);
    
    
    // Load logo texture
    logo = new TextureQuad("Graphics/Images/KeySlayerLogo.png", 1789 * 0.5f, 786 * 0.5f, glm::vec3(screenWidth/2, screenHeight/2, 0.0f), textureShader, true, ortho, glm::mat4(1.0f));
    // Load sign texture
    gradient = new TextureQuad("Graphics/Images/menu_gradient.png", screenHeight * 0.8f, screenHeight, glm::vec3(screenHeight * 0.4f, screenHeight/2, -0.0002f), textureShader, true, ortho, glm::mat4(1.0f));
    sign = new TextureQuad("Graphics/Images/sign.png", 800 * 0.8f, 600 * 0.8f, glm::vec3(screenWidth -330, 300, -0.0002f), textureShader, true, ortho, glm::mat4(1.0f));
    signBorder = new TextureQuad("Graphics/Images/sign_border.png", 800 * 0.8f, 600 * 0.8f, glm::vec3(screenWidth -330, 300, -0.0001f), textureShader, true, ortho, glm::mat4(1.0f));
    // Load strike bar, used when playing song.
    strikeBar = new TextureQuad("Graphics/Images/strike_bar.png", 10.0f, 1.0f, glm::vec3(0.0f, barPosition, 0.1f), textureShader, true, projection, view);
    
    
    progressBar = new TextureQuad("Graphics/Images/progress_bar.png", 200, 20, glm::vec3(0.0f, 0.0f, 0.1f), textureShader, true, ortho, glm::mat4(1.0f));
    progressBorder = new TextureQuad("Graphics/Images/progress_bar_border.png", 200, 20, glm::vec3(0.0f, 0.0f, 0.1f), textureShader, true, ortho, glm::mat4(1.0f));
    
    // Load arrow texture
    arrow = new TextureQuad("Graphics/Images/arrow.png", 64 * 0.5f, 64 * 0.5f, glm::vec3(0.0f, 0.0f, 0.0001f), textureShader, true, ortho, glm::mat4(1.0f));
    
    // Set up the particle system
    glUseProgram(particleShader);
    glUniform4f(glGetUniformLocation(particleShader, "color"), 1.0f, 1.0f, 1.0f, 1.0f);
    sparkle = new TextureQuad("Graphics/Images/sparkle.png", 0.15f, 0.15f, glm::vec3(0.0f, 0.0f, 0.0f), particleShader, true, projection, view);
    particles = new ParticleSystem(particleShader, *sparkle, 100, glm::vec3(0.0f, barPosition, 0.0001f), false);
    bonusParticles = new ParticleSystem(particleShader, *sparkle, 100, glm::vec3(0.0f, barPosition, 0.0001f), true);
    updateScoreVector();
}





//-------------------------------------------//
//----------------- RENDER ------------------//
//-------------------------------------------//

// The main rendering function. Calls other rendering functions depending on what game state is active.
void Game::render(){
    // Update application time variables
    deltaTime = (float)glfwGetTime() - frameTime;
    frameTime = (float)glfwGetTime();
    inactive += deltaTime;
    
    // Check game state and render
    if(State == TITLE_SCREEN){
        renderBackground();
        renderMainMenu();
        logo->render();
    }
    else if(State == SONG_ACTIVE){
        // Render the song
        renderSong();
    }
    else if(State == SONG_SELECT){
        renderBackground();
        renderSongMenu();
        logo->render();
    }
    else if (State == SONG_SETTINGS){
        renderBackground();
        renderSongSettings();
        logo->render();
    }
    else if(State == POST_GAME){
        renderBackground();
        renderPostGame();
        logo->render();
    }
    else if(State == LEADERBOARD){
        renderBackground();
        renderLeaderboard();
        logo->render();
    }
    
    // If player is inactive for more than 30 seconds in the menus, return to title screen.
    if(State != SONG_ACTIVE && State != TITLE_SCREEN && inactive > 60.0f){
        State = TITLE_SCREEN;
        // Scale up logo and move to center of the screen
        logo->scaleQuad(4.0f);
        logo->position( glm::vec3(screenWidth/2, screenHeight/2, 0.0f) );
    }
    
}

//-------------------------------------------//
//-------------- TITLE SCREEN ---------------//
//-------------------------------------------//

void Game::renderMainMenu(){
    
    // Check player Input
    if(Keys[GLFW_KEY_ENTER]){
        // Switch to song state
        State = SONG_SELECT;
        
        // Scale down logo and move to corner of the screen
        logo->scaleQuad(0.25f);
        logo->position( glm::vec3(screenWidth-160, screenHeight-80, 1.0f) );
        
        // Reset active element. Next menu should start at element 0.
        activeMenuItem_SongSelect = 0;
        
        Keys[GLFW_KEY_ENTER] = GL_FALSE;
    }
    
    // Draw text Press Enter
    standardFont->setScale(1.0f);
    standardFont->setColor(glm::vec3(0.015f, 0.517f, 1.0f));
    standardFont->renderText("Press Select", screenWidth/2 -180, screenHeight/2 - 300);
    
}





//-------------------------------------------//
//--------------- SONG MENU -----------------//
//-------------------------------------------//

// Render function for the song menu
void Game::renderSongMenu(){
    
    activeSoundfont = 5;

    // Menu navigation
    if(Keys[GLFW_KEY_ENTER]){
        // Switch to song state
        State = SONG_SETTINGS;
        activeBPM = songs[activeMenuItem_SongSelect].bpm;
		defaultBPM = songs[activeMenuItem_SongSelect].bpm;
		activeMenuItem_SongSettings = 0;
        
        Keys[GLFW_KEY_ENTER] = GL_FALSE;
    }
    else if(Keys[GLFW_KEY_BACKSPACE]){
        State = TITLE_SCREEN;
        // Scale up logo and move to center of the screen
        logo->scaleQuad(4.0f);
        logo->position( glm::vec3(screenWidth/2, screenHeight/2, 0.0f) );
    }
    else if(activeMenuItem_SongSelect > 0 && Keys[GLFW_KEY_UP]) {
        // Go up the menu list
        activeMenuItem_SongSelect--;
        updateScoreVector();
        Keys[GLFW_KEY_UP] = GL_FALSE;
    }
    else if(activeMenuItem_SongSelect < songs.size()-1 && Keys[GLFW_KEY_DOWN]) {
        // Go down the menu list
        activeMenuItem_SongSelect++;
        updateScoreVector();
        Keys[GLFW_KEY_DOWN] = GL_FALSE;
    }
    
    // Set neon color depending on selected song
    glm::vec3 neonColor;
    if(activeMenuItem_SongSelect % 3 == 0){
        neonColor = glm::vec3(1.0f, 0.2f, 0.2f);
    }
    else if(activeMenuItem_SongSelect % 3 == 1){
        neonColor = glm::vec3(0.2f, 1.0f, 0.2f);
    }
    else{
        neonColor = glm::vec3(0.5f, 0.5f, 1.0f);
    }
    neonColor *= 0.5f;
    

    // Render sign
    sign->render();
    signBorder->setColor(neonColor);
    signBorder->render();
    gradient->render();
    
    // Draw list header
    standardFont->setScale(1.0f);
    standardFont->setColor(glm::vec3(0.92f, 0.1f, 0.42f));
    standardFont->renderText("SONGS", 20, screenHeight - screenHeight/10);
    
    // Calculate pulsating color
    float sin = glm::sin(0.3*frameTime);
    sin *= sin;
    glm::vec3 pulse = glm::vec3(sin*0.827f + (1-sin)*0.015f, sin*0.023f + (1-sin)*0.517f, 1.0f);
    standardFont->setColor(pulse);
    
    // Draw song list
    int listLocation = activeMenuItem_SongSelect/8;
    for(int i = 0; i < songs.size(); i++){
        // Highlight active element in list
        if(i >= listLocation*8 && i < (listLocation+1)*8){
            if(i == activeMenuItem_SongSelect){
                standardFont->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
                standardFont->renderText(songs[i].name, 20, screenHeight - (i%8+3)*(screenHeight/11));
				//standardFont->renderText(">" , screenWidth/2 - screenWidth/9, screenHeight - (i % 8 + 2)*(screenHeight / 10));
                standardFont->setColor(glm::vec3(pulse));
            }
            standardFont->renderText(songs[i].name, 20, screenHeight - (i%8+3)*(screenHeight/11));
			//standardFont->renderText(">", screenWidth/3 , screenHeight - (i % 8 + 2)*(screenHeight / 10));
        }
    }
    
    // Draw arrows in the song menu (up and down)
    arrow->reset();
    if(listLocation < songs.size()/8){
        arrow->setColor(pulse);
        arrow->translate(35, 30, 0);
        arrow->rotate(180.0f);
        arrow->scale(1.0 + glm::sin(frameTime * 2.5f)*0.05f);
    }
    else{
        arrow->setColor(pulse * 0.4f);
        arrow->translate(35, 30, 0);
        arrow->rotate(180.0f);
    }
    arrow->render();
    
    arrow->reset();
    if(listLocation > 0){
        arrow->setColor(pulse);
        arrow->translate(35, screenHeight - 2*screenHeight/11 + 10, 0);
        arrow->scale(1.0 + glm::sin(frameTime * 2.5f)*0.05f);
    }
    else{
        arrow->setColor(pulse * 0.4f);
        arrow->translate(35, screenHeight - 2*screenHeight/11 + 10, 0);
    }
    arrow->render();

	// Create an output string stream
	std::ostringstream streamObj;
	//Add double to stream
	streamObj << std::fixed << std::setprecision(2) << songs[activeMenuItem_SongSelect].duration;
	std::string activeMenuItem_SongSelectDuration = streamObj.str();
    
    
    // Draw song information
    neonFont->setScale(1.0f);
    neonFont->setColor(neonColor);
	neonFont->renderText(songs[activeMenuItem_SongSelect].name, screenWidth -335 -neonFont->getSize(songs[activeMenuItem_SongSelect].name)/2, 430);
    
    neonFont->setScale(0.8f);
	neonFont->renderText(songs[activeMenuItem_SongSelect].difficulty, screenWidth -560, 360);
    std::string duration = activeMenuItem_SongSelectDuration + " min";
    neonFont->renderText(duration, screenWidth -100 -neonFont->getSize(duration), 360);

    // Render list of top 3 players in the leaderboard inside the neon sign
	neonFont->renderText("Top Players", screenWidth -335 -neonFont->getSize("Top Players")/2, 290);
    neonFont->renderText("_________________", screenWidth -335 -neonFont->getSize("_________________")/2, 280);
    for(int i = 0; i < 3; i++){
        neonFont->renderText(std::to_string(i+1) + ".", screenWidth -590, 230 - 50*i);
        neonFont->renderText(scoreVector[i].second + ":  " + std::to_string(scoreVector[i].first), screenWidth -540, 230 - 50*i);
    }
    
    neonFont->setScale(0.6f + glm::sin(frameTime)*0.01f);
    neonFont->setColor(neonColor * 1.6f);
    neonFont->renderText("Press Select to Choose Song", screenWidth -335 -neonFont->getSize("Press Select to Choose Song")/2, 23);
    
	
}





//-------------------------------------------//
//------------- SONG SETTINGS ---------------//
//-------------------------------------------//

void Game::renderSongSettings() {
    // Menu navigation
    if (Keys[GLFW_KEY_BACKSPACE]){
        // Reset song settings if player goes back to song select
        State = SONG_SELECT;
        activeSoundfont = 0;
		difficulty = "NORMAL";
		hands = "BOTH";
		durationMulti = 1;
        Keys[GLFW_KEY_BACKSPACE] = GL_FALSE;
    }
    if(Keys[GLFW_KEY_DOWN]){
        activeMenuItem_SongSettings++;
        if(activeMenuItem_SongSettings >2){
            activeMenuItem_SongSettings = 0;
            activeMenuItem_Settings = 0;
        }
        Keys[GLFW_KEY_DOWN] = GL_FALSE;
    }
    if(Keys[GLFW_KEY_UP]){
        activeMenuItem_SongSettings--;
        if(activeMenuItem_SongSettings <0){
            activeMenuItem_SongSettings = 2;
            activeMenuItem_Settings = 0;
        }
        Keys[GLFW_KEY_UP] = GL_FALSE;
    }
    if (Keys[GLFW_KEY_ENTER])
    {
        activeSoundfont = willBeSoundfont;
        State = SONG_ACTIVE;
        
        // Load new selected song
        delete activeTrack;
        delete activeSong;
        activeTrack = new MidiTrack(songs[activeMenuItem_SongSelect].filepath, songs[activeMenuItem_SongSelect].track, activeBPM);
        activeSong = new Song(*activeTrack, colorShader, textureShader, *particles, *bonusParticles, projection, view);
        // Lengthens the start of track notes. Has to be done after creation of Song.
        std::cout << activeTrack->note(activeTrack->size() - 1)->end;
        activeTrack->setStartOffset(0.1);
        // Reset time
        glfwSetTime(0);
        frameTime = 0.0f;
        
        Keys[GLFW_KEY_ENTER] = GL_FALSE;
    }
    

    
    // Create an output string stream
    std::ostringstream streamObj;
    // Add double to stream
    streamObj << std::fixed << std::setprecision(2) << songs[activeMenuItem_SongSelect].duration*durationMulti;
    std::string activeMenuItem_SongSelectDuration = streamObj.str();
    
    // Set neon color depending on selected song
    glm::vec3 neonColor;
    if(activeMenuItem_SongSelect % 3 == 0){
        neonColor = glm::vec3(1.0f, 0.2f, 0.2f);
    }
    else if(activeMenuItem_SongSelect % 3 == 1){
        neonColor = glm::vec3(0.2f, 1.0f, 0.2f);
    }
    else{
        neonColor = glm::vec3(0.5f, 0.5f, 1.0f);
    }
    
    gradient->render();
    
    // Calculate the pulsating color
    standardFont->setScale(1.0f);
    float sin = glm::sin(0.3*frameTime);
    sin *= sin;
    glm::vec3 pulse = glm::vec3(sin*0.827f + (1-sin)*0.015f, sin*0.023f + (1-sin)*0.517f, 1.0f);
    
    // Render arrows (up and down) in song menu
    arrow->setColor(pulse * 0.4f);
    arrow->reset();
    arrow->translate(35, 30, 0);
    arrow->rotate(180.0f);
    arrow->render();
    arrow->reset();
    arrow->translate(35, screenHeight - 2*screenHeight/11 + 10, 0);
    arrow->render();

    // Render neon sign
    sign->render();
    signBorder->setColor(neonColor);
    signBorder->render();
    
    // Font settings
    neonFont->setScale(0.8f);

    // Change color of arrow
    arrow->setColor(neonColor);
    
    //----- SELECT TEMPO -----//
    if(activeMenuItem_SongSettings == 0){
        neonFont->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        neonFont->renderText(difficulty + " speed", screenWidth -335 -neonFont->getSize(difficulty + " speed")/2, 230);
        neonFont->setColor(neonColor);
        
        // Render arrows
        arrow->reset();
        arrow->translate(screenWidth -550, 245, 0);
        arrow->rotate(90.0f);
        arrow->scale(1.0 + glm::sin(frameTime * 2.5f)*0.05f);
        arrow->render();
        arrow->reset();
        arrow->translate(screenWidth -120, 245, 0);
        arrow->rotate(-90.0f);
        arrow->scale(1.0 + glm::sin(frameTime * 2.5f)*0.05f);
        arrow->render();
        
        if (Keys[GLFW_KEY_RIGHT] || Keys[GLFW_KEY_LEFT]){
            
            if (Keys[GLFW_KEY_RIGHT]) activeMenuItem_Settings++;
            if (Keys[GLFW_KEY_LEFT]) activeMenuItem_Settings--;

            if (activeMenuItem_Settings == 1)
            {
                activeBPM = defaultBPM*0.75;
                difficulty = "SLOWER";
                durationMulti = (1.0/0.75);
            }
            else if (activeMenuItem_Settings == 2 || activeMenuItem_Settings < 0)
            {
                activeBPM = defaultBPM*0.5;
                difficulty = "SLOWEST";
                durationMulti = 2;
                activeMenuItem_Settings = 2;
            }
            else if (activeMenuItem_Settings == 0 || activeMenuItem_Settings > 2) {
                activeBPM = defaultBPM;
                difficulty = "NORMAL";
                durationMulti = 1;
                activeMenuItem_Settings = 0;
            }
            
            Keys[GLFW_KEY_RIGHT] = Keys[GLFW_KEY_LEFT] = GL_FALSE;
        }
        
    }
    
    //----- SELECT SOUNDFONT -----//
    if(activeMenuItem_SongSettings == 1){
        neonFont->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        neonFont->renderText(soundfonts[willBeSoundfont], screenWidth -335 - neonFont->getSize(soundfonts[willBeSoundfont])/2, 180);
        neonFont->setColor(neonColor);
        
        // Render arrows
        arrow->reset();
        arrow->translate(screenWidth -550, 195, 0);
        arrow->rotate(90.0f);
        arrow->scale(1.0 + glm::sin(frameTime * 2.5f)*0.05f);
        arrow->render();
        arrow->reset();
        arrow->translate(screenWidth -120, 195, 0);
        arrow->rotate(-90.0f);
        arrow->scale(1.0 + glm::sin(frameTime * 2.5f)*0.05f);
        arrow->render();
    
        if (Keys[GLFW_KEY_RIGHT]){
            if(willBeSoundfont < soundfonts.size()-1){
                willBeSoundfont++;
            }
            else{
                willBeSoundfont = 0;
            }
            
            Keys[GLFW_KEY_RIGHT] = GL_FALSE;
        }
        if (Keys[GLFW_KEY_LEFT]){
            if(willBeSoundfont > 0){
                willBeSoundfont--;
            }
            else{
                willBeSoundfont = (int)soundfonts.size() - 1;
            }
            
            Keys[GLFW_KEY_LEFT] = GL_FALSE;
        }
        
    }
    
    //----- SELECT HANDS -----//
    if (activeMenuItem_SongSettings == 2)
    {
        
        if (Keys[GLFW_KEY_RIGHT] || Keys[GLFW_KEY_LEFT]){
            
            if (Keys[GLFW_KEY_RIGHT]) activeMenuItem_Settings++;
            if (Keys[GLFW_KEY_LEFT]) activeMenuItem_Settings--;

            if (activeMenuItem_Settings == 1){
                hands = "RIGHT";
            }
            else if (activeMenuItem_Settings == 2 || activeMenuItem_Settings < 0){
                hands = "LEFT";
                activeMenuItem_Settings = 2;
            }
            else if (activeMenuItem_Settings == 0 || activeMenuItem_Settings > 2) {
                hands = "BOTH";
                activeMenuItem_Settings = 0;
            }

            Keys[GLFW_KEY_RIGHT] = Keys[GLFW_KEY_LEFT] = GL_FALSE;
        }
        
        neonFont->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        if(hands == "BOTH")
            neonFont->renderText(hands + " hands", screenWidth -335 -neonFont->getSize(hands + " hands")/2, 130);
        else
            neonFont->renderText(hands + " hand", screenWidth -335 -neonFont->getSize(hands + " hand")/2, 130);
        neonFont->setColor(neonColor);
        
        // Render arrows
        arrow->reset();
        arrow->translate(screenWidth -550, 145, 0);
        arrow->rotate(90.0f);
        arrow->scale(1.0 + glm::sin(frameTime * 2.5f)*0.05f);
        arrow->render();
        arrow->reset();
        arrow->translate(screenWidth -120, 145, 0);
        arrow->rotate(-90.0f);
        arrow->scale(1.0 + glm::sin(frameTime * 2.5f)*0.05f);
        arrow->render();
    }
    
    // Draw song information
    neonFont->setColor(neonColor);
    neonFont->setScale(1.0f);
    neonFont->renderText(songs[activeMenuItem_SongSelect].name, screenWidth -335 -neonFont->getSize(songs[activeMenuItem_SongSelect].name)/2, 430);
    
    neonFont->setScale(0.8f);
    neonFont->renderText(songs[activeMenuItem_SongSelect].difficulty, screenWidth -560, 360);
    std::string duration = activeMenuItem_SongSelectDuration + " min";
    neonFont->renderText(duration, screenWidth -100 -neonFont->getSize(duration), 360);

    neonFont->renderText("Settings", screenWidth -335 -neonFont->getSize("Settings")/2, 290);
    neonFont->renderText("____________", screenWidth -335 -neonFont->getSize("____________")/2, 280);
    
    neonFont->renderText(difficulty + " speed", screenWidth -335 -neonFont->getSize(difficulty + " speed")/2, 230);
    neonFont->renderText(soundfonts[willBeSoundfont], screenWidth -335 - neonFont->getSize(soundfonts[willBeSoundfont])/2, 180);
    if(hands == "BOTH")
        neonFont->renderText(hands + " hands", screenWidth -335 -neonFont->getSize(hands + " hands")/2, 130);
    else
        neonFont->renderText(hands + " hand", screenWidth -335 -neonFont->getSize(hands + " hand")/2, 130);
    neonFont->setScale(0.6f + glm::sin(frameTime)*0.01f);
    neonFont->setColor(neonColor * 0.8f);
    neonFont->renderText("Press Select to Play", screenWidth -335 -neonFont->getSize("Press Select to Play")/2, 23);
    
    // Draw list header
    standardFont->setScale(1.0f);
    standardFont->setColor(glm::vec3(0.92f, 0.1f, 0.42f) * 0.5f);
    standardFont->renderText("SONGS", 20, screenHeight - screenHeight/10);
    
    // Draw song list
    standardFont->setColor(pulse * 0.5f);
    int listLocation = activeMenuItem_SongSelect/8;
    for(int i = 0; i < songs.size(); i++){
        // Highlight active element in list
        if(i >= listLocation*8 && i  < (listLocation+1)*8){
            if(i==activeMenuItem_SongSelect){
				standardFont->setColor(glm::vec3(pulse));
                standardFont->renderText(songs[activeMenuItem_SongSelect].name, 20, screenHeight - (i%8+3)*(screenHeight/11));
                standardFont->setColor(pulse * 0.5f);
            }
            standardFont->renderText(songs[i].name, 20, screenHeight - (i%8+3)*(screenHeight/11));
        }
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
    
    // Render game
    activeSong->updatePiano(playerInput);
    activeSong->render(frameTime);
    activeSong->updateNotes(matchingKeys, frameTime + barPosition, deltaTime);
    strikeBar->render();
    
    
    neonFont->setScale(0.4f);
    neonFont->setColor(glm::vec3(0.3f, 0.7f, 0.9f));
    neonFont->renderText(songs[activeMenuItem_SongSelect].name, screenWidth -120 -neonFont->getSize(songs[activeMenuItem_SongSelect].name)/2, screenHeight-30);
    
    displaySongPercent();
    
    // If song is over. Go to post game screen.
    if (activeTrack->note(activeTrack->size() - 1)->end < frameTime - 5){
        State = POST_GAME;
    }
    
    // Update the current notes array. The notes in the track that should currently be played.
    activeTrack->updateCurrentNotes(currentNotes, frameTime + barPosition);
    // Check if the player input matches with current notes. Update matchingKeys.
    for(int i = 0; i < 128; i++){
        if(playerInput[i] && currentNotes[i])
            matchingKeys[i] = true;
        else
            matchingKeys[i] = false;
    }
    
    // Update score
    score.scoreNotes(activeTrack, currentNotes, playerInput, frameTime + barPosition, deltaTime);
    
    // Render score and multiplier
    standardFont->setScale(0.5f);
    standardFont->setColor(glm::vec3(0.3f, 0.7f, 0.9f));
    standardFont->renderText("SCORE", 20, screenHeight - 40);
    standardFont->renderText(std::to_string(score.getScore()), 20, screenHeight - 70);
    standardFont->setColor(glm::vec3(0.6f, 0.4f, 0.8f));
    standardFont->renderText("MULTIPLIER", 20, screenHeight - 110);
    
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
    // Draw multiplier
    standardFont->setColor(multiColor);
    standardFont->renderText(std::to_string(score.getMultiplier()), 20, screenHeight - 140);
}

//-------------------------------------------//
//--------------- POST GAME -----------------//
//-------------------------------------------//
void Game:: renderPostGame(){
    activeSoundfont = 5;
    // If player presses enter, reset and go back to song select.
    if (Keys[GLFW_KEY_ENTER]){
        leaderboardHandler();
        State = SONG_SELECT;
        activeSoundfont = 0;
        difficulty = "NORMAL";
        hands = "BOTH";
        durationMulti = 1;
        glfwSetTime(0);
        activeMenuItem_SongSelect = 0;
        score.reset();
		playerName.clear();
		playerName = "           ";
		letterInPlayerName = 0;
		currentLetter = ' ';
        
        Keys[GLFW_KEY_ENTER] = GL_FALSE;
    }
    // If player presses X, go to the leaderboard of the song.
    if (Keys[GLFW_KEY_X]){
        leaderboardHandler();
        State = LEADERBOARD;
        glfwSetTime(0);
        activeMenuItem_SongSelect = 0;
        score.reset();
        
        Keys[GLFW_KEY_X] = GL_FALSE;
    }
    
    // Render text with statistics for the played song.
    standardFont->setColor(glm::vec3(0.92f, 0.1f, 0.42f));
    standardFont->setScale(1.0f);
    standardFont->renderText("Statistics", 20, screenHeight - screenHeight/10);
    standardFont->setColor(glm::vec3(0.6f, 0.4f, 0.8f));
    standardFont->renderText("Score: " + std:: to_string(score.getScore()), 20, screenHeight - 300);
	standardFont->renderText("Notes Hit: " + std::to_string(notesHit()) +"%", 20, screenHeight - 500);

	// If player presses up, currentLetter should be the previous letter in the alphabet.
	if (Keys[GLFW_KEY_UP]) {
		if (currentLetter == ' ')
		{
			currentLetter = 'Z';
			playerName[letterInPlayerName] = currentLetter;
		}
		else if (currentLetter == 'A')
		{
			currentLetter = ' ';
			playerName[letterInPlayerName] = currentLetter;
		}
		else if (currentLetter > 'A')
		{
			currentLetter--;
			playerName[letterInPlayerName] = currentLetter;
		}
        else{
            currentLetter = 'Z';
        }
		Keys[GLFW_KEY_UP] = GL_FALSE;
	}
    // If player presses down, currentLetter should be the next letter in the alphabet.
	else if (Keys[GLFW_KEY_DOWN]) {
		if (currentLetter == ' ')
		{
			currentLetter = 'A';
			playerName[letterInPlayerName] = currentLetter;
		}
		else if (currentLetter == 'Z')
		{
			currentLetter = ' ';
			playerName[letterInPlayerName] = currentLetter;
		}
		else if (currentLetter < 'Z')
		{
			currentLetter++;
			playerName[letterInPlayerName] = currentLetter;
		}
        else{
            currentLetter = 'A';
        }
		Keys[GLFW_KEY_DOWN] = GL_FALSE;
	}

	if (Keys[GLFW_KEY_RIGHT])
	{

		if (letterInPlayerName < 10)
		{
			playerName[letterInPlayerName] = currentLetter;
			letterInPlayerName++;
			currentLetter = playerName[letterInPlayerName];
		}
		Keys[GLFW_KEY_RIGHT] = GL_FALSE;
	}


	if (Keys[GLFW_KEY_LEFT])
	{
		if (letterInPlayerName > 0)
		{
			playerName[letterInPlayerName] = currentLetter;
			letterInPlayerName--;
			currentLetter = playerName[letterInPlayerName];
		}
		Keys[GLFW_KEY_LEFT] = GL_FALSE;
	}
		
	
	standardFont->renderText("Name: ", 20, screenHeight - 700);

	std::string temp;
	for (int i = 0; i < 11; i++)
	{
		temp.append(1, playerName[i]);
		standardFont->renderText(temp, 200+i*40, screenHeight - 700);
		temp.clear();
		if(letterInPlayerName == i)
		{
            standardFont->setScale(1.5f);
			standardFont->renderText("_", 202 + i * 40, screenHeight - 710);
            standardFont->setScale(1.0f);
		}
	}
	
}


void Game:: renderLeaderboard(){
    activeSoundfont = 5;
    if(Keys[GLFW_KEY_DOWN]){
        
        if(activeMenuItem_Leaderboard < scoreVector.size()-1) activeMenuItem_Leaderboard++;
        Keys[GLFW_KEY_DOWN] = GL_FALSE;
    }
    if(Keys[GLFW_KEY_UP]){
        
        if(activeMenuItem_Leaderboard > 0) activeMenuItem_Leaderboard--;
        Keys[GLFW_KEY_UP] = GL_FALSE;
    }
    
   int listLocation = (int)activeMenuItem_Leaderboard/8;
    for(int i = 0; i < scoreVector.size(); i++){
        if(i >= listLocation*8 && i < (listLocation+1)*8){
            if(i == activeMenuItem_Leaderboard){
                standardFont->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
                standardFont->renderText(std::to_string(i +1) + ". " + scoreVector[i].second + ": " + std::to_string(scoreVector[i].first), 20, screenHeight - 100 - (i%8)*100);
                standardFont->setColor(glm::vec3(0.827f + 0.015f, 0.023f + 0.517f, 1.0f));
            }
            standardFont->renderText(std::to_string(i +1) + ". " + scoreVector[i].second + ": " + std::to_string(scoreVector[i].first), 20,  screenHeight - 100 - (i%8)*100 );
        }
    }
}




//-------------------------------------------//
//------------- OTHER FUNCTIONS -------------//
//-------------------------------------------//


void Game:: updateScoreVector(){
    std::fstream readerFile;
    readerFile.open("Leaderboards/" + songs[activeMenuItem_SongSelect].name + ".txt", std::ios::in);
    std::string line;
    double fileScore;
    scoreVector.clear();
    scoreVector.resize(30);
    
    for(int i = 0; i < scoreVector.size(); i++){
        readerFile >> line >> fileScore;
        scoreVector[i] = std::make_pair(fileScore, line);
        std::getline(readerFile, line);
    }
    
    std::sort(scoreVector.rbegin(),scoreVector.rend());

}

void Game:: leaderboardHandler(){
    std::fstream file;
    std::string fileName;

    if(score.getScore() > scoreVector[scoreVector.size()-1].first){
        file.open("Leaderboards/" + songs[activeMenuItem_SongSelect].name + ".txt", std::ios::out);
        for(int i = 0; i < scoreVector.size()-1; i++){
            file << scoreVector[i].second << " " << scoreVector[i].first << std::endl;
        }
        
    file << playerName << " " << score.getScore();
    
    }
}

void Game::displaySongPercent() {

    GLfloat percent = 0;
    if (frameTime > 2.5) {
        
        double time = activeTrack->note(activeTrack->size() - 1)->end;
        percent = 100 * (frameTime - 2.5) / time;
    }
    if (percent >= 100)
        percent = 100;
    
    // Background inside the bar
    progressBar->reset();
    progressBar->setColor(glm::vec3(0.03f));
    progressBar->translate(screenWidth -120, screenHeight-50, 0.0f);
    progressBar->render();
    
    // Fully transparent block to hide progressbar outside the border
    progressBar->reset();
    progressBar->setColor(glm::vec3(1.0f), 0.0f);
    progressBar->translate(screenWidth -320, screenHeight-50, 0.0003f);
    progressBar->render();
    
    // Progress bar
    progressBar->reset();
    progressBar->setColor(glm::vec3(0.0f, 0.4f, 0.0f));
    progressBar->translate(screenWidth -320 + percent * 2.0f, screenHeight-50, 0.0001f);
    progressBar->render();
    
    // Border
    progressBorder->reset();
    progressBorder->setColor(glm::vec3(0.1f, 0.23f, 0.3f));
    progressBorder->translate(screenWidth -120, screenHeight-50, 0.0002f);
    progressBorder->render();
}

int Game:: returnSoundfont(){
    return activeSoundfont;
}

int Game::notesHit()
{
    int total = activeTrack->size();
    int hit = activeTrack->countTriggeredNotes();
    
    double notesHitPercent = ((double)hit / (double)total)*100;
    
    return notesHitPercent;
}

void Game::renderBackground(){
    GLfloat sinTime = glm::sin(frameTime);
    GLfloat sinTimeSlow = glm::sin(frameTime*0.2f);
    GLfloat cosTimeSlow = glm::cos(frameTime*0.2f);
    
    sunset->render();
    
    // Right back clouds
    clouds1->reset();
    clouds1->translate(sinTimeSlow*0.2f, sinTime*sinTime*0.02f, 0.0f);
    clouds1->render();
    
    // Left back clouds
    clouds3->reset();
    clouds3->translate(-9.5f + sinTimeSlow*0.07f, 1.5f + cosTimeSlow*0.1f, -0.4f);
    clouds3->render();
    
    // Left mountain
    mountain2->render();
    
    birds->reset();
    birds->translate(-cosTimeSlow*7.0f, -sinTime*0.5f, 0.0f);
    birds->render();
    
    // Right mountain
    mountain3->render();
    
    // Left front clouds
    clouds2->reset();
    clouds2->translate(cosTimeSlow*0.4f, sinTime*sinTime*0.02f, 0.0f);
    clouds2->render();
    
    // Right front clouds
    clouds3->reset();
    clouds3->translate(cosTimeSlow*0.1f, sinTimeSlow*0.2f, 0.0f);
    clouds3->render();
    
    // Front mountain
    mountain1->render();
}
