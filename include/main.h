#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#define GLEW_STATIC
#include "GL/glew.h" // Important - this header must come before glfw3 header
#include "GLFW/glfw3.h"
#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../include/ShaderProgram.h"
#include "../include/Texture2D.h"
#include "../include/Camera.h"
#include "../include/Mesh.h"

// Global Variables
const char *APP_TITLE = "Introduction to Modern OpenGL - Multiple Lights";
int gWindowWidth = 1024;
int gWindowHeight = 768;
GLFWwindow *gWindow = NULL;
bool gWireframe = false;
bool gFlashlightOn = true;
glm::vec4 gClearColor(0.0f, 0.0f, 0.0f, 1.0f);
static bool mac_moved = false;
double ball_dir_left_or_right = 0.0;
double ball_speed = 0.0;
bool release_ball = false;
bool respawn_scene = false;
double speed_factor = -0.2f;
glm::vec3 campos(0.000000, 60.879349, 80.000000);
FPSCamera fpsCamera(campos, glm::vec3(-0.0, -0.0, 0.0));
const double ZOOM_SENSITIVITY = -3.0;
const float MOVE_SPEED = 50.0; // units per second
const float MOUSE_SENSITIVITY = 0.1f;
float Blend[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    -3.0f, 3.0f, 0.0f, 0.0f,
    3.0f, -6.0f, 3.0f, 0.0f,
    -1.0f, 3.0f, -3.0f, 1.0f};
glm::mat4 blend_mat = glm::make_mat4(Blend);
std::vector<glm::vec3> dynamic_points;
double bezier_param = 0.0;
glm::vec3 initial_ball_position(-0.0f, 1.25f, 30.0f);

// Function prototypes
void glfw_onKey(GLFWwindow *window, int key, int scancode, int action, int mode);
void glfw_onFramebufferSize(GLFWwindow *window, int width, int height);
void glfw_onMouseScroll(GLFWwindow *window, double deltaX, double deltaY);
void update(double elapsedTime);
void showFPS(GLFWwindow *window);
bool initOpenGL();
void mac_patch(GLFWwindow *);
void setUpDirectionalAndPointLights(ShaderProgram);
void setUpSpotLight(ShaderProgram);
void drawFloor(Mesh mesh, ShaderProgram lightingShader, Texture2D texture, glm::vec3 position, glm::vec3 scale, glm::mat4 model);
void setupScene();
void renderFloor(glm::mat4 model, ShaderProgram lightingShader);
float clip(float n, float lower, float upper);
glm::vec3 get_bezier_points(double t);

//-----------------------------------------------------------------------------
// Initialize GLFW and OpenGL
//-----------------------------------------------------------------------------

const int numModels = 12;
Mesh mesh[numModels];
Texture2D texture[numModels];

// Scene Model positions
glm::vec3 modelPos[] = {

    glm::vec3(5.0f, 0.0f, 0.0f),   // floor
    glm::vec3(0.0f, 0.0f, 0.0f),   // pin
    glm::vec3(-1.0f, 0.0f, -1.0f), // pin
    glm::vec3(1.0f, 0.0f, -1.0f),  // pin
    glm::vec3(-2.0f, 0.0f, -2.0f), // pin
    glm::vec3(0.0f, 0.0f, -2.0f),  // pin
    glm::vec3(2.0f, 0.0f, -2.0f),  // pin
    glm::vec3(-3.0f, 0.0f, -3.0f), // pin
    glm::vec3(-1.0f, 0.0f, -3.0f), // pin
    glm::vec3(1.0f, 0.0f, -3.0f),  // pin
    glm::vec3(3.0f, 0.0f, -3.0f),  // pin
    initial_ball_position          // ball // origin of the ball

};

// Model scale
glm::vec3 modelScale[] = {

    glm::vec3(10.0f, 1.0f, 10.0f), // floor
    glm::vec3(0.2f, 0.2f, 0.2f),   // pin
    glm::vec3(0.2f, 0.2f, 0.2f),   // pin
    glm::vec3(0.2f, 0.2f, 0.2f),   // pin
    glm::vec3(0.2f, 0.2f, 0.2f),   //pin
    glm::vec3(0.2f, 0.2f, 0.2f),   // pin
    glm::vec3(0.2f, 0.2f, 0.2f),   // pin
    glm::vec3(0.2f, 0.2f, 0.2f),
    glm::vec3(0.2f, 0.2f, 0.2f),
    glm::vec3(0.2f, 0.2f, 0.2f),
    glm::vec3(0.2f, 0.2f, 0.2f),
    glm::vec3(0.1f, 0.1f, 0.1f),

};

void setupScene()
{
    mesh[0].loadOBJ("models/floor.obj"); // background
    mesh[1].loadOBJ("models/bowling_pin.obj");
    mesh[2].loadOBJ("models/bowling_pin.obj");
    mesh[3].loadOBJ("models/bowling_pin.obj");
    mesh[4].loadOBJ("models/bowling_pin.obj");
    mesh[5].loadOBJ("models/bowling_pin.obj");
    mesh[6].loadOBJ("models/bowling_pin.obj");
    mesh[7].loadOBJ("models/bowling_pin.obj");
    mesh[8].loadOBJ("models/bowling_pin.obj");
    mesh[9].loadOBJ("models/bowling_pin.obj");
    mesh[10].loadOBJ("models/bowling_pin.obj");
    mesh[11].loadOBJ("models/my_bowling_ball2.obj");

    texture[0].loadTexture("textures/woodbase.png", true);
    texture[1].loadTexture("textures/AMF.tga", true);
    texture[2].loadTexture("textures/AMF.tga", true);
    texture[3].loadTexture("textures/AMF.tga", true);
    texture[4].loadTexture("textures/AMF.tga", true);
    texture[5].loadTexture("textures/AMF.tga", true);
    texture[6].loadTexture("textures/AMF.tga", true);
    texture[7].loadTexture("textures/AMF.tga", true);
    texture[8].loadTexture("textures/AMF.tga", true);
    texture[9].loadTexture("textures/AMF.tga", true);
    texture[10].loadTexture("textures/AMF.tga", true);
    texture[11].loadTexture("textures/marble_ball.png", true);
}

bool initOpenGL()
{
    // Intialize GLFW
    // GLFW is configured.  Must be called before calling any GLFW functions
    if (!glfwInit())
    {
        // An error occured
        std::cerr << "GLFW initialization failed" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // forward compatible with newer versions of OpenGL as they become available but not backward compatible (it will not run on devices that do not support OpenGL 3.3

    // Create an OpenGL 3.3 core, forward compatible context window
    gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);
    if (gWindow == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    // Make the window's context the current one
    glfwMakeContextCurrent(gWindow);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    // Set the required callback functions
    glfwSetKeyCallback(gWindow, glfw_onKey);
    glfwSetFramebufferSizeCallback(gWindow, glfw_onFramebufferSize);
    glfwSetScrollCallback(gWindow, glfw_onMouseScroll);

    // Hides and grabs cursor, unlimited movement
    glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);

    glClearColor(gClearColor.r, gClearColor.g, gClearColor.b, gClearColor.a);

    // Define the viewport dimensions
    glViewport(0, 0, gWindowWidth, gWindowHeight);
    glEnable(GL_DEPTH_TEST);

    return true;
}

//-----------------------------------------------------------------------------
// Is called whenever a key is pressed/released via GLFW
//-----------------------------------------------------------------------------
void glfw_onKey(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
    {
        gWireframe = !gWireframe;
        if (gWireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        // toggle the flashlight
        gFlashlightOn = !gFlashlightOn;
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        respawn_scene = true;
        modelPos[11] = glm::vec3(-4.0f + 4.0f, 1.25f, 30.0f);
        release_ball = false;
    }
}

//For clamping
float clip(float n, float lower, float upper)
{
    return std::max(lower, std::min(n, upper));
}

//-----------------------------------------------------------------------------
// Is called when the window is resized
//-----------------------------------------------------------------------------
void glfw_onFramebufferSize(GLFWwindow *window, int width, int height)
{
    gWindowWidth = width;
    gWindowHeight = height;
    glViewport(0, 0, gWindowWidth, gWindowHeight);
}

//-----------------------------------------------------------------------------
// Called by GLFW when the mouse wheel is rotated
//-----------------------------------------------------------------------------
void glfw_onMouseScroll(GLFWwindow *window, double deltaX, double deltaY)
{
    double fov = fpsCamera.getFOV() + deltaY * ZOOM_SENSITIVITY;

    fov = glm::clamp(fov, 1.0, 120.0);

    fpsCamera.setFOV((float)fov);
}

//-----------------------------------------------------------------------------
// Update stuff every frame
//-----------------------------------------------------------------------------
void update(double elapsedTime)
{
    // Camera orientation
    double mouseX, mouseY;

    // Get the current mouse cursor position delta
    glfwGetCursorPos(gWindow, &mouseX, &mouseY);

    // Rotate the camera the difference in mouse distance from the center screen.  Multiply this delta by a speed scaler
    // fpsCamera.rotate((float)(gWindowWidth / 2.0 - mouseX) * MOUSE_SENSITIVITY, (float)(gWindowHeight / 2.0 - mouseY) * MOUSE_SENSITIVITY);

    // Clamp mouse cursor to center of screen
    glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);

    // Camera FPS movement

    // // Forward/backward
    if (glfwGetKey(gWindow, GLFW_KEY_W) == GLFW_PRESS)
        fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getLook());
    else if (glfwGetKey(gWindow, GLFW_KEY_S) == GLFW_PRESS)
        fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getLook());

    // // Strafe left/right
    if (glfwGetKey(gWindow, GLFW_KEY_A) == GLFW_PRESS)
        fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getRight());
    else if (glfwGetKey(gWindow, GLFW_KEY_D) == GLFW_PRESS)
        fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getRight());

    // // Up/down
    if (glfwGetKey(gWindow, GLFW_KEY_Z) == GLFW_PRESS)
        fpsCamera.move(MOVE_SPEED * (float)elapsedTime * glm::vec3(0.0f, 1.0f, 0.0f));
    else if (glfwGetKey(gWindow, GLFW_KEY_X) == GLFW_PRESS)
        fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -glm::vec3(0.0f, 1.0f, 0.0f));

    if (glfwGetKey(gWindow, GLFW_KEY_LEFT) == GLFW_PRESS && !release_ball)
    {
        if (respawn_scene)
        {
            // modelPos[11].x = 0.0f;
            release_ball = false;
            respawn_scene = false;
            ball_dir_left_or_right = 0.0;
            speed_factor = -0.2f;
            dynamic_points.clear();
            bezier_param = 0.0;
            modelPos[11] = glm::vec3(-0.0f, 1.25f, 30.0f); // original position
        }
        else
        {
            ball_dir_left_or_right = clip(ball_dir_left_or_right - 0.05, -4.0f, 4.0f);
            // std::cout << ball_dir_left_or_right << std::endl;
            modelPos[11].x = ball_dir_left_or_right;
        }
    }
    else if (glfwGetKey(gWindow, GLFW_KEY_RIGHT) == GLFW_PRESS && !release_ball)
    {
        if (respawn_scene)
        {
            // modelPos[11].x = 0.0f;
            respawn_scene = false;
            release_ball = false;
            ball_dir_left_or_right = 0.0;
            speed_factor = -0.2f;
            dynamic_points.clear();
            bezier_param = 0.0;
            modelPos[11] = glm::vec3(-0.0f, 1.25f, 30.0f); // original position
        }
        else
        {
            ball_dir_left_or_right = clip(ball_dir_left_or_right + 0.05, -4.0f, 4.0f);
            // std::cout << ball_dir_left_or_right << std::endl;
            modelPos[11].x = ball_dir_left_or_right;
        }
    }
    else if (glfwGetKey(gWindow, GLFW_KEY_UP) == GLFW_PRESS)
    {
        release_ball = true;
        if (ball_dir_left_or_right < 0.0)
        {
            dynamic_points.push_back(glm::vec3(modelPos[11].x, modelPos[11].y, modelPos[11].z));
            dynamic_points.push_back(glm::vec3(-4.0f, modelPos[11].y, 10.0f));
            dynamic_points.push_back(glm::vec3(-1.0f, modelPos[11].y, 4.0f));
            dynamic_points.push_back(glm::vec3(0.0f, modelPos[11].y, 0.0f));
        }
        else
        {
            dynamic_points.push_back(glm::vec3(modelPos[11].x, modelPos[11].y, modelPos[11].z));
            dynamic_points.push_back(glm::vec3(4.0f, modelPos[11].y, 10.0f));
            dynamic_points.push_back(glm::vec3(1.0f, modelPos[11].y, 4.0f));
            dynamic_points.push_back(glm::vec3(0.0f, modelPos[11].y, 0.0f));
        }
    }
    else if (glfwGetKey(gWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        speed_factor = clip(speed_factor - 0.007f, -0.4f, -0.2f);
        // std::cout << speed_factor << std::endl;
    }
}

//-----------------------------------------------------------------------------
// Code computes the average frames per second, and also the average time it takes
// to render one frame.  These stats are appended to the window caption bar.
//-----------------------------------------------------------------------------
void showFPS(GLFWwindow *window)
{
    static double previousSeconds = 0.0;
    static int frameCount = 0;
    double elapsedSeconds;
    double currentSeconds = glfwGetTime(); // returns number of seconds since GLFW started, as double float

    elapsedSeconds = currentSeconds - previousSeconds;

    // Limit text updates to 4 times per second
    if (elapsedSeconds > 0.25)
    {
        previousSeconds = currentSeconds;
        double fps = (double)frameCount / elapsedSeconds;
        double msPerFrame = 1000.0 / fps;

        // The C++ way of setting the window title
        std::ostringstream outs;
        outs.precision(3); // decimal places
        outs << std::fixed
             << APP_TITLE << "    "
             << "FPS: " << fps << "    "
             << "Frame Time: " << msPerFrame << " (ms)";
        glfwSetWindowTitle(window, outs.str().c_str());

        // Reset for next average.
        frameCount = 0;
    }

    frameCount++;
}
void mac_patch(GLFWwindow *window)
{
    if (glfwGetTime() > 3.0)
    {
        mac_moved = true;
    }
    // glfwGetTim

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE && !mac_moved)
    {
        int x, y;
        glfwGetWindowPos(window, &x, &y);
        glfwSetWindowPos(window, ++x, y);
    }
    else
    {
        mac_moved = true;
    }
}
void setUpDirectionalAndPointLights(ShaderProgram lightingShader)
{
    // Point Light positions
    glm::vec3 pointLightPos[3] = {
        glm::vec3(-5.0f, 3.8f, 0.0f),
        glm::vec3(0.5f, 3.8f, 0.0f),
        glm::vec3(5.0f, 3.8, 0.0f)};

    // Directional light
    lightingShader.setUniform("sunLight.direction", glm::vec3(0.0f, -0.9f, -0.17f));
    lightingShader.setUniform("sunLight.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
    lightingShader.setUniform("sunLight.diffuse", glm::vec3(0.1f, 0.1f, 0.1f)); // dark
    lightingShader.setUniform("sunLight.specular", glm::vec3(0.1f, 0.1f, 0.1f));

    // Point Light 1
    lightingShader.setUniform("pointLights[0].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    lightingShader.setUniform("pointLights[0].diffuse", glm::vec3(0.0f, 1.0f, 0.1f)); // green-ish light
    lightingShader.setUniform("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setUniform("pointLights[0].position", pointLightPos[0]);
    lightingShader.setUniform("pointLights[0].constant", 1.0f);
    lightingShader.setUniform("pointLights[0].linear", 0.22f);
    lightingShader.setUniform("pointLights[0].exponent", 0.20f);

    // Point Light 2
    lightingShader.setUniform("pointLights[1].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    lightingShader.setUniform("pointLights[1].diffuse", glm::vec3(1.0f, 0.1f, 0.0f)); // red-ish light
    lightingShader.setUniform("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setUniform("pointLights[1].position", pointLightPos[1]);
    lightingShader.setUniform("pointLights[1].constant", 1.0f);
    lightingShader.setUniform("pointLights[1].linear", 0.22f);
    lightingShader.setUniform("pointLights[1].exponent", 0.20f);

    // Point Light 3
    lightingShader.setUniform("pointLights[2].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    lightingShader.setUniform("pointLights[2].diffuse", glm::vec3(0.0f, 0.1f, 1.0f)); // blue-ish light
    lightingShader.setUniform("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setUniform("pointLights[2].position", pointLightPos[2]);
    lightingShader.setUniform("pointLights[2].constant", 1.0f);
    lightingShader.setUniform("pointLights[2].linear", 0.22f);
    lightingShader.setUniform("pointLights[2].exponent", 0.20f);
}
void setUpSpotLight(ShaderProgram lightingShader)
{

    glm::vec3 spotlightPos = glm::vec3(0.0, 10.433479, -0.696963);
    glm::vec3 spotlightLook = glm::vec3(0.01, -0.925675, -0.244831);
    lightingShader.setUniform("spotLight.ambient", glm::vec3(0.9f, 0.9f, 0.9f));
    lightingShader.setUniform("spotLight.diffuse", glm::vec3(5.0f, 5.0f, 5.0f));
    lightingShader.setUniform("spotLight.specular", glm::vec3(10.0f, 10.0f, 10.0f));
    lightingShader.setUniform("spotLight.position", spotlightPos);
    lightingShader.setUniform("spotLight.direction", spotlightLook);
    // lightingShader.setUniform("spotLight.direction", fpsCamera.getLook());
    lightingShader.setUniform("spotLight.cosInnerCone", glm::cos(glm::radians(15.0f)));
    lightingShader.setUniform("spotLight.cosOuterCone", glm::cos(glm::radians(20.0f)));
    lightingShader.setUniform("spotLight.constant", 4.0f);
    lightingShader.setUniform("spotLight.linear", 0.07f);
    lightingShader.setUniform("spotLight.exponent", 0.017f);
    lightingShader.setUniform("spotLight.on", gFlashlightOn);
}
glm::vec3 get_bezier_points(double t)
{
    float *point_array = &dynamic_points[0].x;
    glm::mat4x3 control_p = glm::make_mat4x3(point_array);
    return control_p * blend_mat * glm::vec4(1.0f, t, t * t, t * t * t);
}