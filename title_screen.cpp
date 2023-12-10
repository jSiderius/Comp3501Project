#include "title_screen.h"

TitleScreen::TitleScreen() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        exit(EXIT_FAILURE);
    }

    // Set GLFW to create an OpenGL 3.3 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(800, 600, "Title Screen", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        std::cerr << "Failed to create GLFW window\n";
        exit(EXIT_FAILURE);
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        exit(EXIT_FAILURE);
    }

    // Set up OpenGL settings
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set up callback functions
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
}

void TitleScreen::run() {
    // Main rendering loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Render the title screen here
        renderText("Your Game Title", 300.0f, 400.0f, 1.0f);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Clean up GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
}

void TitleScreen::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void TitleScreen::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void TitleScreen::renderText(const char* text, float x, float y, float scale) {
    // Set the text color (white in this case)
    glColor3f(1.0f, 1.0f, 1.0f);

    // Set the text position
    float xPos = x;
    float yPos = y;

    // Character size in pixels
    int charWidth = 8;
    int charHeight = 13;

    // Set the scale
    float scaledCharWidth = charWidth * scale;
    float scaledCharHeight = charHeight * scale;

    // Enable vertex arrays
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, nullptr);

    // Loop through each character of the text
    for (const char* c = text; *c != '\0'; ++c) {
        // Get the ASCII value of the character
        int asciiValue = static_cast<int>(*c);

        // Only render visible ASCII characters
        if (asciiValue >= 32 && asciiValue < 127) {
            // Calculate the vertices for the character
            float vertices[] = {
                xPos, yPos,
                xPos + scaledCharWidth, yPos,
                xPos, yPos + scaledCharHeight,
                xPos + scaledCharWidth, yPos + scaledCharHeight
            };

            // Draw the character using vertex arrays
            glLoadIdentity();
            glTranslatef(x, y, 0.0f);
            glScalef(scale, scale, 1.0f);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            // Move the position for the next character
            xPos += scaledCharWidth;
        }
    }

    // Disable vertex arrays
    glDisableClientState(GL_VERTEX_ARRAY);
}


