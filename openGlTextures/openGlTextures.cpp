#include <glad/glad.h>
#include <glm/detail/setup.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "ImageLoader.h"
#include "camera.h"
#include "shader.h"
#include "stb_image.h"
#include "buffers/VertexBuffer.h"
#include "Shape.h"
#include "buffers/VertexAttribute.h"
#include "Mesh.h"
#include "Texture2D.h"
#include "FrameBuffer.h"
#include <GLFW/glfw3.h>
#include "Utils.h"

using namespace Utils;

//rotation speed for the model matrix
float rotationSpeed = 100.0f;

//LIGHTING
glm::vec3 lightPos(4.0f, 4.0f, 2.0f);
double lightRotationSpeed = 0;
float lightRotateAcceleration = 1.02;

glm::mat4 view = camera.GetViewMatrix();
glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)(Utils::WIDTH / Utils::HEIGHT), 0.1f, 100.0f);


int main()
{
    //initialize GLFW with hints
    initGlfw();
    //create window
    GLFWwindow* window = glfwCreateWindow(Utils::WIDTH, Utils::HEIGHT, "Smoothie", NULL, NULL);
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
    glViewport(0, 0, Utils::WIDTH, Utils::HEIGHT);
    setGLFWCallbacks(window);
    glEnable(GL_DEPTH_TEST);

    glm::vec3 pos(0);
    camera.setInitialFocus(pos);
    Shape shape = Shape("models/dragon.obj", 10, 10, 10, pos, camera);
    shape.setColor(ShapeColor::WHITE);

    //shader creation
    Shader cubeShader("shaders/cubeShader.vert", "shaders/cubeShader.frag");
    Shader lightShader("shaders/lightShader.vert", "shaders/lightShader.frag");
    Shader pickerShader("shaders/pickingShader.vert", "shaders/pickingShader.frag");

    //model view projection math
   

    //framebuffer logic(for now)
    FrameBuffer fbo;
    Texture2D texture;
    fbo.bind();
    texture.texImage(GL_TEXTURE_2D, 0, GL_RGB32UI, Utils::WIDTH, Utils::HEIGHT, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL);
    texture.minLinearParam();
    texture.magLinearParam();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.getID(), 0);
   
    //create renderbuffer
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, Utils::WIDTH, Utils::HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    std::string currentPos;
    std::string rotationValue;
 
    //rendering loop
    while (!glfwWindowShouldClose(window)) {
        fbo.bind();
        //calculate mvp
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)(Utils::WIDTH / Utils::HEIGHT), 0.1f, 2000.0f);
        glm::mat4 mvp = projection * view * shape.modelMatrix;

        pickerShader.use();
        pickerShader.setMatrix4f("mvp", mvp);
        pickerShader.setMatrix4f("model", shape.modelMatrix);
        pickerShader.setUnsignedInt("objIdx", 1);//random id since we only have one object
        texture.unbind();
   
        uint32_t clearValue[] = {999,0,0,0};
        glViewport(0, 0, Utils::WIDTH, Utils::HEIGHT);
        glClearBufferuiv(GL_COLOR, 0, clearValue);
        glClear(GL_DEPTH_BUFFER_BIT);
        texture.bind();
        shape.draw();
        texture.unbind();
        fbo.unbind();

        

        if (processClick) handleClick(fbo, window, shape);
        currentPos = getVec3DebugLog("Position", shape.translation);
        rotationValue = "\nRotation:\nX: " + std::to_string(shape.rotateX) + "\nY: " + std::to_string(shape.rotateY) + "\nZ: " + std::to_string(shape.rotateZ);
        //ui
        ImGuiSliderFlags_AlwaysClamp;
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Camera Settings");
        ImGui::InputFloat("Camera speed", &camera.MovementSpeed, 1, 5);
        ImGui::End();

        ImGui::Begin("Model");
        ImGui::Text("Rotation");
        ImGui::SliderFloat("X-axis", &shape.rotateX, 0, 360);
        ImGui::SliderFloat("Y-axis", &shape.rotateY, 0, 360);
        ImGui::SliderFloat("Z-axis", &shape.rotateZ, 0, 360);

        ImGui::Text("Position");
        ImGui::SliderFloat("X-axis##xx", &shape.translation.x, -2, 2);
        ImGui::SliderFloat("Y-axis##xx", &shape.translation.y, -2, 2);
        ImGui::SliderFloat("Z-axis##xx", &shape.translation.z, -2, 2);
        ImGui::End();

        ImGui::Begin("Debugging");
        ImGui::Text(currentPos.c_str());
        ImGui::Text(rotationValue.c_str());
        ImGui::End();

        ImGui::Begin("Performance Metrics");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
            1000.0f / ImGui::GetIO().Framerate,
            ImGui::GetIO().Framerate);

        ImGui::End();

        shape.updateModelMatrix();

        float currentFrame = glfwGetTime();
        if (cursorEnabled) {
            glfwSetInputMode(window, GLFW_CURSOR,GLFW_CURSOR_NORMAL );
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        DELTA_TIME = currentFrame - lastFrame;
        lastFrame = currentFrame;
       
        if (const auto& io = ImGui::GetIO(); !io.WantCaptureMouse && !io.WantCaptureKeyboard) {
            processInput(window);
        }
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //use the shader object now
        cubeShader.use();
        //cubeShader.setInt("texture1", 0);
        //cubeShader.setInt("texture2", 1);     
        lightRotationSpeed = 0;
        lightRotateAcceleration  = 0;

        glm::vec4 trueLight = glm::vec4(lightPos, 1.0f);
       
        cubeShader.setMatrix4f("projection", projection);
        cubeShader.setMatrix4f("view", view);
        cubeShader.setMatrix4f("model", shape.modelMatrix);
        cubeShader.setMatrix4f("mvp", mvp);
        cubeShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        cubeShader.setVec3("objectColor", shape.getColor());
        cubeShader.setVec3("lightPos", glm::vec3(trueLight.x, trueLight.y, trueLight.z));
        cubeShader.setVec3("viewPos", camera.Position);
        cubeShader.setMatrix3f("inverseNormal", glm::mat3(glm::transpose(glm::inverse(shape.modelMatrix))));
        //draw the elements
        shape.draw();

        //render ui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        //swap the frame buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //de-allocate all resources
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
