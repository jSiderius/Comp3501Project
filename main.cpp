/*
 *
 * Demo covering lighting plus texturing including normal mapping
 *
 * Copyright (c) 2018 Oliver van Kaick <Oliver.vanKaick@carleton.ca>, David Mould <mould@scs.carleton.ca>
 *
 */


#include <iostream>
#include <exception>
#include "game.h"
<<<<<<< HEAD
#include "title_screen.h"
=======
>>>>>>> 497e5860fc00dce238d1a3e9764e57100c9f11c3

// Macro for printing exceptions
#define PrintException(exception_object)\
	std::cerr << exception_object.what() << std::endl

// Main function that builds and runs the game
<<<<<<< HEAD
int main(int argc, char** argv){

    // TitleScreen titleScreen;
    // titleScreen.run();
    // return 0;

=======
int main(void){
>>>>>>> 497e5860fc00dce238d1a3e9764e57100c9f11c3
    game::Game app; // Game application

    try {
        // Initialize game
        app.Init();

        // Set up the main resources and scene in the game
        app.SetupResources();
        
        app.SetupScene();
        // Run game
        app.MainLoop();
    }
    catch (std::exception &e){
        PrintException(e);
    }

<<<<<<< HEAD

    

=======
>>>>>>> 497e5860fc00dce238d1a3e9764e57100c9f11c3
    return 0;
}
