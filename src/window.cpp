#include "window.hpp"

Window::Window()
{
	width = 800;
	height = 600;
    // camera fix 
    xChange = 0.0f;
    yChange = 0.0f;

    for(size_t i = 0; i < 1024; i++)
    {
        keys[i] = 0;
    }
};

Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
    // camera fix    
    xChange = 0.0f;
    yChange = 0.0f;
    
    for(size_t i = 0; i < 1024; i++)
    {
        keys[i] = 0;
    }
};

int Window::init()
{
    if (!glfwInit())
    {
        printf("Error Initialising GLFW");
        glfwTerminate();
        return 1;
    }

    // FIXME: ISSUES WITH VERSION
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // Versión mayor de OpenGL
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);  // Versión menor de OpenGL
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // Usar el perfil core de OpenGL

    mainWindow = glfwCreateWindow(width, height, "NBody simulation", NULL, NULL);
    if (!mainWindow)
    {
        printf("Error creating GLFW window!");
        glfwTerminate();
        return 1;
    }

    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    glfwMakeContextCurrent(mainWindow);

    createCallbacks();

    glEnable(GL_PROGRAM_POINT_SIZE);
    glewExperimental = GL_TRUE;

    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        printf("Error: %s", glewGetErrorString(error));
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, bufferWidth, bufferHeight);
    
    // TODO: CHANGE THIS
    glOrtho(-1, 1, -1, 1, -1, 1); // Ajustar la escala para visualizar todos los puntos
    glfwSetWindowUserPointer(mainWindow, this);

    return 0; // Agregar retorno cuando todo sea exitoso
};

void Window::createCallbacks()
{
    glfwSetKeyCallback(mainWindow, handleKeys);
    glfwSetCursorPosCallback(mainWindow, handleMouse);
};

GLfloat Window::getXChange()
{
    GLfloat theChange = xChange;
    xChange = 0.0f;
    return theChange;
};

GLfloat Window::getYChange()
{
    GLfloat theChange = yChange;
    yChange = 0.0f;
    return theChange;
};

void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
    Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if(key >= 0 && key < 1024)
    {
        if(action == GLFW_PRESS) 
        {
            theWindow->keys[key] = true;
            //printf("Pressed: %d\n", key);
        }
        else if (action == GLFW_RELEASE)
        {
            theWindow->keys[key] = false;
            //printf("Released: %d\n", key);
        }
    }

};

void Window::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
    Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(theWindow->mouseFirstMoved)
    {
        theWindow->lastX = xPos;
        theWindow->lastY = yPos;
        theWindow->mouseFirstMoved = false;
    }

    theWindow->xChange = xPos - theWindow->lastX;
    theWindow->yChange = theWindow->lastY - yPos;

    theWindow->lastX = xPos;
    theWindow->lastY = yPos;

    //printf("x: %.6f, y: %.6f\n", theWindow->xChange, theWindow->yChange);
};

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}
