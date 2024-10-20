#include "Viewport.h"

#include "../Surface.h"
#include "../core/tracking/Audio/AudioTracking.h"

void window_size_callback(GLFWwindow* _window, int _width, int _height)
{
    glViewport(0, 0, _width, _height);
};

bool Viewport::init()
{
    if (!glfwInit())
    {
        // Initialization failed
        std::cout << "Initialization failed." << std::endl;
        return false;
    }

    // GLFW Settings
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Get GLFW window
    Viewport::window = glfwCreateWindow(640, 480, "VAvatar", NULL, NULL);

    if (!window)
    {
        // Window creation failed
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(Viewport::window);

    gladLoadGL();
    

    glfwSetWindowSizeCallback(Viewport::window, window_size_callback);

    glEnable(GL_FRAMEBUFFER_SRGB); // Enables the Depth Buffer
    glDisable(0x809D);  // Disable multisampling

    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_GREATER);

    // Blend
    glEnable(GL_BLEND); // Enables the Depth Buffer
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    loop();


    // Free GLFW
    glfwDestroyWindow(Viewport::window);
    glfwTerminate();

    return false;
}

void Viewport::loop() const
{
    Shader shader("default.vert", "default.frag");

    GLuint uniID = glGetUniformLocation(shader.ID, "scale");


    Surface sur = Surface("Head.png", 0.6f, 0.6f, 0.1f, 0.1f, shader);
    Surface MouthClosed = Surface("Head.png", 0.1f, 0.1f, 0.1f, 0.1f, shader);
    Surface MouthOpen = Surface("Head.png", 0.1f, 0.1f, 0.1f, 0.01f, shader);

    AudioTracking* aud = new AudioTracking;

    while (!glfwWindowShouldClose(Viewport::window))
    {
        // Specify the color of the background
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Activate();
        glUniform1f(uniID, 0.5f);
        sur.Bind();

        aud->captureAudio();

        if (aud->isActivated())
        {
            MouthOpen.Bind();

        }
        else
        {
            MouthClosed.Bind();
        }

        
        //input();
        //update();
       // render();

        glfwSwapBuffers(Viewport::window);    // Swap the back buffer with the front buffer
        glfwPollEvents();                     // Take care of all GLFW events
    }

    sur.Delete();
}

// Python input value anchors
void Viewport::input() const
{
    //world.input();
}

// Update the avatars new position, Rig, Emotion type
void Viewport::update()
{
    //world->update();
}

// Render the avatar new position
void Viewport::render()
{
    //world->render();
}