#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

class Window
{
public:
    Window();
    Window(GLint windowWidth, GLint windowHeightn);
    int init(const char* title);
    GLint getBufferWidth() { return bufferWidth; }
    GLint getBufferHeight() { return bufferHeight; }
    bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }
    bool* getKeys() { return keys; }
    GLfloat getXChange();
    GLfloat getYChange();
    void swapBuffers() { glfwSwapBuffers(mainWindow); }
    ~Window();

private:
    GLFWwindow* mainWindow;
    GLint width, height;
    GLint bufferWidth, bufferHeight;
    bool keys[1024];
    GLfloat lastX, lastY, xChange, yChange;
    bool mouseFirstMoved;
    void createCallbacks();
    static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
    static void handleMouse(GLFWwindow* window, double xPos, double yPos);
};
#endif