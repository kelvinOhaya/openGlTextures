#include "shader.h"
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

//function declarations
void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int WIDTH = 800;
int HEIGHT = 800;

int main()
{
    //initialize GLFW with hints
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

    //create window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Texture", NULL, NULL);
    if (window == NULL) {
        std::cout << "Error creating the window: " << std::endl;
        glfwTerminate();
        return -1;
    }

    //make this window the current context
    glfwMakeContextCurrent(window);
    //initialzie glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Error loading GLAD: failed to initialize" << std::endl;
        return -1;
    }

    //set the viewport size
    glViewport(0, 0, WIDTH, HEIGHT);

    //VERTEX CREATION & BUFFER BINDING
   //---------------------------------------------------------------
   //vertices for our square
    float vertices[] = {
    0.5f, 0.5f, 0.0f, // top right
    0.5f, -0.5f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, // bottom left
    -0.5f, 0.5f, 0.0f // top left
    };
    unsigned int indices[] = { // note that we start from 0!
    0, 1, 3, // first triangle
    1, 2, 3 // second triangle
    };

    unsigned int VBO, VAO, EBO;

    //vertex buffer creation
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    //BINDING
    //VAO & EBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //attribute pointer (VAO)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //our file paths for the shader object
    const char* vertexPath = "shaders/vertexShader.txt";
    const char* fragmentPath = "shaders/fragmentShader.txt";

    //creating the shader object
    Shader myShader(vertexPath, fragmentPath);

    //rendering loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        //code for green fading in and out
        float timeValue = glfwGetTime();
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        
        //use the shader object now
        myShader.use();
        
        //update the color uniform vertex with the code above
        int vertexColorLocation = glGetUniformLocation(myShader.ID, "ourColor");
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        
        //draw the elements
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //swap the frame buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //terminate the program
    glfwTerminate();
    return 0;
}

//tells opengl what we want to do when the window resizes
void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

//processes all our input keys
void processInput(GLFWwindow* window) {
    //close the window if esc is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
