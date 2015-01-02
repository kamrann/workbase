/*
callbacks.h
*/

#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <glfw/glfw3.h>


struct UIState
{
	bool showMenu;
	int scroll;
	int scrollarea1;
	bool mouseOverMenu;
	bool chooseTest;
};


void sCreateUI();
void sResizeWindow(GLFWwindow*, int width, int height);
void sKeyCallback(GLFWwindow*, int key, int scancode, int action, int mods);
void sMouseButton(GLFWwindow*, int button, int action, int mods);
void sMouseMotion(GLFWwindow*, double xd, double yd);
void sScrollCallback(GLFWwindow*, double, double dy);
void sRestart();
void sSimulate();
void sInterface();



#endif
