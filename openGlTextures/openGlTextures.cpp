#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <glm/detail/setup.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ImageLoader.h"
#include "camera.h"
#include "shader.h"
#include "stb_image.h"
#include "buffers/VertexBuffer.h"
#include "Shape.h"
#include "buffers/VertexAttribute.h"
#include "Mesh.h"

//GLOBALS
//screen dimensions
int WIDTH = 800;
int HEIGHT = 800;
bool isDragging = false;
bool cursorEnabled = true;

//timing 
float DELTA_TIME = 0.0f;
float lastFrame = 0.0f;

//mouse coordinates
float lastX = 0.0f;
float lastY = 0.0f;
bool firstMouse = true;

//rotation speed for the model matrix
float rotationSpeed = 100.0f;

//struct for origin coordinates for rotaion
struct Origin {
    glm::vec3 xAxis = glm::vec3(0.1f, 0.0f, 0.0f);
    glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);
};
Origin origin;

//camera
glm::vec3 position(4,4,-4);
Camera camera(position);

//LIGHTING
glm::vec3 lightPos(4.0f, 4.0f, 2.0f);
double lightRotationSpeed = 0;
float lightRotateAcceleration = 1.02;


//function declarations
void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
glm::mat4 rotateModel(float x, float y, float z);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);





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
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //initialzie glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Error loading GLAD: failed to initialize" << std::endl;
        return -1;
    }

    //set the viewport size
    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glEnable(GL_DEPTH_TEST);

    unsigned int texture1, texture2;

    glm::vec3 pos(0,0,0);
    camera.setInitialFocus(pos);
    Shape shape = Shape(1, 1, 1, pos, camera);
    shape.setColor(ShapeColor::CYAN);


    glGenTextures(1, &texture1);
    glGenTextures(1, &texture2);

    //creating the shader object
    Shader cubeShader("shaders/cubeShader.vert", "shaders/cubeShader.frag");
    Shader lightShader("shaders/lightShader.vert", "shaders/lightShader.frag");
   
    //rendering loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        if (cursorEnabled) {
            glfwSetInputMode(window, GLFW_CURSOR,GLFW_CURSOR_NORMAL );
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        DELTA_TIME = currentFrame - lastFrame;
        lastFrame = currentFrame;
       
        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //use the shader object now
        cubeShader.use();
        //cubeShader.setInt("texture1", 0);
        //cubeShader.setInt("texture2", 1);     
        lightRotationSpeed = 0;
        lightRotateAcceleration  = 0;

        glm::mat4 lightRotateMat = rotateModel(0,0, 0);
        glm::vec4 trueLight = lightRotateMat * glm::vec4(lightPos, 1.0f);
        glm::mat4 cubeModel, lightModel, view, projection, cubeClipped, lightClipped;
       
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)(WIDTH / HEIGHT), 0.1f, 100.0f);
        cubeShader.setMatrix4f("projection", projection);
        cubeShader.setMatrix4f("view", view);
        cubeShader.setMatrix4f("model", shape.getModelMatrix());
        cubeShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        cubeShader.setVec3("objectColor", shape.getColor());
        cubeShader.setVec3("lightPos", glm::vec3(trueLight.x, trueLight.y, trueLight.z));
        cubeShader.setVec3("viewPos", camera.Position);
        //draw the elements
        shape.draw();

        //swap the frame buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------

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
    //WASD movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, DELTA_TIME);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, DELTA_TIME);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, DELTA_TIME);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, DELTA_TIME);
    }  
}

//callbacks
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isDragging = true;
        }
        else if (action == GLFW_RELEASE) {
            isDragging = false;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            cursorEnabled = !cursorEnabled;
        }
    }
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    if ((cursorEnabled && isDragging) || (!cursorEnabled)) { camera.ProcessMouseMovement((float)xoffset, (float)yoffset); }
    //print camera position
    //std::cout << camera.Position.x << ", ";
    //std::cout << camera.Position.y << ", ";
    //std::cout << camera.Position.z << "\n\n";
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    
        camera.ProcessMouseScroll((float)yoffset);
   
} 

glm::mat4 rotateModel(float x, float y, float z) {
    glm::mat4 model(1.0f); // <-- ADD 1.0f HERE to make it an identity matrix
    model = glm::rotate(model, glm::radians(x), origin.xAxis) * glm::rotate(model, glm::radians(y), origin.yAxis) * glm::rotate(model, glm::radians(z), origin.zAxis);
    return model;
}