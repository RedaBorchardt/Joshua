#define STB_IMAGE_IMPLEMENTATION

// Local Headers
#include "joshua.hpp"
#include "shader.hpp"
#include "eliza.hpp"

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;
float mixvalue = 0.5f;

static void add_responses(JOSHUA::eliza& el);

int main()
{
	JOSHUA::eliza el;
	add_responses(el);

    std::cout << "Hello. I'm " << el.name() << ". How are you feeling today?\n";
    std::string input;
    while (std::getline(std::cin, input) && input != "quit") {
        std::cout << el.respond(input) << "\n";
    }
	// Start sound engine
	//
	irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();

	if (!engine)
			return 0;
	engine->play2D("Resources/Sounds/ambient.mp3", true);

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
	const char* glsl_version= "#version 450";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Joshua", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // gl3w: load all OpenGL function pointers
    // ---------------------------------------

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	if (gl3wInit()) {
		fprintf(stderr, "failed to initialize OpenGL\n");
		return -1;
	}
	if (!gl3wIsSupported(3, 2)) {
		fprintf(stderr, "OpenGL 3.2 not supported\n");
		return -1;
	}
	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
	       glGetString(GL_SHADING_LANGUAGE_VERSION));

	// IMGUI Setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("Resources/Shaders/simple.vert", "Resources/Shaders/simple.frag");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // load and create a texture 
    // -------------------------
    unsigned int texture1, texture2;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load("Resources/Textures/uvgrid.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("Resources/Textures/psygnosis.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
    // or set it via the texture class
    ourShader.setInt("texture2", 1);

    // Camera
    //glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    //glm::vec3 cameraTarget = glm::vec3(0.0f,0.0f,0.0f);
    //glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

    //glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    //glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    //glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

    glm::mat4 view;
    view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f),
		    glm::vec3(0.0f, 0.0f, 0.0f),
		    glm::vec3(0.0f, 1.0f, 0.0f));

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

		// Start IMGUI Frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Show a simple window
		ImGui::Begin("Hello Joshua");
		ImGui::Text("Welcome professor Falken");
		ImGui::Text("The current mix value: %.3f", mixvalue); 
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

	// tranforms
	// -----
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, (float)glfwGetTime()*10, glm::vec3(0.0f, 0.0f, 1.0f));
	//trans = glm::scale(trans, glm::vec3((float)sin(glfwGetTime()/10), (float)sin(glfwGetTime()/10), 1.0f));
	//trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));

        // render
        // ------ 
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

	// Going 3D
        glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        //glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);
        model = glm::rotate(model, 0.0f+(float)glfwGetTime()*20, glm::vec3(1.0f, 0.0f, 0.0f));
        //view  = glm::translate(view, glm::vec3(0.0f, -0.0f, -2.0f));
        projection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	// render container
        ourShader.use();
		ourShader.setFloat("mixvalue", mixvalue);
		ourShader.setMat4("transform", trans);
		ourShader.setMat4("model", model);
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

	// irrKlang Cleanup
	engine->drop();

	// IMGUI Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
	glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	mixvalue -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	mixvalue += 0.01f;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

static void add_responses(JOSHUA::eliza& el)
{
    el.responds_to("Can you (.*)")
        .with("Don't you believe that I can %1%?")
        .with("You want me to be able to %1%?");
 
    el.responds_to("Can I ([^\\?]*)\\??")
        .with("Perhaps you don't want to %1%.")
        .with("Do you want to be able to %1%?")
        .with("If you could %1%, would you?");
 
    el.responds_to("You are (.*)")
        .with("Why do you think I am %1%?")
        .with("Does it please you to think that I'm %1%?")
        .with("Perhaps you would like be %1%.")
        .with("Do you sometimes wish you were %1%?");
 
    el.responds_to("I don'?t (.*)")
        .with("Don't you really %1%?")
        .with("Why don't you %1%?")
        .with("Do you wish to be able to %1%?");
 
    el.responds_to("I feel (.*)")
        .with("Does it trouble you to feel %1%?")
        .with("Do you often feel %1%?")
        .with("Do you enjoy feeling %1%?")
        .with("When do you usually feel %1%?")
        .with("When you feel %1%, what do you do?");
 
    el.responds_to("Why don'?t you ([^\\?]*)\\??")
        .with("Do you really believe I don't %1%?")
        .with("Perhaps in good time I will %1%.")
        .with("Do you want me to %1%?");
 
    el.responds_to("Why can'?t I ([^\\?]*)\\??")
        .with("Do you think you should be able to %1%?")
        .with("Why can't you?");
 
    el.responds_to("Are you ([^\\?]*)\\??")
        .with("Why are you interested in whether I am %1%?")
        .with("Would you prefer it if I were not %1%?")
        .with("Perhaps in your fantasies I am %1%.");
 
    el.responds_to("I can'?t (.*)")
        .with("How do you know you can't %1%?")
        .with("Have you tried to %1%?")
        .with("Perhaps now you can %1%");
 
    el.responds_to("I am (.*)")
        .with("Did you come to me because you are %1%?")
        .with("How long have you been %1%?");
 
    el.responds_to("I'?m (.*)")
        .with("Do you believe it is normal to be %1%?")
        .with("Do you enjoy being %1%?");
 
    el.responds_to("You (.*)")
        .with("We were discussing you - not me.")
        .with("Oh, I %1%?");
 
    el.responds_to("I want (.*)")
        .with("What would it mean to you if you got %1%?")
        .with("Why do you want %1%?")
        .with("Suppose you soon got %1%?")
        .with("What if you never got %1%?")
        .with("I sometimes also want %1%");
 
    el.responds_to("What (.*)")
        .with("Why do you ask?")
        .with("Does that question interest you?")
        .with("What answer would please you the most?")
        .with("What do you think?")
        .with("Are such questions on your mind often?")
        .with("What is it that you really want to know?")
        .with("Have you asked anyone else?")
        .with("Have you asked such questions before?")
        .with("What else comes to mind when you ask that?");
 
    el.responds_to("Because")
        .with("Is that the real reason?")
        .with("Don't any other reasons come to mind?")
        .with("Does that reason explain anything else?")
        .with("What other reasons might there be?");
 
    el.responds_to("Sorry")
        .with("There are many times when no apology is needed.")
        .with("What feelings do you have when you apologize?");
 
    el.responds_to("Dream")
        .with("What does that dream suggest to you?")
        .with("Do you dream often?")
        .with("What persons appear in your dreams?")
        .with("Are you disturbed by your dreams?");
 
    el.responds_to("^Hello")
        .with("How do you do... Please state your problem.");
 
    el.responds_to("^Maybe")
        .with("You don't seem quite certain.")
        .with("Why the uncertain tone?")
        .with("Can't you be more positive?")
        .with("You aren't sure?")
        .with("Don't you know?");
 
    el.responds_to("^No[.!]?$")
        .with("Are you saying that just to be negative?")
        .with("You are being a bit negative.")
        .with("Why not?")
        .with("Are you sure?")
        .with("Why no?");
 
    el.responds_to("Your (.*)")
        .with("Why are you concerned about my %1%?")
        .with("What about your own %1%?");
 
    el.responds_to("Always")
        .with("Can you think of a specific example?")
        .with("When?")
        .with("Really, always?");
 
    el.responds_to("Think (.*)")
        .with("What are you thinking of?")
        .with("Do you really think so?")
        .with("But you are not sure %1%?")
        .with("Do you doubt %1%?");
 
    el.responds_to("Alike")
        .with("In what way?")
        .with("What resemblance do you see?")
        .with("What does the similarity suggest to you?")
        .with("what other connections do you see?")
        .with("Could there really be some connection?")
        .with("How?");
 
    el.responds_to("^Yes[.!]?$")
        .with("You seem quite positive.")
        .with("Are you sure?")
        .with("I see.")
        .with("I understand.");
 
    el.responds_to("Friend")
        .with("Why do you bring up the topic of friends?")
        .with("Do your friends worry you?")
        .with("Do your friends pick on you?")
        .with("Are you sure you have any friends?")
        .with("Do you impose on your friends?")
        .with("Perhaps your love for friends worries you.");
 
    el.responds_to("Computer")
        .with("Do computers worry you?")
        .with("Are you talking about me in particular?")
        .with("Are you frightened by machines?")
        .with("Why do you mention computers?")
        .with("What do you think machines have to do with your problem?")
        .with("Don't you think computers can help people?")
        .with("What is it about machines that worries you?");
 
    el.responds_to("(.*)")
        .with("Say, do you have any psychological problems?")
        .with("What does that suggest to you?")
        .with("I see.")
        .with("I'm not sure I understand you fully.")
        .with("Come come elucidate your thoughts.")
        .with("Can you elaborate on that?")
        .with("That is quite interesting.");
}
