#include "Utils.h"
#include <imgui_impl_opengl3.h>
#include <boost/algorithm/algorithm.hpp>



namespace Utils
{
 
	void initGlfw()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	}
	//callbacks
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

        if ((isDragging) || !cursorEnabled) { camera.ProcessMouseMovement((float)xoffset, (float)yoffset); }
        //print camera position
        //std::cout << camera.Position.x << ", ";
        //std::cout << camera.Position.y << ", ";
        //std::cout << camera.Position.z << "\n\n";
    }

    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    
        ImGuiIO& io = ImGui::GetIO();
        if(!io.WantCaptureMouse) {camera.ProcessMouseScroll((float)yoffset);}
    } 

    void handleClick(FrameBuffer& fbo, GLFWwindow* window, Shape& shape) {
        fbo.bind();
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        uint32_t pixelData[4];
        //opengl and mouse use different coord sytem, hence the Utils::HEIGHT flip
        glReadPixels(mouseX, Utils::HEIGHT-mouseY, 1,1,GL_RGBA_INTEGER, GL_UNSIGNED_INT, &pixelData);
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
        glViewport(0, 0, Utils::WIDTH, Utils::HEIGHT);
    }

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (const auto& io = ImGui::GetIO(); action == GLFW_PRESS && (!io.WantCaptureMouse && !io.WantCaptureKeyboard)) {
                isDragging = true;
                if (processClick) processClick = false;
                else if (!processClick) processClick = true;
            }
            else if (action == GLFW_RELEASE && isDragging) {
                isDragging = false;
                processClick = false;
            }
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            if (action == GLFW_PRESS) {
                cursorEnabled = !cursorEnabled;
            }
        }
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

    void setGLFWCallbacks(GLFWwindow* window)
    {
        glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
    }

};
