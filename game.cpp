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


void Game::InitEventHandlers(void){

    // Set event callbacks
    glfwSetFramebufferSizeCallback(window_, ResizeCallback);

    // Set pointer to game object, so that callbacks can access it
    glfwSetWindowUserPointer(window_, (void *) this);
}


void Game::SetupResources(void){

    // Create geometry of the objects
    resman_.CreateSphere("SphereMesh");
	resman_.CreateTorus("SimpleTorusMesh", 0.8, 0.35, 30, 30);
	resman_.CreateSeamlessTorus("SeamlessTorusMesh", 0.8, 0.35, 80, 80);
	resman_.CreateWall("FlatSurface");
	resman_.CreateCylinder("SimpleCylinderMesh", 2.0, 0.4, 30, 30);
    resman_.CreateTerrain("TerrainMesh", 200.0, 200.0, 360, 360);
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

    // Load texture to be used in normal mapping
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/normal_map2.png");
    resman_.LoadResource(Texture, "NormalMap", filename.c_str());

}


void Game::SetupScene(void){

    // Set background color for the scene
    scene_.SetBackgroundColor(viewport_background_color_g);

    // Create an object for showing the texture
	// instance contains identifier, geometry, shader, and texture
	
	game::SceneNode *mytorus1 = CreateInstance("MyTorus1", "SquareMesh", "Lighting", "RockyTexture"); 
    game::SceneNode *mytorus2 = CreateInstance("MyTorus2", "SquareMesh", "Checkers", "RockyTexture");    
    game::SceneNode *mytorus3 = CreateInstance("MyTorus3", "SquareMesh", "Combined", "Blocks");     
    // game::SceneNode *sphere = CreateInstance("MySphere", "SphereMesh", "Sun", "Blocks");
    
    game::SceneNode *floor = CreateInstance("Floor", "TerrainMesh", "TextureShader", "RockyTexture"); 
    game::SceneNode *skybox = CreateInstance("SkyBox", "SquareMesh", "TextureShader", "StaryTexture");

    CreatePlayer("Player", "PlayerMesh", "TextureShader", "MetalTexture");  

	mytorus1->Translate(glm::vec3(3.0, 0.5, 0));
    mytorus1->Scale(glm::vec3(1.0, 1.0, 1.0));

    mytorus2->Translate(glm::vec3(0.0, 0.0, 0.0));
    mytorus2->Scale(glm::vec3(0.5, 0.5, 0.5)); 

    mytorus3->Translate(glm::vec3(-1.5, 0, 0));
    mytorus3->Scale(glm::vec3(0.5, 0.5, 0.5));

    skybox->Scale(glm::vec3(400.0, 400.0, 400.0));

    // sphere->Scale(glm::vec3(0.5, 0.5, 0.5));

    //floor->Rotate(glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    floor->Translate(glm::vec3(-400, 0, 400));
    floor->Scale(glm::vec3(10.0, 10.0, 10.0));

	//game::SceneNode *mytorus = CreateInstance("MyTorus1", "SeamlessTorusMesh", "Lighting", "RockyTexture");
	//game::SceneNode *wall = CreateInstance("Canvas", "FlatSurface", "Procedural", "RockyTexture"); // must supply a texture, even if not used
		
}


void Game::MainLoop(void){
	float bleh = 0;
    
    SceneNode* floor = scene_.GetNode("Floor");

    std::vector<std::vector<float>> height_map = ResourceManager::ReadHeightMap(material_directory_g+"\\height_map.txt");
    std::vector<std::vector<bool>> impassable_map = CreateImpassableTerrainMap(height_map);
    player_->SetFloorPos(floor->GetPosition());
    player_->SetFloorScale(floor->GetScale());
    player_->SetImpassableMap(impassable_map);
    
    // Loop while the user did not close the window
    while (!glfwWindowShouldClose(window_)){
        // Animate the scene
        if (animating_){
            static double last_time = 0;
            double current_time = glfwGetTime();
            if ((current_time - last_time) > 0.01)
			{
                scene_.Update();
                Controls();

                // Animate the scene
                SceneNode *node1 = scene_.GetNode("MyTorus1");
                SceneNode *node2 = scene_.GetNode("MyTorus2");
                SceneNode *node3 = scene_.GetNode("MyTorus3");
                SceneNode *node4 = scene_.GetNode("MySphere");
                SceneNode *skybox = scene_.GetNode("SkyBox");
                
                //SceneNode *node = scene_.GetNode("Canvas");

				glm::quat rotation = glm::angleAxis(0.95f*glm::pi<float>()/180.0f, glm::vec3(0.0, 1.0, 0.0));
                node1->Rotate(rotation);
                node2->Rotate(-rotation);
                node3->Rotate(rotation);

                glm::quat orientationMatrix = player_->GetOrientation();
                glm::vec3 offsetInPlayerSpace = glm::vec3(0.2, 1.5, 15.0);
                glm::vec3 offsetInWorldSpace = glm::vec3(orientationMatrix * glm::vec4(offsetInPlayerSpace, 0.0f));

                player_->Update(height_map);
                camera_.SetPosition(player_->GetPosition() + offsetInWorldSpace);
                camera_.SetOrientation(player_->GetOrientation());

                skybox->SetPosition(player_->GetPosition());

                last_time = current_time;
            }
        }

        // Draw the scene
        scene_.Draw(&camera_);

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);

        // Update other events like input handling
        glfwPollEvents();
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

    Player *player = new Player(entity_name, geom, mat, tex, wheels, num_wheels, antennas, 2);
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
