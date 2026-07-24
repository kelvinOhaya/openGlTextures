#pragma once
#define _USE_MATH_DEFINES
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <glm/detail/setup.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Globals.h"
#include "camera.h"
#include "Model.h"

//all functions should end with setting their respective flag to false if they want click functionality rather than holding functionality
struct Ray {
	glm::vec3 direction;
	glm::vec3 origin;
};


class Mouse {
private:
	GLFWwindow* window;
	Camera& camera;

	//mouse coordinates
	float lastX = 0.0f;
	float lastY = 0.0f;
	bool firstMouse = true;

	//checks if we have an existing mouse instance. If we do, run the instanced callback
	static void buttonCallback(GLFWwindow* windowArg, int button, int action, int mods);
	static void mouseCallback(GLFWwindow* windowArg, double xpos, double ypos);
	//callback for button being pressed
	void instanceButtonCallback(int button, int action, int mods);
	void instanceMouseCallback(double xpos, double ypos);

public:
	Mouse() = delete;
	Mouse(GLFWwindow* window, Camera& camera) : window(window), camera(camera) {};
	//switches/settings
	bool isDragging = false;
	bool cursorEnabled = true;
	bool processClick = false;
	bool leftButtonIsDown = false;
	bool rightButtonIsDown = false;
	void printHit(Model& s);
	//Ray castRay();
	//bool intersects(Model& s);
	//glm::vec3 getIntersectionPoint(Model& s);
	void init(GLFWwindow* window);
};