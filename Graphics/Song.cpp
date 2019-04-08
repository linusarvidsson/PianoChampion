#include "Song.hpp"

// Static constant background data
const GLfloat Song::WIDTH = 5.45f;
const GLfloat Song::HEIGHT = 5.0f;

const GLfloat Song::backgroundVertices[] = {
    -WIDTH, -HEIGHT, -0.01f,
    WIDTH, -HEIGHT, -0.01f,
    -WIDTH, HEIGHT, -0.01f,
    WIDTH, HEIGHT, -0.01f
};

const GLuint Song::backgroundIndices[] = {
    0,1,2,2,1,3
};

const GLfloat Song::backgroundUV[] = {
    0.0f, 1.0f,   // top-left corner
    1.0f, 1.0f,  // top-right corner
    0.0f, 0.0f,  // lower-left corner
    1.0f, 0.0f,  // lower-right corner
    
};

// Destructor. Delete VAOs and VBOs
Song::~Song(){
    glDeleteBuffers(1, &noteVertexBuffer);
    glDeleteBuffers(1, &noteColorBuffer);
    glDeleteBuffers(1, &noteElementBuffer);
    glDeleteBuffers(1, &backgroundVertexBuffer);
    glDeleteBuffers(1, &backgroundUVBuffer);
    glDeleteBuffers(1, &backgroundElementBuffer);
    
    glDeleteVertexArrays(1, &noteVAO);
    glDeleteVertexArrays(1, &backgroundVAO);
}


Song::Song(MidiTrack& track, GLuint& colorShader, GLuint& textureShader){
    backgroundShader = &textureShader;
    noteShader = &colorShader;
    songTrack = &track;
    
    // Perspective camera:
    // Projection matrix: 45∞ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    view = glm::lookAt(
                       glm::vec3(0,0,9), // Camera position
                       glm::vec3(0,0,0),  // The point the camera looks at
                       glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                       );
    // Model matrix : an identity matrix (model will be at the origin)
    model = glm::mat4(1.0f);
    /*
    // Orthograpic camera:
    
    projection = glm::ortho(-8.0f,8.0f,-8.0f,8.0f, 0.0f, 100.0f); // In world coordinates
    view = glm::lookAt(
                       glm::vec3(0,0,10), // Camera position
                       glm::vec3(0,0,0),  // The point the camera looks at
                       glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                       );
    model = glm::mat4(1.0f);
    */
     
    initNotes();
    initBackground();
    initPiano();
}

void Song::initNotes(){
    //----- NOTE DATA -----//
    noteVertices.reserve(songTrack->size()*4);
    noteColors.reserve(songTrack->size()*4);
    noteIndices.reserve(songTrack->size()*6);
    
    for(int i = 0; i < songTrack->size(); i++){
        Note n_i = Note(songTrack->note(i)->keyNumber, (GLfloat)songTrack->note(i)->start, (GLfloat)songTrack->note(i)->end);
        
        // Vertex 1
        noteVertices.push_back( glm::vec3(n_i.left(), n_i.start(), 0.0f) );
        // Vertex 2
        noteVertices.push_back( glm::vec3(n_i.right(), n_i.start(), 0.0f) );
        // Vertex 3
        noteVertices.push_back( glm::vec3(n_i.left(), n_i.end() - 0.03, 0.0f) );
        // Vertex 4
        noteVertices.push_back( glm::vec3(n_i.right(), n_i.end() - 0.03, 0.0f) );
        
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
    
    
    //----- NOTE VAO -----//
    glGenVertexArrays(1, &noteVAO);
    glBindVertexArray(noteVAO);
    
    //----- NOTE VBOs -----//
    // Create vertex buffer
    glGenBuffers(1, &noteVertexBuffer);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, noteVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, noteVertices.size() * sizeof(glm::vec3), &noteVertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    // Create color buffer
    glEnableVertexAttribArray(1);
    glGenBuffers(1, &noteColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, noteColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, noteColors.size() * sizeof(glm::vec3), &noteColors.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    // Generate index buffer
    glGenBuffers(1, &noteElementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, noteElementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, noteIndices.size() * sizeof(GLuint), &noteIndices.front(), GL_STATIC_DRAW);
}

void Song::renderNotes(){
    glUseProgram(*noteShader);
    glBindVertexArray(noteVAO);
    
    // MVP for notes. Translates notes with time.
    glm::mat4 noteTranslation = projection * view * translate(glm::mat4(1.0f), glm::vec3(0.0f, -glfwGetTime(), 0.0f)) * model;
    
    // Send the transformation to the currently bound shader,
    glUniformMatrix4fv(glGetUniformLocation(*noteShader, "MVP"), 1, GL_FALSE, &noteTranslation[0][0]);
    
    glDrawElements(GL_TRIANGLES, (GLsizei)noteIndices.size(), GL_UNSIGNED_INT, NULL);
    
    glBindVertexArray(0);
}


void Song::updateNotes(){
    // Update Note Color
    for (int n = 0; n < songTrack->size(); n++){
        // Update color data if note is played.
        if(songTrack->note(n)->start < glfwGetTime() - 2.5f){
            for(int v = 0; v < 4; v++){
                noteColors[4*n + v] = glm::vec3(0.2f, 0.2f, 0.25f);
            }
        }
    }
    // Bind the new data to the buffers
    glBindBuffer(GL_ARRAY_BUFFER, noteColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, noteColors.size() * sizeof(glm::vec3), &noteColors.front(), GL_STATIC_DRAW);
}


void Song::initBackground(){
    //----- BACKGROUND DATA -----//
    backgroundTexture = GraphicsTools::loadTexture("Graphics/Images/pianoklaviatur.png");
    
    //----- BACKGROUND VAO -----//
    glGenVertexArrays(1, &backgroundVAO);
    glBindVertexArray(backgroundVAO);
    
    //----- BACKGROUND VBOs -----//
    glGenBuffers(1, &backgroundVertexBuffer);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, backgroundVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    // Create UV buffer
    glGenBuffers(1, &backgroundUVBuffer);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, backgroundUVBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundUV), backgroundUV, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    // Create index buffer
    glGenBuffers(1, &backgroundElementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backgroundElementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(backgroundIndices), backgroundIndices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backgroundElementBuffer);
    
    
    // Send MVP transformation to backgroundShader
    glm::mat4 MVP = projection * view * model;
    glUseProgram(*backgroundShader);
    glUniformMatrix4fv(glGetUniformLocation(*backgroundShader, "MVP"), 1, GL_FALSE, &MVP[0][0]);
    
    // Set "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(glGetUniformLocation(*backgroundShader, "myTextureSampler"), 0);
    
    glBindVertexArray(0);
}

void Song::renderBackground(){
    glUseProgram(*backgroundShader);
    glBindVertexArray(backgroundVAO);
    
    // Bind texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    
    // Draw the textured elements
    glDrawElements(GL_TRIANGLES, sizeof(backgroundIndices), GL_UNSIGNED_INT, NULL);
    
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}



void Song::initPiano(){
    int startKey = 0;
    int endKey = 127;
    int numKeys = endKey - startKey +1;
    pianoKeyVertices.reserve(numKeys*4);
    pianoKeyColors.reserve(numKeys*4);
    pianoKeyIndices.reserve(numKeys*6);
    
    int index = 0;
    for(int key = startKey; key <= endKey; key++){
        
        Note n_key = Note(key, 0.0f, 0.0f);
        n_key.pianoSettings();
        
        // Vertex 1
        pianoKeyVertices.push_back( glm::vec3(n_key.left(), n_key.start(), n_key.height()) );
        // Vertex 2
        pianoKeyVertices.push_back( glm::vec3(n_key.right(), n_key.start(), n_key.height()) );
        // Vertex 3
        pianoKeyVertices.push_back( glm::vec3(n_key.left(), n_key.end() - 0.03, n_key.height()) );
        // Vertex 4
        pianoKeyVertices.push_back( glm::vec3(n_key.right(), n_key.end() - 0.03, n_key.height()) );
        
        // Color
        if(n_key.isBlack()){
            for (int vertex = 0; vertex < 4; vertex++){
                pianoKeyColors.push_back( glm::vec3(0.0f, 0.0f, 0.0f) );
            }
        }
        else{
            for (int vertex = 0; vertex < 4; vertex++){
                pianoKeyColors.push_back( glm::vec3(1.0f, 1.0f, 1.0f) );
            }
        }
        
        // Indexing
        pianoKeyIndices.push_back( index*4 );
        pianoKeyIndices.push_back( index*4 +1 );
        pianoKeyIndices.push_back( index*4 +2 );
        pianoKeyIndices.push_back( index*4 +2 );
        pianoKeyIndices.push_back( index*4 +1 );
        pianoKeyIndices.push_back( index*4 +3 );
        index++;
    }
    
    //----- Piano VAO -----//
    glGenVertexArrays(1, &pianoVAO);
    glBindVertexArray(pianoVAO);
    
    //----- Piano VBOs -----//
    // Create vertex buffer
    glGenBuffers(1, &pianoVertexBuffer);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, pianoVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, pianoKeyVertices.size() * sizeof(glm::vec3), &pianoKeyVertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    // Create color buffer
    glEnableVertexAttribArray(1);
    glGenBuffers(1, &pianoColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, pianoColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, pianoKeyColors.size() * sizeof(glm::vec3), &pianoKeyColors.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    // Generate index buffer
    glGenBuffers(1, &pianoElementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pianoElementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, pianoKeyIndices.size() * sizeof(GLuint), &pianoKeyIndices.front(), GL_STATIC_DRAW);
}

void Song::renderPiano(){
    
    glUseProgram(*noteShader);
    glBindVertexArray(pianoVAO);
    
    // MVP for notes. Translates notes with time.
    glm::mat4 MVP = projection * view * model;
    
    // Send the transformation to the currently bound shader,
    glUniformMatrix4fv(glGetUniformLocation(*noteShader, "MVP"), 1, GL_FALSE, &MVP[0][0]);
    
    glDrawElements(GL_TRIANGLES, (GLsizei)pianoKeyIndices.size(), GL_UNSIGNED_INT, NULL);
    
    glBindVertexArray(0);
}

void Song::updatePiano(bool playerInput[]){
    
    // Update key colors
    for (int key = 0; key < 128; key++){
        if(playerInput[key]){
            for(int v = 0; v < 4; v++){
                if(black[key])
                    pianoKeyColors[4*key + v] = glm::vec3(0.3f, 0.3f, 0.3f);
                else
                    pianoKeyColors[4*key + v] = glm::vec3(0.6f, 0.6f, 0.6f);
            }
        }
        else{
            for(int v = 0; v < 4; v++){
                if(black[key])
                    pianoKeyColors[4*key + v] = glm::vec3(0.0f, 0.0f, 0.0f);
                else
                    pianoKeyColors[4*key + v] = glm::vec3(1.0f, 1.0f, 1.0f);
            }
        }
    }
    
    // Bind the new data to the buffers
    glBindBuffer(GL_ARRAY_BUFFER, pianoColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, pianoKeyColors.size() * sizeof(glm::vec3), &pianoKeyColors.front(), GL_STATIC_DRAW);
}