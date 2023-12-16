#include <iostream>
#include <time.h>
#include <sstream>

#include "game.h"

#include "path_config.h"

namespace game {

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Main window settings
const std::string window_title_g = "Texturing & Lighting Demo";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const bool window_full_screen_g = false;

// Viewport and camera settings
float camera_near_clip_distance_g = 0.1;
float camera_far_clip_distance_g = 1000.0;
float camera_fov_g = 20.0; // Field-of-view of camera
const glm::vec3 viewport_background_color_g(0.0, 0.0, 0.0);
glm::vec3 camera_position_g(0.5, 0.5, 10.0);
glm::vec3 camera_look_at_g(0.0, 0.0, 0.0);
glm::vec3 camera_up_g(0.0, 1.0, 0.0);

// Materials 
const std::string material_directory_g = MATERIAL_DIRECTORY;


Game::Game(void){

    // Don't do work in the constructor, leave it for the Init() function
}


void Game::Init(void){

    // Run all initialization steps
    InitWindow();
    InitView();
    InitEventHandlers();
    Init2D();
    // Set variables
    animating_ = true;
}

       
void Game::InitWindow(void){

    // Initialize the window management library (GLFW)
    if (!glfwInit()){
        throw(GameException(std::string("Could not initialize the GLFW library")));
    }

    // Create a window and its OpenGL context
    if (window_full_screen_g){
        window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), glfwGetPrimaryMonitor(), NULL);
    } else {
        window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), NULL, NULL);
    }
    if (!window_){
        glfwTerminate();
        throw(GameException(std::string("Could not create window")));
    }

    // Make the window's context the current one
    glfwMakeContextCurrent(window_);

    // Initialize the GLEW library to access OpenGL extensions
    // Need to do it after initializing an OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK){
        throw(GameException(std::string("Could not initialize the GLEW library: ")+std::string((const char *) glewGetErrorString(err))));
    }
}


void Game::InitView(void){


    // Set up z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set viewport
    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    glViewport(0, 0, width, height);

    // Set up camera
    // Set current view
    camera_.SetView(camera_position_g, camera_look_at_g, camera_up_g);
    // Set projection
    camera_.SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);
}

void Game::Init2D(void){
    const char* vertexShaderCode2D = R"(
        #version 330 core
        layout(location = 0) in vec2 vertexPosition;
        layout(location = 1) in vec2 textureCoord;
        out vec2 fragTextureCoord;
        uniform mat4 projection_mat;
        void main() {
            fragTextureCoord = textureCoord;
            gl_Position = projection_mat * vec4(vertexPosition, 0.0, 1.0);
        }
    )";

    const char* fragmentShaderCode2D = R"(
        #version 330 core

        in vec2 fragTextureCoord;
        out vec4 fragColor;
        uniform vec4 color;
        uniform sampler2D textureSampler;
        void main() {
            fragColor = texture2D(textureSampler, fragTextureCoord);
        }
    )";

    // Compile the vertex shader for 2D rendering
    GLuint vertexShaderID2D = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID2D, 1, &vertexShaderCode2D, nullptr);
    glCompileShader(vertexShaderID2D);

    // Compile the fragment shader for 2D rendering
    GLuint fragmentShaderID2D = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID2D, 1, &fragmentShaderCode2D, nullptr);
    glCompileShader(fragmentShaderID2D);

    // Link the 2D shader program
    programID2D = glCreateProgram();
    glAttachShader(programID2D, vertexShaderID2D);
    glAttachShader(programID2D, fragmentShaderID2D);
    glLinkProgram(programID2D);

    // Clean up shader resources
    glDeleteShader(vertexShaderID2D);
    glDeleteShader(fragmentShaderID2D);

    // Load the PNG image
    // GLuint textureID;
    int width, height;

    std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("\\Rover.png");
    unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
    if (!image) {
        std::cerr << "Failed to load PNG image\n";
        exit(EXIT_FAILURE);
    }

    // Generate OpenGL texture
    glGenTextures(1, &textureIDs[0]);
    glBindTexture(GL_TEXTURE_2D, textureIDs[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    filename = std::string(MATERIAL_DIRECTORY) + std::string("\\orb.png");
    image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
    if (!image) {
        std::cerr << "Failed to load PNG image\n";
        exit(EXIT_FAILURE);
    }

    // Generate OpenGL texture
    glGenTextures(1, &textureIDs[1]);
    glBindTexture(GL_TEXTURE_2D, textureIDs[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Game::InitEventHandlers(void){

    // Set event callbacks
    glfwSetFramebufferSizeCallback(window_, ResizeCallback);

    // Set pointer to game object, so that callbacks can access it
    glfwSetWindowUserPointer(window_, (void *) this);
}

void Game::Render2DOverlay(void){
    // Set up 2D rendering, using orthographic projection
    glDisable(GL_DEPTH_TEST);

    // Use the 2D shader program
    glUseProgram(programID2D);

    // Set uniform variables (e.g., color)
    GLuint colorUniform = glGetUniformLocation(programID2D, "color");
    glUniform4f(colorUniform, 1.0f, 0.0f, 0.0f, 1.0f);  // Red color
    
    glm::mat4 projectionMatrix = glm::ortho(0.0f, window_width_g * 1.0f, 0.0f, window_height_g * 1.0f, -1.0f, 1.0f);
    GLuint projectionMatrixUniform = glGetUniformLocation(programID2D, "projection_mat");
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Define vertices for a simple rectangle
    float rectangleVertices[] = {
        5.0f*window_width_g/6.0f, 5.0f*window_height_g/6.0f, 0.0f, 1.0f, // Top-left corner
        19.0f*window_width_g/20.0f, 5.0f*window_height_g/6.0f, 1.0f, 1.0f,  // Top-right corner
        19.0f*window_width_g/20.0f, 19.0f*window_height_g/20.0f, 1.0f, 0.0f,  // Bottom-right corner
        5.0f*window_width_g/6.0f, 19.0f*window_height_g/20.0f, 0.0f, 0.0f   // Bottom-left corner
    };

    // Generate a vertex buffer object (VBO) for the rectangle
    GLuint rectangleVBO;
    glGenBuffers(1, &rectangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, rectangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

    // Enable vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureIDs[1]);

    // Set the texture uniform in the shader
    glUniform1i(glGetUniformLocation(programID2D, "textureSampler"), 0);

    // Render the rectangle
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Disable vertex attributes and clean up resources
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDeleteBuffers(1, &rectangleVBO);

    // RenderPNG();

    // Re-enable depth testing for subsequent rendering
    glEnable(GL_DEPTH_TEST);
}

void Game::RenderPNG() {
    return;
}

void Game::RenderGameMenu(int menu_index = 1){
    // Set up 2D rendering, using orthographic projection
    glDisable(GL_DEPTH_TEST);

    // Use the 2D shader program
    glUseProgram(programID2D);

    // Set uniform variables (e.g., color)
    GLuint colorUniform = glGetUniformLocation(programID2D, "color");
    glUniform4f(colorUniform, 1.0f, 0.0f, 0.0f, 1.0f);  // Red color
    
    glm::mat4 projectionMatrix = glm::ortho(0.0f, window_width_g * 1.0f, 0.0f, window_height_g * 1.0f, -1.0f, 1.0f);
    GLuint projectionMatrixUniform = glGetUniformLocation(programID2D, "projection_mat");
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Define vertices for a simple rectangle
    float rectangleVertices[] = {
        0.0f, window_height_g, 0.0f, 0.0f, // Top-left corner
        window_width_g, window_height_g, 1.0f, 0.0f,  // Top-right corner
        window_width_g, 0.0f, 1.0f, 1.0f,  // Bottom-right corner
        0.0f, 0.0f, 0.0f, 1.0f   // Bottom-left corner
    };

    // Generate a vertex buffer object (VBO) for the rectangle
    GLuint rectangleVBO;
    glGenBuffers(1, &rectangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, rectangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

    // Enable vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureIDs[menu_index]);

    // Set the texture uniform in the shader
    glUniform1i(glGetUniformLocation(programID2D, "textureSampler"), 0);

    // Render the rectangle
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Disable vertex attributes and clean up resources
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDeleteBuffers(1, &rectangleVBO);

    // Re-enable depth testing for subsequent rendering
    glEnable(GL_DEPTH_TEST);

    // RenderText("Press Enter to continue", 400.0f, 300.0f, 1.0f);
}

void Game::RenderText(const char* text, float x, float y, float scale) {
    // Set up 2D rendering, using orthographic projection
    glDisable(GL_DEPTH_TEST);

    // Use the 2D shader program
    glUseProgram(programID2D);

    // Set up the text color
    GLuint colorUniform = glGetUniformLocation(programID2D, "color");
    glUniform4f(colorUniform, 1.0f, 1.0f, 1.0f, 1.0f);  // White color

    // Set up the orthographic projection matrix for text rendering
    glm::mat4 projectionMatrix = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    GLuint projectionMatrixUniform = glGetUniformLocation(programID2D, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Character size in the texture atlas
    int charWidth = 8;
    int charHeight = 13;

    // Texture atlas dimensions
    int atlasWidth = 128;  // Change this based on the width of your texture atlas

    // Calculate texture coordinates for each character in the text
    for (const char* c = text; *c != '\0'; ++c) {
        // Calculate the texture coordinates for the character
        float uStart = (*c % (atlasWidth / charWidth)) * (1.0f / (atlasWidth / charWidth));
        float vStart = (*c / (atlasWidth / charWidth)) * (1.0f / (atlasWidth / charWidth));
        float uEnd = uStart + (1.0f / (atlasWidth / charWidth));
        float vEnd = vStart + (1.0f / (atlasWidth / charHeight));

        // Define vertices for rendering the character using a textured quad
        float charVertices[] = {
            x, y, uStart, vEnd,  // Top-left
            x + charWidth * scale, y, uEnd, vEnd,  // Top-right
            x + charWidth * scale, y + charHeight * scale, uEnd, vStart,  // Bottom-right
            x, y + charHeight * scale, uStart, vStart  // Bottom-left
        };

        // Define vertices for a simple rectangle
        // float charVertices[] = {
        //     0.0f, y, 1.0f, 0.0f, // Top-left corner
        //     x, y, 0.0f, 0.0f,  // Top-right corner
        //     x, 0.0f, 0.0f, 1.0f,  // Bottom-right corner
        //     0.0f, 0.0f, 1.0f, 1.0f   // Bottom-left corner
        // };

        // Generate a vertex buffer object (VBO) for the character
        GLuint charVBO;
        glGenBuffers(1, &charVBO);
        glBindBuffer(GL_ARRAY_BUFFER, charVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(charVertices), charVertices, GL_STATIC_DRAW);

        // Enable vertex attributes
        // glEnableVertexAttribArray(0);  // Position
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
        
        // Enable vertex attributes
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureIDs[0]);

        // Set the texture uniform in the shader
        glUniform1i(glGetUniformLocation(programID2D, "textureSampler"), 0);

        // Render the character
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // Disable vertex attributes and clean up resources
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDeleteBuffers(1, &charVBO);

        // Move the position for the next character
        x += charWidth * scale;
    }

    glEnable(GL_DEPTH_TEST);
}


void Game::SetupResources(void){

    // Create geometry of the objects
    resman_.CreateSphere("SphereMesh");
	resman_.CreateTorus("SimpleTorusMesh", 0.8, 0.35, 30, 30);
	resman_.CreateSeamlessTorus("SeamlessTorusMesh", 0.8, 0.35, 80, 80);
	resman_.CreateWall("FlatSurface");
	resman_.CreateCylinder("SimpleCylinderMesh", 2.0, 0.4, 30, 30);
    resman_.CreateTerrain("TerrainMesh", length_, width_);
    resman_.CreateRectangle("PlayerMesh", 1.0, 0.5, 3.0);
    resman_.CreateSquare("SquareMesh");

    resman_.CreateCylinder("AntennaCylinderMesh", 1.0, 0.025, 30, 30);
    resman_.CreateSeamlessTorus("AntennaTorusMesh", 0.1, 0.05, 80, 80);

    //RESOURCE MANAGER ADDS TO THE FILENAME STRING 
    // Load shader for texture mapping
	std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/textured_material"); //SO /textured_material_fp.glsl, /textured_material_vp.glsl ... 
	resman_.LoadResource(Material, "TextureShader", filename.c_str());

	// shader for corona effect
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/corona");
	resman_.LoadResource(Material, "Procedural", filename.c_str());

    // Load material to be used for normal mapping
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/normal_map");
    resman_.LoadResource(Material, "NormalMapMaterial", filename.c_str());

	// shader for checkerboard effect
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/rectangle"); 
	resman_.LoadResource(Material, "Blocks", filename.c_str());

    //ADD THE CHECKERBOARD MATERIAL
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/procedural");
	resman_.LoadResource(Material, "Checkers", filename.c_str());

	// shader for 3-term lighting effect
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/lit");
	resman_.LoadResource(Material, "Lighting", filename.c_str());

    // SHADER FOR SUN OBJECT
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/sun");
	resman_.LoadResource(Material, "Sun", filename.c_str());

    // SHADER FOR COMBINED CHECKERBOARD & LIGHTING
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/combined");
	resman_.LoadResource(Material, "Combined", filename.c_str());

	// Load texture to be used on the object
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/mars.jpg");
	resman_.LoadResource(Texture, "RockyTexture", filename.c_str());

    // Load texture to be used on the object
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/robot.jpg");
	resman_.LoadResource(Texture, "RobotTexture", filename.c_str());

    // Load texture to be used on the object
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/stars.png");
	resman_.LoadResource(Texture, "StaryTexture", filename.c_str());

    // Load texture to be used on the object
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/tire.png");
	resman_.LoadResource(Texture, "TireTexture", filename.c_str());

	// Load texture to be used on the object
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/download.jpg");
	resman_.LoadResource(Texture, "WoodTexture", filename.c_str());

    // Load texture to be used on the object
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/metal.png");
	resman_.LoadResource(Texture, "MetalTexture", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/orb.png");
	resman_.LoadResource(Texture, "OrbTexture", filename.c_str());

    // Load texture to be used in normal mapping
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/normal_map2.png");
    resman_.LoadResource(Texture, "NormalMap", filename.c_str());

}


void Game::SetupScene(void){

    // Set background color for the scene
    scene_.SetBackgroundColor(viewport_background_color_g);

    // Create an object for showing the texture
	// instance contains identifier, geometry, shader, and texture
    game::SceneNode *floor = CreateInstance("Floor", "TerrainMesh", "TextureShader", "RockyTexture"); 
    game::SceneNode *skybox = CreateInstance("SkyBox", "SphereMesh", "TextureShader", "StaryTexture");

    for(int i = 0; i < num_orbs_; i++){
        std::stringstream ss;
        ss << i;
        std::string index = ss.str();
        std::string name = "OrbInstance" + index;

        orbs_[i] = CreateOrbInstance(name, "SphereMesh", "TextureShader", "OrbTexture");
        orbs_[i]->Translate(glm::vec3(i*4,i*4,i));
        orbs_[i]->Scale(glm::vec3(3.0, 3.0, 3.0));
        orbs_[i]->SetRadius(3.0f);
    }
    
    CreatePlayer("Player", "PlayerMesh", "TextureShader", "MetalTexture");  

    skybox->Scale(glm::vec3(600.0, 400.0, 600.0));

    floor->Translate(glm::vec3(-400, 0, 400));
    floor->Scale(glm::vec3(10.0, 10.0, 10.0));	
}


void Game::MainLoop(void){
	float bleh = 0;
    
    SceneNode* floor = scene_.GetNode("Floor");

    std::vector<std::vector<float>> height_map = ResourceManager::ReadHeightMap(material_directory_g+"\\height_map.txt");
    std::vector<std::vector<bool>> impassable_map = CreateImpassableTerrainMap(height_map);
    player_->SetFloorPos(floor->GetPosition());
    player_->SetFloorScale(floor->GetScale());
    player_->SetImpassableMap(impassable_map);

    for(int i =0; i < num_orbs_; i++){
        orbs_[i]->SetFloorPos(floor->GetPosition());
        orbs_[i]->SetFloorScale(floor->GetScale());
        orbs_[i]->SetImpassableMap(impassable_map);
        orbs_[i]->Update(height_map, length_ , width_);
    }
    
    // Loop while the user did not close the window
    while (!glfwWindowShouldClose(window_)){
        // Animate the scene
        if (animating_ && !pre_game){
            static double last_time = 0;
            double current_time = glfwGetTime();
            if ((current_time - last_time) > 0.01)
			{
                scene_.Update();
                Controls();

                SceneNode *skybox = scene_.GetNode("SkyBox");

                glm::quat orientationMatrix = player_->GetOrientation();
                glm::vec3 offsetInPlayerSpace = glm::vec3(0.2, 1.5, 15.0);
                glm::vec3 offsetInWorldSpace = glm::vec3(orientationMatrix * glm::vec4(offsetInPlayerSpace, 0.0f));

                player_->Update(height_map, length_, width_);
                camera_.SetPosition(player_->GetPosition() + offsetInWorldSpace);
                camera_.SetOrientation(player_->GetOrientation());

                skybox->SetPosition(player_->GetPosition());

                last_time = current_time;
            }
        }

        // Draw the scene
        scene_.Draw(&camera_);

        if(pre_game){ 
            RenderGameMenu(0); 
            Controls();
        }else if(post_game){
            RenderGameMenu(1);
        }else{
            Render2DOverlay();
            UpdateOrbs();
        }
            
        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);

        // Update other events like input handling
        glfwPollEvents();
    }
}

void Game::UpdateOrbs(void){

    if(num_orbs_ == 0){
        post_game = true;
    }

    for(int i = 0; i < num_orbs_; i++){
        if(orbs_[i]->Colliding(player_->GetPosition(), 2)){
            delete orbs_[i];
            orbs_[i] = orbs_[num_orbs_ - 1];
            num_orbs_--;
            break;
        }
        orbs_[i]->Update();
        orbs_[i]->Draw(&camera_);
        //add collision function
        //remove orb from array / delete and num_orbs_ --
        //maybe give orbs some minimal amount of movement
    }
}



void Game::ResizeCallback(GLFWwindow* window, int width, int height){

    // Set up viewport and camera projection based on new window size
    glViewport(0, 0, width, height);
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;
    game->camera_.SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);
}


Game::~Game(){
    
    glfwTerminate();
}


Orb* Game::CreateOrbInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name){

    // Get resources
    Resource *geom = resman_.GetResource(object_name);
    if (!geom){
        throw(GameException(std::string("Could not find resource \"")+object_name+std::string("\"")));
    }

    Resource *mat = resman_.GetResource(material_name);
    if (!mat){
        throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
    }

    Resource *tex = NULL;
    if (texture_name != ""){
        tex = resman_.GetResource(texture_name);
        if (!tex){
            throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
        }
    }

    // Create asteroid instance
    Orb *orb = new Orb(entity_name, geom, mat, tex);
    // scene_.AddNode(orb);
    return orb;
}

Asteroid *Game::CreateAsteroidInstance(std::string entity_name, std::string object_name, std::string material_name){

    // Get resources
    Resource *geom = resman_.GetResource(object_name);
    if (!geom){
        throw(GameException(std::string("Could not find resource \"")+object_name+std::string("\"")));
    }

    Resource *mat = resman_.GetResource(material_name);
    if (!mat){
        throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
    }

    // Create asteroid instance
    Asteroid *ast = new Asteroid(entity_name, geom, mat);
    scene_.AddNode(ast);
    return ast;
}


void Game::CreateAsteroidField(int num_asteroids){

    // Create a number of asteroid instances
    for (int i = 0; i < num_asteroids; i++){
        // Create instance name
        std::stringstream ss;
        ss << i;
        std::string index = ss.str();
        std::string name = "AsteroidInstance" + index;

        // Create asteroid instance
        Asteroid *ast = CreateAsteroidInstance(name, "SimpleSphereMesh", "ObjectMaterial");

        // Set attributes of asteroid: random position, orientation, and
        // angular momentum
        ast->SetPosition(glm::vec3(-300.0 + 600.0*((float) rand() / RAND_MAX), -300.0 + 600.0*((float) rand() / RAND_MAX), 600.0*((float) rand() / RAND_MAX)));
        ast->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>()*((float) rand() / RAND_MAX), glm::vec3(((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX)))));
        ast->SetAngM(glm::normalize(glm::angleAxis(0.05f*glm::pi<float>()*((float) rand() / RAND_MAX), glm::vec3(((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX)))));
    }
}


void Game::CreatePlayer(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name){
    // Get resources
    Resource *geom = resman_.GetResource(object_name);
    if (!geom){
        throw(GameException(std::string("Could not find resource \"")+object_name+std::string("\"")));
    }

    Resource *mat = resman_.GetResource(material_name);
    if (!mat){
        throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
    }

    Resource *tex = NULL;
    if (texture_name != ""){
        tex = resman_.GetResource(texture_name);
        if (!tex){
            throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
        }
    }

    const int num_wheels = 6;
    SceneNode* wheels[num_wheels];
    for(int i = 0; i < num_wheels; i++){
        std::stringstream ss;
        ss << "Wheel" << i;
        wheels[i] = CreateInstance(ss.str(), "SphereMesh", "TextureShader", "TireTexture");
        wheels[i]->Scale(glm::vec3(0.3, 0.3, 0.3));
    }

    SceneNode* antennas[2];
    antennas[0] = CreateInstance("Antenna1", "AntennaCylinderMesh", "TextureShader", "MetalTexture");
    antennas[1] = CreateInstance("Antenna2", "AntennaTorusMesh", "TextureShader", "MetalTexture");

    Player *player = new Player(entity_name, geom, mat, tex, wheels, num_wheels, antennas, 2, length_, width_);
    scene_.AddNode(player);

    player_ = player;

    player_->SetView(camera_position_g, camera_look_at_g, camera_up_g); 
}


SceneNode *Game::CreateInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name){

    Resource *geom = resman_.GetResource(object_name);
    if (!geom){
        throw(GameException(std::string("Could not find resource \"")+object_name+std::string("\"")));
    }

    Resource *mat = resman_.GetResource(material_name);
    if (!mat){
        throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
    }

    Resource *tex = NULL;
    if (texture_name != ""){
        tex = resman_.GetResource(texture_name);
        if (!tex){
            throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
        }
    }

    SceneNode *scn = scene_.CreateNode(entity_name, geom, mat, tex);
    return scn;
}


void Game::Controls(void)
{
    if (glfwGetKey(window_, GLFW_KEY_ENTER) == GLFW_PRESS){
        pre_game = false;
    }
    if(pre_game){ return; }
    
     // Get user data with a pointer to the game class
    void* ptr = glfwGetWindowUserPointer(window_);
    Game *game = (Game *) ptr;

    // Quit game if 'q' is pressed
    if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS){
        glfwSetWindowShouldClose(window_, true);
    }

    // Stop animation if space bar is pressed
    if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS){
        game->animating_ = (game->animating_ == true) ? false : true;
    }

    // View control
    float rot_factor(glm::pi<float>() / 360.0); // amount the ship turns per keypress
    float trans_factor = 5.0; // amount the ship steps forward per keypress
    
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS){
        game->player_->Yaw(rot_factor);
    }
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS){
        game->player_->Yaw(-rot_factor);
    }

    float speed_factor = 0.0025f;
    if(glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS){
        // player_->SetSpeed(player_->GetSpeed()+4.0f*speed_factor);
        game->player_->Translate(game->player_->GetForward()*trans_factor);
    }
    else if(glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS){
        // player_->SetSpeed(player_->GetSpeed()-3.0f*speed_factor);
        game->player_->Translate(game->player_->GetForward()*-trans_factor);
    }else{
        // player_->SetSpeed(player_->GetSpeed()*-trans_factor);
    }

    //DISABLED BECAUSE IT DOESNT MAKE SENSE IN THE GAME RIGHT NOW 
    // if (glfwGetKey(window_, GLFW_KEY_J) == GLFW_PRESS){
    //     game->player_->Translate(-game->player_->GetSide()*trans_factor);
    // }
    // if (glfwGetKey(window_, GLFW_KEY_L) == GLFW_PRESS){
    //     game->player_->Translate(game->player_->GetSide()*trans_factor);
    // }
    // if (glfwGetKey(window_, GLFW_KEY_I) == GLFW_PRESS){
    //     game->player_->Translate(game->player_->GetUp()*trans_factor);
    // }
    // if (glfwGetKey(window_, GLFW_KEY_K) == GLFW_PRESS){
    //     game->player_->Translate(-game->player_->GetUp()*trans_factor);
    // }
    if (glfwGetKey(window_, GLFW_KEY_UP) == GLFW_PRESS){
        game->player_->Pitch(rot_factor);
    }
    if (glfwGetKey(window_, GLFW_KEY_DOWN) == GLFW_PRESS){
        game->player_->Pitch(-rot_factor);
    }
    // if (glfwGetKey(window_, GLFW_KEY_LEFT) == GLFW_PRESS){
            // game->player_->Roll(rot_factor);
    //     
    // }
    // if (glfwGetKey(window_, GLFW_KEY_RIGHT) == GLFW_PRESS){
    //     
    // game->player_->Roll(-rot_factor);
    // }
}


std::vector<std::vector<bool>> Game::CreateImpassableTerrainMap(std::vector<std::vector<float>> height_values) {

    std::vector<std::vector<bool>> impassableMap;

    for (int i = 0; i < height_values.size(); i++) {
        std::vector<bool> row;

        for (int j = 0; j < height_values[i].size(); j++) {
            if (i == 0 || i == height_values.size() - 1 || j == 0 || j == height_values[i].size() - 1) {
                row.push_back(false);
            }
            else {

                float a = height_values[i][j];
                float b = height_values[i][j + 1];
                float c = height_values[i + 1][j];
                float d = height_values[i + 1][j + 1];

                int threshold = 15;

                if (abs(a - b) > threshold || abs(a - c) > threshold || abs(b - d) > threshold || abs(c - d) > threshold) {
                    row.push_back(false);
                }
                else {
                    row.push_back(true);
                }
            }
        }

        impassableMap.push_back(row);

    }

    return impassableMap;

}


} // namespace game
