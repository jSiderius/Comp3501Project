#ifndef GAME_H_
#define GAME_H_

#define GLEW_STATIC

#include <exception>
#include <string>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/Noise.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL/SOIL.h>

#include "scene_graph.h"
#include "resource_manager.h"
#include "camera.h"
#include "player.h"
#include "orb.h"

namespace game {

    // Exception type for the game
    class GameException : public std::exception
    {
    private:
        std::string message_;
    public:
        GameException(std::string message) : message_(message) {};
        virtual const char* what() const throw() { return message_.c_str(); };
        virtual ~GameException() throw() {};
    };

    // Game application
    class Game {

    public:
        // Constructor and destructor
        Game(void);
        ~Game();
        // Call Init() before calling any other method
        void Init(void);
        // Set up resources for the game
        void SetupResources(void);
        // Set up initial scene
        void SetupScene(void);
        // Run the game: keep the application active
        void MainLoop(void);

    private:
        // GLFW window
        GLFWwindow* window_;

        // Scene graph containing all nodes to render
        SceneGraph scene_;

        // Resources available to the game
        ResourceManager resman_;

        // Camera abstraction
        Camera camera_;

        // Player abstraction
        Player* player_;

        int num_orbs_init_ = 20;
        int num_orbs_ = 20;
        //Orb, Array Eventually
        Orb* orbs_[20];

        // Flag to turn animation on/off
        bool animating_;

        bool pre_game = true;
        bool post_game = false;

        //height map length and width
        float length_ = 500;
        float width_ = 500;

        GLuint programID3D;
        GLuint programID2D;
        GLuint programID2DTank;

        GLuint textureIDs[4];
        GLuint numberTextures[10];
        GLuint slash;

        // Methods to initialize the game
        void InitWindow(void);
        void InitView(void);
        void InitEventHandlers(void);
        void Init2D(void);

        void Render2DOverlay(void);
        void RenderTank(void);
        void RenderText(const char* text, float x, float y, float scale);
        void RenderGameMenu(int menu_index);
        void RenderPNG(float offset_x, GLuint texture);
        void Load2DTexture(std::string filename, GLuint* textureID);

        // Methods to handle events
        static void ResizeCallback(GLFWwindow* window, int width, int height);
        void Controls(void);

        void UpdateOrbs(void);

        // Asteroid field
        // Create instance of one asteroid
        Orb* CreateOrbInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name);

        SceneNode* CreateNonSceneInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texturename);

        // Create entire random asteroid field
        void CreateAsteroidField(int num_asteroids, SceneNode* floor_, std::vector<std::vector<float>> height_values);
        // Create the player
        void CreatePlayer(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name);

        // Create an instance of an object stored in the resource manager
        SceneNode* CreateInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name = std::string(""));

        std::vector<std::vector<bool>> CreateImpassableTerrainMap(std::vector<std::vector<float>> height_values);

    }; // class Game

} // namespace game

#endif // GAME_H_
