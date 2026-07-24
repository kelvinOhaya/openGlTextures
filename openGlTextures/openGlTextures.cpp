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
#include "Model.h"
#include "buffers/VertexAttribute.h"
#include "Mesh.h"
#include "Texture2D.h"
#include "FrameBuffer.h"
#include <GLFW/glfw3.h>
#include <wrl/def.h>
#include "Mouse.h"

//GLOBALS
GLFWwindow* window = nullptr;


//switches/settings
 bool isDragging = false;
 bool cursorEnabled = true;
 bool processClick = false;

//camera
 glm::vec3 position(4, 4, -40);
 Camera camera(position);

 //mouse (needs model and camera to initialize)
 Mouse mouse(window, camera);

 //triangle coords
 float coords[9] = {
    0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f
 };

//should camera own projection matrix?
 glm::mat4 view = camera.GetViewMatrix();
 glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)(Globals::WIDTH / Globals::HEIGHT), 0.1f, 100.0f);

//timing 
 float DELTA_TIME = 0.0f;
 float lastFrame = 0.0f;

//function declarations
void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void handleClick(FrameBuffer& fbo, GLFWwindow* window, Model& model);
std::string getVec3DebugLog(std::string title, glm::vec3& vec);
//
//bool rayTriangleIntersect(
//    const glm::vec3& orig, const glm::vec3& dir,
//    const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
//    float& t)
//{
//    const float EPSILON = 0.0000001f;
//    glm::vec3 edge1 = v1 - v0;
//    glm::vec3 edge2 = v2 - v0;
//    glm::vec3 h = glm::cross(dir, edge2);
//    float a = glm::dot(edge1, h);
//    if (a > -EPSILON && a < EPSILON) return false; // Ray is parallel to triangle
//
//    float f = 1.0f / a;
//    glm::vec3 s = orig - v0;
//    float u = f * glm::dot(s, h);
//    if (u < 0.0f || u > 1.0f) return false;
//
//    glm::vec3 q = glm::cross(s, edge1);
//    float v = f * glm::dot(dir, q);
//    if (v < 0.0f || u + v > 1.0f) return false;
//
//    t = f * glm::dot(edge2, q);
//    return t > EPSILON; // Intersection found
//}

//scratchapixel mollor trumbore ray triangle intersection
//https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle//moller-trumbore-ray-triangle-intersection.html
//bool rayTriangleIntersect(glm::vec3& origin, glm::vec3& direction, glm::vec3& v0, glm::vec3& v1, glm::vec3& v2, float &t) {
//    float u, v;
//    float epsilion = 0.000001;
//    glm::vec3 v0v1 = v1 - v0;
//    glm::vec3 v0v2 = v2 - v0;
//
//    //triple scalar method - find the dot product for the determinant
//    glm::vec3 pvec = glm::cross(direction, v0v2);
//    float det = glm::dot(pvec, v0v1);
//
//    if (det > -epsilion && det < epsilion) return false;
//
//    float inv = 1 / det;
//
//    glm::vec3 tvec = origin - v0;
//    u = inv * glm::dot(tvec, pvec);
//    if (u < 0 || u > 1) return false;
//
//    glm::vec3 qvec = glm::cross(tvec, v0v1);
//    v = inv* glm::dot(direction, qvec);
//    if (v < 0 || v > 1) return false;
//
//    t = glm::dot(v0v2, qvec) * inv;
//
//    return true;
//}

int main()
{
    //initialize GLFW with hints
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    //create window
    window = glfwCreateWindow(Globals::WIDTH, Globals::HEIGHT, "Smoothie", NULL, NULL);
    if (window == NULL) {
        std::cout << "Error creating the window: " << std::endl;
        glfwTerminate();
        return -1;
    }

    mouse.init(window);

    //make this window the current context
    glfwMakeContextCurrent(window);
    //initialzie glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Error loading GLAD: failed to initialize" << std::endl;
        return -1;
    }

    //set the viewport size
    glViewport(0, 0, Globals::WIDTH, Globals::HEIGHT);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
    glfwSetScrollCallback(window, scroll_callback);
    glEnable(GL_DEPTH_TEST);

    glm::vec3 pos(0);
    camera.setInitialFocus(pos);
    Model model;
    model.init("models/MarioPenguin.DAE");

    //shader creation
    Shader cubeShader("shaders/cubeShader.vert", "shaders/cubeShader.frag");
    Shader triangleShader("shaders/triangle.vert", "shaders/triangle.frag");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    std::string currentPos;
    std::string rotationValue;
    VertexBuffer bufferObj;
    VertexAttribute vao;
    vao.bind();
    size_t coordsSize = 9 * sizeof(float);
    bufferObj.init(nullptr, 9 * sizeof(float));
    bufferObj.bind();
    vao.addPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    bool hasSelectedTriangle = false;
    vao.unbind();
    bufferObj.unbind();


    //rendering loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        if (cursorEnabled) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        model.updateModelMatrix();


        //calculate mvp
        view = camera.GetViewMatrix();
        projection = camera.GetProjectionMatrix();
        glm::mat4 mvp = projection * view * model.modelMatrix;


        //if (mouse.leftButtonIsDown) {
        //    if (mouse.intersects(model)) {
        //        glm::vec3 rayOrigin = camera.Position;
        //        glm::vec3 rayDir = mouse.castRay().direction;

        //        
     
        //       

        //        glm::mat4 invModel = glm::inverse(model.modelMatrix);
        //        glm::vec3 localOrigin = glm::vec3(invModel * glm::vec4(rayOrigin, 1.0f));
        //        glm::vec3 localDir = glm::normalize(glm::vec3(invModel * glm::vec4(rayDir, 0.0f)));

        //        float closestT = std::numeric_limits<float>::max();
        //        bool foundTriangle = false;
        //        glm::vec3 trueV1(0.0f), trueV2(0.0f), trueV3(0.0f);

        //        // --- SWITCH TO THE OPENMESH FACE ITERATOR ---
        //        for (auto f_it = model.mesh.data.faces_begin(); f_it != model.mesh.data.faces_end(); ++f_it) {
        //            
        //            // The Face-Vertex iterator cleanly extracts the 3 connected nodes
        //            auto fv_it = model.mesh.data.fv_iter(*f_it);
        //            
        //            auto p0 = model.mesh.data.point(*fv_it); ++fv_it;
        //            auto p1 = model.mesh.data.point(*fv_it); ++fv_it;
        //            auto p2 = model.mesh.data.point(*fv_it);

        //            // Convert OpenMesh points to glm::vec3
        //            glm::vec3 v0(p0[0], p0[1], p0[2]);
        //            glm::vec3 v1(p1[0], p1[1], p1[2]);
        //            glm::vec3 v2(p2[0], p2[1], p2[2]);

        //            float t;
        //            if (rayTriangleIntersect(localOrigin, localDir, v0, v1, v2, t)) {
        //                if (t < closestT) {
        //                    closestT = t;
        //                    trueV1 = v0;
        //                    trueV2 = v1;
        //                    trueV3 = v2;
        //                    foundTriangle = true;
        //                }
        //            }
        //        }

        //        if (foundTriangle) {
        //            float coordValues[9] = {
        //                trueV1.x, trueV1.y, trueV1.z,
        //                trueV2.x, trueV2.y, trueV2.z,
        //                trueV3.x, trueV3.y, trueV3.z
        //            };

        //            std::copy(std::begin(coordValues), std::end(coordValues), std::begin(coords));
        //            hasSelectedTriangle = true;
        //        }
        //    }
        //}
        //else {
        //    // OPTIONAL: If you want the triangle highlight to disappear 
        //    // the instant you let go of the mouse button, uncomment the line below:
        //    // hasSelectedTriangle = false;
        //}
        
        currentPos = getVec3DebugLog("Position", model.translation);
        rotationValue = "\nRotation:\nX: " + std::to_string(model.rotateX) + "\nY: " + std::to_string(model.rotateY) + "\nZ: " + std::to_string(model.rotateZ);
        //ui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Camera Settings");
        ImGui::InputFloat("Camera speed", &camera.MovementSpeed, 1, 5);
        ImGui::End();

        ImGui::Begin("Model");
        ImGui::Text("Rotation");
        ImGui::SliderFloat("X-axis", &model.rotateX, 0, 360);
        ImGui::SliderFloat("Y-axis", &model.rotateY, 0, 360);
        ImGui::SliderFloat("Z-axis", &model.rotateZ, 0, 360);

        ImGui::Text("Position");
        ImGui::SliderFloat("X-axis##xx", &model.translation.x, -20, 20);
        ImGui::SliderFloat("Y-axis##xx", &model.translation.y, -20, 20);
        ImGui::SliderFloat("Z-axis##xx", &model.translation.z, -20, 20);

        ImGui::Text("Scale");
        ImGui::SliderFloat("scale", &model.scaleFactor,0, 20);

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

        float currentFrame = glfwGetTime();
        
        DELTA_TIME = currentFrame - lastFrame;
        lastFrame = currentFrame;
       
        if (const auto& io = ImGui::GetIO(); !io.WantCaptureMouse && !io.WantCaptureKeyboard) {
            processInput(window);
        }


        //use the shader object now
        cubeShader.use();
        cubeShader.setMatrix4f("projection", projection);
        cubeShader.setMatrix4f("view", view);
        cubeShader.setMatrix4f("model", model.modelMatrix);
        cubeShader.setMatrix4f("mvp", mvp);
        cubeShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        cubeShader.setVec3("objectColor", model.getColor());
        cubeShader.setVec3("viewPos", camera.Position);
        cubeShader.setMatrix3f("inverseNormal", glm::mat3(glm::transpose(glm::inverse(model.modelMatrix))));
        //draw the elements
        model.draw();


        //if (hasSelectedTriangle) {
   
        //    vao.bind();
        //    bufferObj.bind();
        //    bufferObj.updateData(coords, 9 * sizeof(float));
        //   

        //    // Shader logic
        //    triangleShader.use();
        //    triangleShader.setMatrix4f("mvp", mvp);

        //    // 1. Keep depth testing ON, but enable Polygon Offset
        //    glDisable(GL_DEPTH_TEST);

        //    // 2. Subtle push to bring the selection slightly forward toward the camera
        //    glPolygonOffset(-1.0f, -1.0f);

        //    // 3. Draw your clean triangle!
        //    glDrawArrays(GL_TRIANGLES, 0, 3);

        //    glEnable(GL_DEPTH_TEST);

        //    vao.unbind();
        //    bufferObj.unbind();
        //}

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



void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {


    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse) { camera.ProcessMouseScroll((float)yoffset); }
}

void handleClick(FrameBuffer& fbo, GLFWwindow* window, Model& model) {
    fbo.bind();
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    uint32_t pixelData[4];
    //opengl and mouse use different coord sytem, hence the HEIGHT flip
    glReadPixels(mouseX, Globals::HEIGHT - mouseY, 1, 1, GL_RGBA_INTEGER, GL_UNSIGNED_INT, &pixelData);
    fbo.unbind();
    uint32_t clickedObjID = pixelData[0];
    if (clickedObjID == 1) model.setColor(ModelColor::MAGENTA);
    else model.setColor(ModelColor::WHITE);

    //std::cout << "ObjectID: " << clickedObjID << std::endl;
}

std::string getVec3DebugLog(std::string title, glm::vec3& vec) {
    return title + ":\nX: " + std::to_string(vec.x) + "\nY: " + std::to_string(vec.y) + "\nZ: " + std::to_string(vec.z);
};

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, Globals::WIDTH, Globals::HEIGHT);
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