#include "Mouse.h"


Ray Mouse::castRay() {
    if (window == nullptr) throw std::runtime_error("Window must be initialized");
    double mouseX, mouseY;
    // FIX: Dynamically fetch the current window size instead of using Globals
    int currentWidth, currentHeight;
    glfwGetWindowSize(window, &currentWidth, &currentHeight);
    glfwGetCursorPos(window, &mouseX, &mouseY);
    //make our normalized device coordinate ray (z is negative so it can face forwards)
    glm::vec4 m_clip(((2 * mouseX) / currentWidth - 1), 1 - (2 * mouseY) / currentHeight, -1, 1);
    //reverse projection matrix to get to camera space
    glm::vec4 m_eye(glm::inverse(camera.GetProjectionMatrix()) * m_clip);
    //clean up random z and w values from inverse projection operation
    m_eye.z = -1;
    m_eye.w = 1;
    //reverse camera space to get world space
    glm::vec4 m_worldCalc((glm::inverse(camera.GetViewMatrix()) * m_eye));
    glm::vec3 m_worldClickPoint(
        m_worldCalc.x,
        m_worldCalc.y,
        m_worldCalc.z);
    //direction = destination - start
    //make a ray from the camera pos to the point in world space
    glm::vec3 ray(glm::normalize(glm::vec3(m_worldClickPoint - camera.Position)));
    Ray result;
    result.origin = camera.Position;
    result.direction = ray;
    return result;
}

glm::vec3 Mouse::getIntersectionPoint(Shape& s) {
    Ray ray = castRay();

    glm::mat4 invModel = glm::inverse(s.modelMatrix);
    glm::vec3 localOrigin = glm::vec3(invModel * glm::vec4(ray.origin, 1.0f));
    glm::vec3 localDirection = glm::vec3(invModel * glm::vec4(ray.direction, 0.0f));

    float hitDistance = s.mesh.pointIntersectedWithBox(localDirection, localOrigin);

    // CRUCIAL: Use local variables here!
    glm::vec3 intersectionPoint = localOrigin + (hitDistance * localDirection);
    return intersectionPoint;
}

bool Mouse::intersects(Shape& s) {
    Ray ray = castRay();

    // Get the inverse of the object's model matrix and transform to local space
    glm::mat4 invModel = glm::inverse(s.modelMatrix);
    glm::vec3 localOrigin = glm::vec3(invModel * glm::vec4(ray.origin, 1.0f));
    glm::vec3 localDirection = glm::normalize(glm::vec3(invModel * glm::vec4(ray.direction, 0.0f)));

    float hitDistance = s.mesh.pointIntersectedWithBox(localDirection, localOrigin);

    return !(hitDistance < 0);

    //std::cout << "Ray Coordinates: (" << ray.direction.x << ", " << ray.direction.y << ", " << ray.direction.z << ")" << std::endl;
}

void Mouse::printHit(Shape& s) {
    if (window == nullptr) throw std::runtime_error("Window must be initialized");
    std::cout << (intersects(s) ? "Hit!" : "Miss") << std::endl;
    leftButtonIsDown = false;
}

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