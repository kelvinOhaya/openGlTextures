#include "shader.h"
#include "stb_image.h"
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <glm/detail/setup.hpp>
#define GLM_SWIZZLE GLM_SWIZZLE_FULL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//function declarations
void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, glm::vec3& scaleVec, glm::vec3& translateVec, float& rotateDegrees);

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
        // positions      // colors         // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // top right 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // bottom right  0.0f, 1.0f
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // bottom left 1.0f, 1.0f
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,0.0f// top left 1.0f, 0.0f
    };

    unsigned int indices[] = { // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };


    unsigned int VBO, VAO, EBO, texture1, texture2;

    //buffer creation
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    glGenTextures(1, &texture1);
    glGenTextures(1, &texture2);

    //BINDING & FORMATTING
    //--------------------------------------------------------------------------------      
    //VAO & EBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);
    //attribute pointer
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(2);


    //TEXTURE
    //loading in texture (pg. 59)
    int t1width, t1height, t1nrChannels, t1alignment, t2width, t2height, t2nrChannels, t2alignment; //nrChannels = number of color channels

    //mario
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    //parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    //texture1
    stbi_set_flip_vertically_on_load(false);
    unsigned char* t1Data = stbi_load("textures/mario.png", &t1width, &t1height, &t1nrChannels, 0);
    //generate the image and mimaps if data exists
    if (t1Data)
    {
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &t1alignment);
        std::cout << "TEXTURE1: " << std::endl;
        std::cout << "Data: " << &t1Data << std::endl;
        std::cout << "Channels: " << t1nrChannels << std::endl;
        std::cout << "Alignment: " << t1alignment << std::endl;
        //pg. 60 or https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t1width, t1height, 0, GL_RGBA, GL_UNSIGNED_BYTE, t1Data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "failed to load textures" << std::endl;
    }
    stbi_image_free(t1Data);


    //sky
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    //texture2
    unsigned char* t2Data = stbi_load("textures/sky.png", &t2width, &t2height, &t2nrChannels, 0);
    //generate the image and mimaps if data exists
    if (t2Data)
    {
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &t2alignment);
        std::cout << "TEXTURE2: " << std::endl;
        std::cout << "Data: " << &t2Data << std::endl;
        std::cout << "Channels: "  << t2nrChannels << std::endl;
        std::cout << "Alignment: " << t2alignment << std::endl;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        //pg. 60 or https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t2width, t2height, 0, GL_RGB, GL_UNSIGNED_BYTE, t2Data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "failed to load textures" << std::endl;
    }

    //free our data
    stbi_image_free(t2Data);


    //our file paths for the shaders
    const char* vertexPath = "shaders/shader.vert";
    const char* fragmentPath = "shaders/shader.frag";

    //strings for the shader obj
    std::string texture1str = "texture1";
    std::string texture2str = "texture2";

    //creating the shader object
    Shader myShader(vertexPath, fragmentPath);
   
    glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.1f);
    glm::vec3 scaleVec = glm::vec3(0.5f, 0.5f, 0.0f);
    glm::vec3 translateVec = glm::vec3(0.0f, 0.0f, 0.1f);
    float rotateDegrees = 0.0f;

    //rendering loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window, scaleVec, translateVec, rotateDegrees);
        //use the shader object now
        myShader.use();
        myShader.setInt(texture1str, 0);
        myShader.setInt(texture2str, 1);

        glm::mat4 trans = glm::mat4(1.0f);

        //transforming matrices
        trans = glm::translate(trans, translateVec);
        trans = glm::rotate(trans, glm::radians(rotateDegrees), origin);
        trans = glm::scale(trans, scaleVec);

        unsigned int transformLoc = glGetUniformLocation(myShader.ID, "trans");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
       
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
void processInput(GLFWwindow* window, glm::vec3 &scaleVec, glm::vec3 &translateVec, float &rotateDegrees) {
    //close the window if esc is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    //UP DOWN LEFT RIGHT (scaling & rotating)
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        scaleVec.x += 0.001f;
        scaleVec.y += 0.001f;
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        scaleVec.x -= 0.001f;
        scaleVec.y -= 0.001f;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        rotateDegrees += 0.4f;
    }
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        rotateDegrees -= 0.4f;
    }
    //WASD movement
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        translateVec.y += 0.004f;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        translateVec.y -= 0.004f;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        translateVec.x += 0.004f;
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        translateVec.x -= 0.004f;
    }
    
}
