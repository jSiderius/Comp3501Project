#ifndef TITLE_SCREEN_H_
#define TITLE_SCREEN_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

class TitleScreen {
public:
    TitleScreen();
    void run();

private:
    GLFWwindow* window;

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void TitleScreen::renderText(const char* text, float x, float y, float scale);
};

#endif // TITLE_SCREEN_H_
