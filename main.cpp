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
#include "title_screen.h"

// Macro for printing exceptions
#define PrintException(exception_object)\
	std::cerr << exception_object.what() << std::endl

// Main function that builds and runs the game
int main(int argc, char** argv){

    srand(static_cast<unsigned int>(time(0)));

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


    

    return 0;
}
