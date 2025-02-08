#include "Viewport.h"

#include "../Surface.h"
#include "../core/tracking/Audio/AudioTracking.h"
#include "PNGTuber.h"
#include "../Config.h"

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
    // Create Shader
    Shader shader("default.vert", "default.frag");
    GLuint uniID = glGetUniformLocation(shader.ID, "scale");

    // Load Config class
    Config config;
    if (!config.load("Config.txt")) {
        return; // Handle error
    }

    // Get the amount of contents

    // Define the number of objects for each state
    const int numImages = config.getInt("importer_defaults", "num_images");

    std::vector<PNGTuber> pngTuberDefault;
    std::vector<PNGTuber> pngTuberSilent;
    std::vector<PNGTuber> pngTuberTalk;

    for (int i = 0; i < numImages; i++)
    {

        // get id of image in text and chose whether its pngTuberDefault, pngTuberSilent, pngTuberTalk
        std::string index = std::to_string(i);

        std::string texturePath = config.getString("importer_defaults", "image" + index + "_path");

        float x = config.getFloat("importer_defaults", "image" + index + "_x");
        float y = config.getFloat("importer_defaults", "image" + index + "_y");
        float width = config.getFloat("importer_defaults", "image" + index + "_w");
        float height = config.getFloat("importer_defaults", "image" + index + "_h");

        PNGTuber pngTuber(texturePath.c_str(), width, height, x, y, shader);

        cout << texturePath.c_str() << endl;

        //std::cout << config.getFloat("importer_defaults", "image" + index + "_bounce_min_x") << std::endl;
        //std::cout << config.getFloat("importer_defaults", "image" + index + "_bounce_min_y") << std::endl;

        //glm::vec2 bouncemin = glm::vec2(config.getFloat("importer_defaults", "image" + index + "_bounce_min_x"), config.getFloat("importer_defaults", "image" + index + "_bounce_min_y"));
        //glm::vec2 bouncemax = glm::vec2(config.getFloat("importer_defaults", "image" + index + "_bounce_min_x"), config.getFloat("importer_defaults", "image" + index + "_bounce_max_y"));
        //pngTuber.setBounce(bouncemin, bouncemax);

        //glm::vec2 squishmin = glm::vec2(config.getFloat("importer_defaults", "image" + index + "_squish_min_x"), config.getFloat("importer_defaults", "image" + index + "_squish_max_x"));
        //glm::vec2 squishmax = glm::vec2(config.getFloat("importer_defaults", "image" + index + "_squish_min_y"), config.getFloat("importer_defaults", "image" + index + "_squish_max_y"));
        //pngTuber.setSquish(squishmin, squishmax);

        //glm::vec2 freqencemin = glm::vec2(config.getFloat("importer_defaults", "image" + index + "_frequency_min"), config.getFloat("importer_defaults", "image" + index + "_frequency_max"));
        //glm::vec2 freqencemax = glm::vec2(config.getFloat("importer_defaults", "image" + index + "_bounce_min_y"), config.getFloat("importer_defaults", "image" + index + "_bounce_max_y"));
        //pngTuber.setFreqence(freqencemin, freqencemax);

        //float amplitudeMin = config.getFloat("importer_defaults", "image" + index + "_amplitude_min");
        //float amplitudeMax = config.getFloat("importer_defaults", "image" + index + "_amplitude_max");

        //pngTuber.setAmplitude(glm::vec2(), glm::vec2());
        //pngTuber.setRotaional(config.getFloat("importer_defaults", "image" + index + "_rotational_min"), config.getFloat("importer_defaults", "image" + index + "_rotational_max"));

        //float dragMin = config.getFloat("importer_defaults", "image" + index + "_drag_min");
        //float dragMax = config.getFloat("importer_defaults", "image" + index + "_drag_max");
        //pngTuber.setDrag(dragMin, dragMax);

        //pngTuber.setColor(config.getFloat("importer_defaults", "image" + index + "_color_r"), config.getFloat("importer_defaults", "image" + index + "_color_g"), config.getFloat("importer_defaults", "image" + index + "_color_b"));
        

        if (config.getInt("importer_defaults", "image" + index + "_type") == -1)
        {
            pngTuberDefault.push_back(pngTuber);
        }
        if (config.getInt("importer_defaults", "image" + index + "_type") == 0)
        {
            pngTuberSilent.push_back(pngTuber);
        }
        if (config.getInt("importer_defaults", "image" + index + "_type") == 1)
        {
            pngTuberTalk.push_back(pngTuber);
        }
    }

    //PNGTuber sur = PNGTuber("Head.png", 0.6f, 0.6f, 0.1f, 0.1f, shader);
    //PNGTuber MouthClosed = PNGTuber("MouthClosed.png", 0.2f, 0.2f, 0.1f, 0.01f, shader);
    //PNGTuber MouthOpen = PNGTuber("MouthOpen.png", 0.2f, 0.2f, 0.1f, 0.01f, shader);

    AudioTracking* aud = new AudioTracking;


    while (!glfwWindowShouldClose(Viewport::window))
    {
        // Specify the color of the background
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Activate();
        glUniform1f(uniID, 0.5f);

        // Update
        aud->captureAudio();


        // We get all the pngtuber classes and update them
        for (int i = 0;  i < pngTuberDefault.size(); i++)
        {
            pngTuberDefault[i].update();
            pngTuberDefault[i].render();
        }
        //sur.update();

        // Render
        //sur.render();

        if (aud->isActivated())
        {
            // We get all Talking and render them
            for (int i = 0; i < pngTuberTalk.size(); i++)
            {
                pngTuberTalk[i].update();
                pngTuberTalk[i].render();
            }
            //MouthOpen.render();

        }
        else
        {
            // We get all Silent and render them
            for (int i = 0; i < pngTuberSilent.size(); i++)
            {
                pngTuberSilent[i].update();
                pngTuberSilent[i].render();
            }
            //MouthClosed.render();
        }

        glfwSwapBuffers(Viewport::window);    // Swap the back buffer with the front buffer
        glfwPollEvents();                     // Take care of all GLFW events
    }

    // Terminate
    pngTuberDefault.clear();
    pngTuberTalk.clear();
    pngTuberSilent.clear();

    //sur.Delete();
    //MouthOpen.Delete();
    //MouthClosed.Delete();
}