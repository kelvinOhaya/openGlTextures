#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.h"
#include <string>
#include "FrameBuffer.h"
#include "Shape.h"

namespace Utils
{
	//GLOBALS
	//screen dimensions
	inline int WIDTH = 1200;
	inline 	int HEIGHT = 800;

	//switches/settings
	inline bool isDragging = false;
	inline bool cursorEnabled = true;
	inline bool processClick = false;

	//camera
	inline glm::vec3 position(4, 4, -16);
	inline Camera camera(position);

	//timing 
	inline float DELTA_TIME = 0.0f;
	inline float lastFrame = 0.0f;

	//mouse coordinates
	inline float lastX = 0.0f;
	inline float lastY = 0.0f;
	inline bool firstMouse = true;

	//initialize glfw with hints
	void initGlfw();

	//function declarations
	void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
	void processInput(GLFWwindow* window);
	void setGLFWCallbacks(GLFWwindow* window);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	void handleClick(FrameBuffer& fbo, GLFWwindow* window, Shape& shape);
	std::string getVec3DebugLog(std::string title, glm::vec3& vec);

};

