// Name: Adam Motaouakkil
// ID: 260956145

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "Axis.h"
#include "Motion.h"
#include "DAGNode.h"

using namespace std;

GLFWwindow *window; // Main application window
string RES_DIR = ""; // Where data files live
shared_ptr<Program> prog; // For drawing shape
shared_ptr<Program> prog2; // For drawing with colours
shared_ptr<Shape> shape1; // For drawing teapot
shared_ptr<Shape> shape2; // For drawing a sphere
shared_ptr<Shape> shape3; // For drawing a cube
shared_ptr<Axis> axis; // For drawing axes

// Float array needed to contain the frameData
float* frameData;

// Frame counter indices
int frameCounter = 0;
int steppingRate = 1;
int stepMem = 0;
int debugger = 0;

// BVH, GL variables needed to configure OpenGL program
Motion motion{};
GLuint vao;
GLuint posBufID; // position buffer for drawing a line loop
GLuint aPosLocation = 0; // location set in col_vert.glsl (or can be queried)
const GLuint NumVertices = 6;

// Scaling down the vertices axes to make everything viewable.
GLfloat vertices[NumVertices][3] = {
                    { 0, 0, 0 },
                    { 0.05, 0, 0 },
                    { 0, 0, 0 },
                    { 0, 0.05, 0 },
                    { 0, 0, 0 },
                    { 0, 0, 0.05 }
};

static void error_callback(int error, const char *description)
{
    cerr << description << endl;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        steppingRate++;
    }
    
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS && steppingRate > 1)
    {
        steppingRate--;
    }
    
    
    // SPACE: Change the stepping rate based on whether it's to 0 or not.
    // Keep a variable to store the previous stepping rate.
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        if (steppingRate != 0)
        {
            stepMem = steppingRate;
            steppingRate = 0;
        }
        else
        {
            steppingRate = stepMem;
        }
    }
    
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    {
        steppingRate *= -1;
    }
}

static void init()
{
    GLSL::checkVersion();

    // Check how many texture units are supported in the vertex shader
    int tmp;
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &tmp);
    cout << "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS = " << tmp << endl;
    // Check how many uniforms are supported in the vertex shader
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &tmp);
    cout << "GL_MAX_VERTEX_UNIFORM_COMPONENTS = " << tmp << endl;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &tmp);
    cout << "GL_MAX_VERTEX_ATTRIBS = " << tmp << endl;

    // Set background color.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    // Enable z-buffer test.
    glEnable(GL_DEPTH_TEST);

    // Initialize teapot mesh.
    shape1 = make_shared<Shape>();
    shape1->loadMesh(RES_DIR + "teapot.obj");
    shape1->init();
    
    // Initialize sphere mesh.
    shape2 = make_shared<Shape>();
    shape2->loadMesh(RES_DIR + "sphere.obj");
    shape2->init();
    
    // Initialize cube mesh.
    shape3 = make_shared<Shape>();
    shape3->loadMesh(RES_DIR + "cube.obj");
    shape3->init();
    
    // Initialize the GLSL programs.
    prog = make_shared<Program>();
    prog->setVerbose(true);
    prog->setShaderNames(RES_DIR + "nor_vert.glsl", RES_DIR + "nor_frag.glsl");
    prog->init();
    prog->addUniform("P");
    prog->addUniform("MV");
    prog->addAttribute("aPos");
    prog->addAttribute("aNor");
    prog->setVerbose(false);
    
    prog2 = make_shared<Program>();
    prog2->setVerbose(true);
    prog2->setShaderNames(RES_DIR + "col_vert.glsl", RES_DIR + "col_frag.glsl");
    prog2->init();
    prog2->addUniform("P");
    prog2->addUniform("MV");
    prog2->addUniform("col");
    prog2->addAttribute("aPos");
    prog2->setVerbose(false);
    
    
    // Load the BVH files, choose which ones to uncomment and display
    
    // motion.loadBVH(RES_DIR + "/0019_AdvanceBollywoodDance001.bvh");
    motion.loadBVH(RES_DIR + "/Cyrus_Take6.bvh");
    // motion.loadBVH(RES_DIR + "/OptiTrack-IITSEC2007.bvh");
    frameData = motion.data;
    
    // If there were any OpenGL errors, this will print something.
    // You can intersperse this line in your code to find the exact location
    // of your OpenGL error.
    GLSL::checkError(GET_FILE_LINE);

    // Create a buffers for doing some line drawing
    // Keep these lines to draw the axes as you will apply the
    // MV matrix transformations to move around the drawn axes.
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &posBufID);
    glBindBuffer(GL_ARRAY_BUFFER, posBufID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(aPosLocation);
    glVertexAttribPointer(aPosLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

static void render()
{
    // Get current frame buffer size.
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspect = width/(float)height;
    if (isnan(aspect))
    {
        aspect = 0;
    }
    glViewport(0, 0, width, height);

    // Clear framebuffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Create matrix stacks.
    auto P = make_shared<MatrixStack>();
    auto MV = make_shared<MatrixStack>();
    // Apply projection.
    P->pushMatrix();
    P->multMatrix(glm::perspective((float)(45.0*M_PI/180.0), aspect, 0.01f, 100.0f));
    // Apply camera transform.
    MV->pushMatrix();
    MV->translate(glm::vec3(0, 0, -5));
    
    
    // The index counter that allows appropriate motion through frameData.
    int frameIndex = frameCounter * (motion.numChannels);
    
    axis = make_shared<Axis>();
    motion.root->draw(prog2, prog, shape1, shape2, shape3,
                      MV, frameData, P, axis,
                      vao, frameIndex, debugger);
    
    // Draw the world frame axis
    MV->scale(5);
    axis->draw(prog2, MV, P, vao);

    // Pop matrix stacks.
    MV->popMatrix();
    P->popMatrix();
    
    // Update the speed based on the stepping rate.
    frameCounter += steppingRate;;
    
    if (frameCounter >= motion.numFrames)
    {
        frameCounter = 0;
    }
    else if (frameCounter <= 0)
    {
        frameCounter = motion.numFrames - 1;
    }
    GLSL::checkError(GET_FILE_LINE);
}

int main(int argc, char **argv)
{
    if(argc < 2) {
        cout << "Please specify the resource directory." << endl;
        return 0;
    }
    RES_DIR = argv[1] + string("/");

    // Set error callback.
    glfwSetErrorCallback(error_callback);
    // Initialize the library.
    if(!glfwInit()) {
        return -1;
    }
    
    // https://en.wikipedia.org/wiki/OpenGL
    // hint to use OpenGL 4.1 on all paltforms
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // Create a windowed mode window and its OpenGL context.
    window = glfwCreateWindow(640, 480, "YOUR NAME", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return -1;
    }
    // Make the window's context current.
    glfwMakeContextCurrent(window);
    // Initialize GLEW.
    glewExperimental = true;
    if(glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW" << endl;
        return -1;
    }
    glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
    cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    // Set vsync.
    glfwSwapInterval(1);
    // Set keyboard callback.
    glfwSetKeyCallback(window, key_callback);
    // Initialize scene.
    init();
    // Loop until the user closes the window.
    while(!glfwWindowShouldClose(window)) {
        // Render scene.
        render();
        
        // Create a frame counter in the title.
        std::string temp = "Adam Motaouakkil: 260956145";
        glfwSetWindowTitle(window, temp.c_str());
        
        // Swap front and back buffers.
        glfwSwapBuffers(window);
        // Poll for and process events.
        glfwPollEvents();
    }
    // Quit program.
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
