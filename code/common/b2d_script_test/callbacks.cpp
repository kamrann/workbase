/*
* Copyright (c) 2006-2013 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include <glew/glew.h>
#include "callbacks.h"
#include "imgui.h"
#include "RenderGL3.h"
#include "DebugDraw.h"
#include "ScriptTest.h"

#include <stdio.h>

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

//
//namespace
//{
	GLFWwindow* mainWindow = NULL;
	UIState ui;

//	int32 testIndex = 0;
//	int32 testSelection = 0;
//	int32 testCount = 0;
	size_t script_index = 0;
	ScriptTestEntry* entry;
	ScriptTest* test;
	Settings settings;
	bool rightMouseDown;
	b2Vec2 lastp;
//}


//
void sCreateUI()
{
	ui.showMenu = true;
	ui.scroll = 0;
	ui.scrollarea1 = 0;
	ui.chooseTest = false;
	ui.mouseOverMenu = false;

	// Init UI
    const char* fontPath = "./Data/DroidSans.ttf";
    
	if (RenderGLInit(fontPath) == false)
	{
		fprintf(stderr, "Could not init GUI renderer.\n");
		assert(false);
		return;
	}
}

//
void sResizeWindow(GLFWwindow*, int width, int height)
{
	g_camera.m_width = width;
	g_camera.m_height = height;
}

//
void sKeyCallback(GLFWwindow*, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			// Quit
			glfwSetWindowShouldClose(mainWindow, GL_TRUE);
			break;

		case GLFW_KEY_LEFT:
			// Pan left
			if (mods == GLFW_MOD_CONTROL)
			{
				b2Vec2 newOrigin(2.0f, 0.0f);
				test->ShiftOrigin(newOrigin);
			}
			else
			{
				g_camera.m_center.x -= 0.5f;
			}
			break;

		case GLFW_KEY_RIGHT:
			// Pan right
			if (mods == GLFW_MOD_CONTROL)
			{
				b2Vec2 newOrigin(-2.0f, 0.0f);
				test->ShiftOrigin(newOrigin);
			}
			else
			{
				g_camera.m_center.x += 0.5f;
			}
			break;

		case GLFW_KEY_DOWN:
			// Pan down
			if (mods == GLFW_MOD_CONTROL)
			{
				b2Vec2 newOrigin(0.0f, 2.0f);
				test->ShiftOrigin(newOrigin);
			}
			else
			{
				g_camera.m_center.y -= 0.5f;
			}
			break;

		case GLFW_KEY_UP:
			// Pan up
			if (mods == GLFW_MOD_CONTROL)
			{
				b2Vec2 newOrigin(0.0f, -2.0f);
				test->ShiftOrigin(newOrigin);
			}
			else
			{
				g_camera.m_center.y += 0.5f;
			}
			break;

		case GLFW_KEY_HOME:
			// Reset view
			g_camera.m_zoom = 1.0f;
			g_camera.m_center.Set(0.0f, 20.0f);
			break;

		case GLFW_KEY_Z:
			// Zoom out
			g_camera.m_zoom = b2Min(1.1f * g_camera.m_zoom, 20.0f);
			break;

		case GLFW_KEY_X:
			// Zoom in
			g_camera.m_zoom = b2Max(0.9f * g_camera.m_zoom, 0.02f);
			break;

		case GLFW_KEY_R:
			// Reset test
			delete test;
			test = entry->createFcn(b2d_scripts[script_index].filepath);
			break;

		case GLFW_KEY_SPACE:
			// Launch a bomb.
			if (test)
			{
				test->LaunchBomb();
			}
			break;

		case GLFW_KEY_P:
			// Pause
			settings.pause = !settings.pause;
			break;

/*		case GLFW_KEY_LEFT_BRACKET:
			// Switch to previous test
			--testSelection;
			if (testSelection < 0)
			{
				testSelection = testCount - 1;
			}
			break;

		case GLFW_KEY_RIGHT_BRACKET:
			// Switch to next test
			++testSelection;
			if (testSelection == testCount)
			{
				testSelection = 0;
			}
			break;
*/
		case GLFW_KEY_TAB:
			ui.showMenu = !ui.showMenu;

		default:
			if (test)
			{
				test->Keyboard(key);
			}
		}
	}
	else if (action == GLFW_RELEASE)
	{
		test->KeyboardUp(key);
	}
	// else GLFW_REPEAT
}

//
void sMouseButton(GLFWwindow*, int32 button, int32 action, int32 mods)
{
	double xd, yd;
	glfwGetCursorPos(mainWindow, &xd, &yd);
	b2Vec2 ps((float32)xd, (float32)yd);

	// Use the mouse to move things around.
	if (button == GLFW_MOUSE_BUTTON_1)
	{
        //<##>
        //ps.Set(0, 0);
		b2Vec2 pw = g_camera.ConvertScreenToWorld(ps);
		if (action == GLFW_PRESS)
		{
			if (mods == GLFW_MOD_SHIFT)
			{
				test->ShiftMouseDown(pw);
			}
			else
			{
				test->MouseDown(pw);
			}
		}
		
		if (action == GLFW_RELEASE)
		{
			test->MouseUp(pw);
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_2)
	{
		if (action == GLFW_PRESS)
		{	
			lastp = g_camera.ConvertScreenToWorld(ps);
			rightMouseDown = true;
		}

		if (action == GLFW_RELEASE)
		{
			rightMouseDown = false;
		}
	}
}

//
void sMouseMotion(GLFWwindow*, double xd, double yd)
{
	b2Vec2 ps((float)xd, (float)yd);

	b2Vec2 pw = g_camera.ConvertScreenToWorld(ps);
	test->MouseMove(pw);
	
	if (rightMouseDown)
	{
		b2Vec2 diff = pw - lastp;
		g_camera.m_center.x -= diff.x;
		g_camera.m_center.y -= diff.y;
		lastp = g_camera.ConvertScreenToWorld(ps);
	}
}

//
void sScrollCallback(GLFWwindow*, double, double dy)
{
	if (ui.mouseOverMenu)
	{
		ui.scroll = -int(dy);
	}
	else
	{
		if (dy > 0)
		{
			g_camera.m_zoom /= 1.1f;
		}
		else
		{
			g_camera.m_zoom *= 1.1f;
		}
	}
}

//
void sRestart()
{
	delete test;
//	entry = g_testEntries + testIndex;
	test = entry->createFcn(b2d_scripts[script_index].filepath);
}

//
void sSimulate()
{
	glEnable(GL_DEPTH_TEST);
	test->Step(&settings);

	test->DrawTitle(entry->name);
	glDisable(GL_DEPTH_TEST);

/*	if (testSelection != testIndex)
	{
		testIndex = testSelection;
		delete test;
		entry = g_testEntries + testIndex;
		test = entry->createFcn();
		g_camera.m_zoom = 1.0f;
		g_camera.m_center.Set(0.0f, 20.0f);
	}
	*/
}

//
void sInterface()
{
	int menuWidth = 200;
	ui.mouseOverMenu = false;
	if (ui.showMenu)
	{
		bool over = imguiBeginScrollArea("Testbed Controls", g_camera.m_width - menuWidth - 10, 10, menuWidth, g_camera.m_height - 20, &ui.scrollarea1);
		if (over) ui.mouseOverMenu = true;

		imguiSeparatorLine();

		imguiLabel("Script");
		if (imguiButton(entry->name, true))
		{
			ui.chooseTest = !ui.chooseTest;
		}

		imguiSeparatorLine();

		imguiSlider("Vel Iters", &settings.velocityIterations, 0, 50, 1, true);
		imguiSlider("Pos Iters", &settings.positionIterations, 0, 50, 1, true);
		imguiSlider("Hertz", &settings.hz, 5.0f, 120.0f, 5.0f, true);

		if (imguiCheck("Sleep", settings.enableSleep, true))
			settings.enableSleep = !settings.enableSleep;
		if (imguiCheck("Warm Starting", settings.enableWarmStarting, true))
			settings.enableWarmStarting = !settings.enableWarmStarting;
		if (imguiCheck("Time of Impact", settings.enableContinuous, true))
			settings.enableContinuous = !settings.enableContinuous;
		if (imguiCheck("Sub-Stepping", settings.enableSubStepping, true))
			settings.enableSubStepping = !settings.enableSubStepping;

		imguiSeparatorLine();

		if (imguiCheck("Shapes", settings.drawShapes, true))
			settings.drawShapes = !settings.drawShapes;
		if (imguiCheck("Joints", settings.drawJoints, true))
			settings.drawJoints = !settings.drawJoints;
		if (imguiCheck("AABBs", settings.drawAABBs, true))
			settings.drawAABBs = !settings.drawAABBs;
		if (imguiCheck("Contact Points", settings.drawContactPoints, true))
			settings.drawContactPoints = !settings.drawContactPoints;
		if (imguiCheck("Contact Normals", settings.drawContactNormals, true))
			settings.drawContactNormals = !settings.drawContactNormals;
		if (imguiCheck("Contact Impulses", settings.drawContactImpulse, true))
			settings.drawContactImpulse = !settings.drawContactImpulse;
		if (imguiCheck("Friction Impulses", settings.drawFrictionImpulse, true))
			settings.drawFrictionImpulse = !settings.drawFrictionImpulse;
		if (imguiCheck("Center of Masses", settings.drawCOMs, true))
			settings.drawCOMs = !settings.drawCOMs;
		if (imguiCheck("Statistics", settings.drawStats, true))
			settings.drawStats = !settings.drawStats;
		if (imguiCheck("Profile", settings.drawProfile, true))
			settings.drawProfile = !settings.drawProfile;

		if (imguiButton("Pause", true))
			settings.pause = !settings.pause;

		if (imguiButton("Single Step", true))
			settings.singleStep = !settings.singleStep;

		if (imguiButton("Restart", true))
			sRestart();

		if (imguiButton("Quit", true))
			glfwSetWindowShouldClose(mainWindow, GL_TRUE);

		imguiEndScrollArea();
	}

	int testMenuWidth = 200;
	if (ui.chooseTest)
	{
		static int testScroll = 0;
		bool over = imguiBeginScrollArea("Choose Script", g_camera.m_width - menuWidth - testMenuWidth - 20, 10, testMenuWidth, g_camera.m_height - 20, &testScroll);
		if (over) ui.mouseOverMenu = true;

//		for (int i = 0; i < testCount; ++i)
		size_t i = 0;
		for(auto const& s : b2d_scripts)
		{
			if (imguiItem(s.name.c_str(), true))
			{
				script_index = i;
				delete test;
				//entry = g_testEntries + i;
				test = entry->createFcn(b2d_scripts[script_index].filepath);
				ui.chooseTest = false;
			}

			++i;
		}

		imguiEndScrollArea();
	}

	imguiEndFrame();

}
