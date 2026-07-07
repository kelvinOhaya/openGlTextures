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

 //mouse (needs shape and camera to initialize)
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
void handleClick(FrameBuffer& fbo, GLFWwindow* window, Shape& shape);
std::string getVec3DebugLog(std::string title, glm::vec3& vec);

bool rayTriangleIntersect(
    const glm::vec3& orig, const glm::vec3& dir,
    const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
    float& t)
{
    const float EPSILON = 0.0000001f;
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(dir, edge2);
    float a = glm::dot(edge1, h);
    if (a > -EPSILON && a < EPSILON) return false; // Ray is parallel to triangle

    float f = 1.0f / a;
    glm::vec3 s = orig - v0;
    float u = f * glm::dot(s, h);
    if (u < 0.0f || u > 1.0f) return false;

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(dir, q);
    if (v < 0.0f || u + v > 1.0f) return false;

    t = f * glm::dot(edge2, q);
    return t > EPSILON; // Intersection found
}

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
    Shape shape("models/yoshi.obj", 1, 1, 1, pos, camera);
    shape.setColor(ShapeColor::WHITE);
    std::cout << "GPU Vertex Count: " << shape.mesh.vertices.size() << std::endl;
    std::cout << "GPU Index Count: " << shape.mesh.indices.size() << std::endl;
    std::cout << "OpenMesh Face Count: " << shape.mesh.omMesh.n_faces() << std::endl;

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

        shape.updateModelMatrix();


        //calculate mvp
        //calculate mvp
        view = camera.GetViewMatrix();
        projection = camera.GetProjectionMatrix();
        glm::mat4 mvp = projection * view * shape.modelMatrix;

       

       // if (mouse.leftButtonIsDown) {
        
        //    if (mouse.intersects(shape)) {
        //        glm::vec3 dir = mouse.getIntersectionPoint(shape);

        //        Vertex* nearest = shape.mesh.nearestVertex(dir.x, dir.y, dir.z);

        //        // 1. Get the starting half-edge
        //        HalfEdge outEdge = shape.mesh.edges[nearest->out];

        //        // 2. CRUCIAL: If this edge is an unassigned boundary seam, 
        //        // flip over to its twin so we walk a real filled face!
        //        if (outEdge.face == -1) {
        //            outEdge = shape.mesh.edges[outEdge.twin];
        //        }

        //        // 3. Now we are guaranteed to be on a closed triangle loop!
        //        HalfEdge outEdge2 = shape.mesh.edges[outEdge.next];
        //        HalfEdge outEdge3 = shape.mesh.edges[outEdge2.next];

        //        Vertex firstVertex = shape.mesh.vertices[outEdge.origin];
        //        Vertex secondVertex = shape.mesh.vertices[outEdge2.origin];
        //        Vertex thirdVertex = shape.mesh.vertices[outEdge3.origin];
        //       
        //        float coordValues[9] = {
        //            firstVertex.pos.x, firstVertex.pos.y, firstVertex.pos.z,
        //            secondVertex.pos.x, secondVertex.pos.y, secondVertex.pos.z,
        //            thirdVertex.pos.x, thirdVertex.pos.y, thirdVertex.pos.z
        //        };

        //        std::copy(std::begin(coordValues), std::end(coordValues), std::begin(coords));
        //        hasSelectedTriangle = true;
        //        std::cout << "Coordinates: " << std::endl;
        //        for (size_t i = 1; const auto& coord : coords) {
        //            std::cout << coord << ", " << ((i > 0 && i % 3 == 0) ? "\n" : "");
        //            i++;
        //        }
        //        std::cout << std::endl;

        //        size_t length = (sizeof(coords) / sizeof(coords[0]));
        //        std::cout << "Length: " << length << std::endl;

        //      
        //    }
        //}

        if (mouse.leftButtonIsDown) {
            if (mouse.intersects(shape)) {
                glm::vec3 rayOrigin = camera.Position;
                glm::vec3 rayDir = mouse.getIntersectionPoint(shape) - camera.Position;
                rayDir = glm::normalize(rayDir);

                glm::mat4 invModel = glm::inverse(shape.modelMatrix);
                glm::vec3 localOrigin = glm::vec3(invModel * glm::vec4(rayOrigin, 1.0f));
                glm::vec3 localDir = glm::normalize(glm::vec3(invModel * glm::vec4(rayDir, 0.0f)));

                float closestT = std::numeric_limits<float>::max();
                bool foundTriangle = false;
                glm::vec3 trueV1(0.0f), trueV2(0.0f), trueV3(0.0f);

                // --- SWITCH TO THE OPENMESH FACE ITERATOR ---
                for (auto f_it = shape.mesh.omMesh.faces_begin(); f_it != shape.mesh.omMesh.faces_end(); ++f_it) {
                    
                    // The Face-Vertex iterator cleanly extracts the 3 connected nodes
                    auto fv_it = shape.mesh.omMesh.fv_iter(*f_it);
                    
                    auto p0 = shape.mesh.omMesh.point(*fv_it); ++fv_it;
                    auto p1 = shape.mesh.omMesh.point(*fv_it); ++fv_it;
                    auto p2 = shape.mesh.omMesh.point(*fv_it);

                    // Convert OpenMesh points to glm::vec3
                    glm::vec3 v0(p0[0], p0[1], p0[2]);
                    glm::vec3 v1(p1[0], p1[1], p1[2]);
                    glm::vec3 v2(p2[0], p2[1], p2[2]);

                    float t;
                    if (rayTriangleIntersect(localOrigin, localDir, v0, v1, v2, t)) {
                        if (t < closestT) {
                            closestT = t;
                            trueV1 = v0;
                            trueV2 = v1;
                            trueV3 = v2;
                            foundTriangle = true;
                        }
                    }
                }

                if (foundTriangle) {
                    float coordValues[9] = {
                        trueV1.x, trueV1.y, trueV1.z,
                        trueV2.x, trueV2.y, trueV2.z,
                        trueV3.x, trueV3.y, trueV3.z
                    };

                    std::copy(std::begin(coordValues), std::end(coordValues), std::begin(coords));
                    hasSelectedTriangle = true;
                }
            }
        }
        else {
            // OPTIONAL: If you want the triangle highlight to disappear 
            // the instant you let go of the mouse button, uncomment the line below:
            // hasSelectedTriangle = false;
        }
        
        currentPos = getVec3DebugLog("Position", shape.translation);
        rotationValue = "\nRotation:\nX: " + std::to_string(shape.rotateX) + "\nY: " + std::to_string(shape.rotateY) + "\nZ: " + std::to_string(shape.rotateZ);
        //ui
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
        cubeShader.setMatrix4f("model", shape.modelMatrix);
        cubeShader.setMatrix4f("mvp", mvp);
        cubeShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        cubeShader.setVec3("objectColor", shape.getColor());
        cubeShader.setVec3("viewPos", camera.Position);
        cubeShader.setMatrix3f("inverseNormal", glm::mat3(glm::transpose(glm::inverse(shape.modelMatrix))));
        //draw the elements
        shape.draw();


        if (hasSelectedTriangle) {
            vao.bind();
            bufferObj.bind();
            bufferObj.updateData(coords, 9 * sizeof(float));

            // Shader logic
            triangleShader.use();
            triangleShader.setMatrix4f("mvp", mvp);

            // 1. Keep depth testing ON, but enable Polygon Offset
            
            glDisable(GL_DEPTH_TEST);

            // 2. Subtle push to bring the selection slightly forward toward the camera
            glPolygonOffset(-1.0f, -1.0f);

            // 3. Draw your clean triangle!
            glDrawArrays(GL_TRIANGLES, 0, 3);

            glEnable(GL_DEPTH_TEST);

            vao.unbind();
            bufferObj.unbind();
        }

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

void handleClick(FrameBuffer& fbo, GLFWwindow* window, Shape& shape) {
    fbo.bind();
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    uint32_t pixelData[4];
    //opengl and mouse use different coord sytem, hence the HEIGHT flip
    glReadPixels(mouseX, Globals::HEIGHT - mouseY, 1, 1, GL_RGBA_INTEGER, GL_UNSIGNED_INT, &pixelData);
    fbo.unbind();
    uint32_t clickedObjID = pixelData[0];
    if (clickedObjID == 1) shape.setColor(ShapeColor::MAGENTA);
    else shape.setColor(ShapeColor::WHITE);

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