#include "Game.hpp"

// Constructor. Should be declared globally in main. Can not contain GL-related code before the window is initialized.
Game::Game(){
    State = MAIN_MENU;
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
    
    // Default active element in menus
    activeElement = 0;
    
    songs.reserve(6);
    // Add songs to the song list. Name, filepath, main track, BPM.
    songs.push_back( songItem{ "Twinkle", "MusicLibrary/twinkle.mid", 1, 100 } );
    songs.push_back( songItem{ "Piano Man", "MusicLibrary/pianoman.mid", 1, 100 } );
    songs.push_back( songItem{ "Crab Rave", "MusicLibrary/crab_rave.mid", 0, 125 } );
    songs.push_back( songItem{ "Mountain King", "MusicLibrary/mountainking.mid", 1, 100 } );
    songs.push_back( songItem{ "Levels", "MusicLibrary/levels.mid", 0, 128 } );
    songs.push_back( songItem{ "Impromptu", "MusicLibrary/impromptu.mid", 0, 168 } );
    
    // Load shaders
    // Text shader is a shared shader used in the class Font.
    textShader = GraphicsTools::loadShaders( "Graphics/Shaders/TextVertexShader.vertexshader", "Graphics/Shaders/TextFragmentShader.fragmentshader" );
    // Color shader is mainly used in the class Song for rendering the notes.
    colorShader = GraphicsTools::loadShaders( "Graphics/Shaders/ColorVertexShader.vertexshader", "Graphics/Shaders/ColorFragmentShader.fragmentshader" );
    // Texture shader is used for all textured objects. The background is rendered with this shader.
    textureShader = GraphicsTools::loadShaders( "Graphics/Shaders/TextureVertexShader.vertexshader", "Graphics/Shaders/TextureFragmentShader.fragmentshader" );
    
    // Initialize the standard font for the game
    standardFont = new Font("Graphics/Fonts/Orbitron-Black.ttf", textShader, screenWidth, screenHeight);
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
    if(State == SETTINGS){
        renderSettings();
    }
    
    
    // Draw Title
    standardFont->setScale(0.7f);
    standardFont->setColor(glm::vec3(0.6f, 0.0f, 0.0f));
    standardFont->renderText("KEY SLAYER", screenWidth - 290, screenHeight - 60);

}



void Game::renderSong(){
    // Check player Input
    if(Keys[GLFW_KEY_ENTER]){
        // Switch to song state
        State = SONG_SELECT;
        
        Keys[GLFW_KEY_ENTER] = GL_FALSE;
    }
    
    activeSong->updateNotes();
    activeSong->renderBackground();
    activeSong->renderNotes();
    activeSong->renderPiano();
    activeSong->updatePiano(playerInput);
}



// Render function for the song menu
void Game::renderSongMenu(){
    // Check player Input
    if(Keys[GLFW_KEY_ENTER]){
        // Switch to song state
        State = SONG_ACTIVE;
        
        // Load new selected song
        delete activeTrack;
        delete activeSong;
        activeTrack = new MidiTrack(songs[activeElement].filepath, songs[activeElement].track, songs[activeElement].bpm);
        activeSong = new Song(*activeTrack, colorShader, textureShader);
        // Reset time
        glfwSetTime(0);
        
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
    else if(Keys[GLFW_KEY_RIGHT]) {
        // Go to leaderboard
        State = SETTINGS;
        // Reset active element. Next menu should start at element 0.
        activeElement = 0;
        Keys[GLFW_KEY_RIGHT] = GL_FALSE;
    }
    
    // Draw list header
    standardFont->setScale(1.0f);
    standardFont->setColor(glm::vec3(0.3f, 0.7f, 0.9f));
    standardFont->renderText("SONGS", 20, screenHeight - screenHeight/10);
    standardFont->setColor(glm::vec3(0.3f, 0.3f, 0.3f));
    standardFont->renderText("SETTINGS", 270, screenHeight - screenHeight/10);
    standardFont->setColor(glm::vec3(1.0f, 0.0f, 0.0f));
    
    // Draw song list
    for(int i = 0; i < songs.size(); i++){
        // Highlight active element in list
        if(i == activeElement){
            standardFont->setColor(glm::vec3(0.0f, 1.0f, 0.0f));
            standardFont->renderText(songs[i].name, 20, screenHeight - (i+2)*(screenHeight/10));
            standardFont->setColor(glm::vec3(1.0f, 0.0f, 0.0f));
        }
        // Draw list non-active list elements
        standardFont->renderText(songs[i].name, 20, screenHeight - (i+2)*(screenHeight/10));
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



void Game::renderSettings(){
    if(Keys[GLFW_KEY_LEFT]) {
        // Go to leaderboard
        State = SONG_SELECT;
        // Reset active element. Next menu should start at element 0.
        activeElement = 0;
        Keys[GLFW_KEY_LEFT] = GL_FALSE;
    }
    
    standardFont->setScale(1.0f);
    standardFont->setColor(glm::vec3(0.3f, 0.3f, 0.3f));
    standardFont->renderText("SONGS", 20, screenHeight - screenHeight/10);
    standardFont->setColor(glm::vec3(0.3f, 0.7f, 0.9f));
    standardFont->renderText("SETTINGS", 270, screenHeight - screenHeight/10);
    standardFont->setColor(glm::vec3(1.0f, 0.0f, 0.0f));
}
