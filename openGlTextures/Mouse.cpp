#include "Mouse.h"




//glm::vec3 Mouse::getIntersectionPoint(Model& s) {
//    Ray ray = castRay();
//
//    glm::mat4 invModel = glm::inverse(s.modelMatrix);
//    glm::vec3 localOrigin = glm::vec3(invModel * glm::vec4(ray.origin, 1.0f));
//    glm::vec3 localDirection = glm::vec3(invModel * glm::vec4(ray.direction, 0.0f));
//
//    float hitDistance = s.mesh.pointIntersectedWithBox(localDirection, localOrigin);
//
//    // CRUCIAL: Use local variables here!
//    glm::vec3 intersectionPoint = localOrigin + (hitDistance * localDirection);
//    return intersectionPoint;
//}

//bool Mouse::intersects(Model& s) {
//    Ray ray = castRay();
//
//    // Get the inverse of the object's model matrix and transform to local space
//    glm::mat4 invModel = glm::inverse(s.modelMatrix);
//    glm::vec3 localOrigin = glm::vec3(invModel * glm::vec4(ray.origin, 1.0f));
//    glm::vec3 localDirection = glm::normalize(glm::vec3(invModel * glm::vec4(ray.direction, 0.0f)));
//
//    float hitDistance = s.mesh.pointIntersectedWithBox(localDirection, localOrigin);
//
//    return !(hitDistance < 0);
//
//    //std::cout << "Ray Coordinates: (" << ray.direction.x << ", " << ray.direction.y << ", " << ray.direction.z << ")" << std::endl;
//}

//void Mouse::printHit(Model& s) {
//    if (window == nullptr) throw std::runtime_error("Window must be initialized");
//    std::cout << (intersects(s) ? "Hit!" : "Miss") << std::endl;
//    leftButtonIsDown = false;
//}

void Mouse::buttonCallback(GLFWwindow* windowArg, int button, int action, int mods) {
    Mouse* mouseInstance = static_cast<Mouse*>(glfwGetWindowUserPointer(windowArg));
    if (!mouseInstance) throw std::runtime_error("Mouse must be initialized");
    mouseInstance->instanceButtonCallback(button, action, mods);
}

void Mouse::mouseCallback(GLFWwindow* windowArg, double xpos, double ypos) {
    Mouse* mouseInstance = static_cast<Mouse*>(glfwGetWindowUserPointer(windowArg));
    if (!mouseInstance) throw std::runtime_error("Mouse must be initialized");
    mouseInstance->instanceMouseCallback(xpos, ypos);
}

void Mouse::instanceButtonCallback(int button, int action, int mods)
{
    //determines if the viewport was pressed, rather than ImGui's ui
    auto viewportWasPressed = [](int action) {
        const auto& io = ImGui::GetIO();
        return action == GLFW_PRESS && (!io.WantCaptureMouse && !io.WantCaptureKeyboard);
        };

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (viewportWasPressed(action)) {
            leftButtonIsDown = true;
            isDragging = true;
            if (processClick) processClick = false;
            else if (!processClick) processClick = true;
        }
        else if (action == GLFW_RELEASE) {
            leftButtonIsDown = false;
            isDragging = false;
            processClick = false;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (viewportWasPressed(action)) {
            rightButtonIsDown = true;
            cursorEnabled = !cursorEnabled;
        }
        else if (action == GLFW_RELEASE) {
            rightButtonIsDown = false;
        }
    }
}

void Mouse::instanceMouseCallback(double xpos, double ypos)
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

void Mouse::init(GLFWwindow* win) {
    window = win;

    glfwSetWindowUserPointer(window, this);
    glfwSetMouseButtonCallback(window, Mouse::buttonCallback);
    glfwSetCursorPosCallback(window, Mouse::mouseCallback);

}