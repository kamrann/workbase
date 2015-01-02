// b2d_script_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "ScriptTest.h"
#include "callbacks.h"
#include "../b2d_script/parsing/object_parser.h"

#include "imgui.h"
#include "RenderGL3.h"
#include "DebugDraw.h"
#include "Test.h"

#include <glew/glew.h>
#include <glfw/glfw3.h>

#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <regex>

#ifdef _MSC_VER
#define snprintf _snprintf
#endif


namespace qi = boost::spirit::qi;
namespace fs = std::tr2::sys;


std::vector< script > b2d_scripts;

extern GLFWwindow* mainWindow;
extern UIState ui;
extern size_t script_index;
extern ScriptTestEntry* entry;
extern ScriptTest* test;


size_t find_scripts()
{
	b2d_scripts.clear();

	auto filter_str = std::string(R"(.*\.b2d)");
	std::regex rgx(filter_str);

	auto fs_path = fs::current_path< fs::path >();
	for(auto it = fs::directory_iterator(fs_path); it != fs::directory_iterator(); ++it)
	{
		auto const& file = it->path();

		if(!fs::is_directory(file) && std::regex_search(file.filename(), rgx))
		{
			b2d_scripts.push_back(script{ file.filename(), file.file_string() });
		}
	}

	return b2d_scripts.size();
}

int _tmain(int argc, _TCHAR* argv[])
{
	auto script_count = find_scripts();
	if(script_count == 0)
	{
		std::cout << "No box2d script files found" << std::endl;
		return -1;
	}

	g_camera.m_width = 1024;
	g_camera.m_height = 640;

	if(glfwInit() == 0)
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	char title[64];
	sprintf(title, "Box2D Script Test");

#if defined(__APPLE__)
	// Not sure why, but these settings cause glewInit below to crash.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

	mainWindow = glfwCreateWindow(g_camera.m_width, g_camera.m_height, title, NULL, NULL);
	if(mainWindow == NULL)
	{
		fprintf(stderr, "Failed to open GLFW mainWindow.\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(mainWindow);
	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

	glfwSetWindowSizeCallback(mainWindow, sResizeWindow);
	glfwSetKeyCallback(mainWindow, sKeyCallback);
	glfwSetMouseButtonCallback(mainWindow, sMouseButton);
	glfwSetCursorPosCallback(mainWindow, sMouseMotion);
	glfwSetScrollCallback(mainWindow, sScrollCallback);

#if defined(__APPLE__) == FALSE
	//glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if(GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}
#endif

	g_debugDraw.Create();

	sCreateUI();

	/*	testCount = 0;
	while (g_testEntries[testCount].createFcn != NULL)
	{
	++testCount;
	}

	testIndex = b2Clamp(testIndex, 0, testCount - 1);
	testSelection = testIndex;
	*/
	//entry = g_testEntries + testIndex;
	entry = new ScriptTestEntry{ "B2D Script Test", ScriptTest::Create };
	test = entry->createFcn(b2d_scripts[script_index].filepath);

	// Control the frame rate. One draw per monitor refresh.
	glfwSwapInterval(1);

	double time1 = glfwGetTime();
	double frameTime = 0.0;

	glClearColor(0.3f, 0.3f, 0.3f, 1.f);

	while(!glfwWindowShouldClose(mainWindow))
	{
		glfwGetWindowSize(mainWindow, &g_camera.m_width, &g_camera.m_height);
		glViewport(0, 0, g_camera.m_width, g_camera.m_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		unsigned char mousebutton = 0;
		int mscroll = ui.scroll;
		ui.scroll = 0;

		double xd, yd;
		glfwGetCursorPos(mainWindow, &xd, &yd);
		int mousex = int(xd);
		int mousey = int(yd);

		mousey = g_camera.m_height - mousey;
		int leftButton = glfwGetMouseButton(mainWindow, GLFW_MOUSE_BUTTON_LEFT);
		if(leftButton == GLFW_PRESS)
			mousebutton |= IMGUI_MBUT_LEFT;

		imguiBeginFrame(mousex, mousey, mousebutton, mscroll);

		sSimulate();
		sInterface();

		// Measure speed
		double time2 = glfwGetTime();
		double alpha = 0.9f;
		frameTime = alpha * frameTime + (1.0 - alpha) * (time2 - time1);
		time1 = time2;

		char buffer[32];
		snprintf(buffer, 32, "%.1f ms", 1000.0 * frameTime);
		AddGfxCmdText(5, 5, TEXT_ALIGN_LEFT, buffer, WHITE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		RenderGLFlush(g_camera.m_width, g_camera.m_height);

		glfwSwapBuffers(mainWindow);

		glfwPollEvents();
	}

	g_debugDraw.Destroy();
	RenderGLDestroy();
	glfwTerminate();



	/*
	b2ds::object_parser< std::string::const_iterator > p;

	auto fname = std::string{ argv[1] };

	std::ifstream file(fname);
	if(!file.is_open())
	{
		std::cout << "Failed to open file" << std::endl;
		return 0;
	}

	auto input = std::string{
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	};

	auto it = std::begin(input);
	b2ds::ast::object obj;
	auto success = qi::phrase_parse(it, std::end(input), p, qi::space_type{}, obj);
	success = success && it == std::end(input);

	if(!success)
	{
		std::cout << "Failed to parse b2d script" << std::endl;
	}
	*/

	return 0;
}

