// gles_main.cpp

#include "proto_app.h"

#include "gles_angle/win32/esAppLoop.h"

#include <tchar.h>

int _tmain(int argc, TCHAR* argv[])
{
	app_loop< proto_app > loop;
	return loop.run() ? 0 : -1;
}

