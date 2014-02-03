//
// Book:      OpenGL(R) ES 2.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com
//

// Hello_Triangle.c
//
//    This is a simple example that draws a single triangle with
//    a minimal vertex/fragment shader.  The purpose of this 
//    example is to demonstrate the basic concepts of 
//    OpenGL ES 2.0 rendering.

#include "draw_graph_app.h"

#include <tchar.h>


typedef struct
{
	draw_graph_app app;

} UserData;


void ext_input(ESContext* context, unsigned char c, int x, int y)
{
	UserData* ud = (UserData*)context->userData;

	ud->app.process_keyboard_input(c, x, y);
}

void ext_update(ESContext* context, float dtime)
{
	UserData* ud = (UserData*)context->userData;

	ud->app.update(context, dtime);
}

void ext_draw(ESContext* context)
{
	UserData* ud = (UserData*)context->userData;

	ud->app.draw(context);
}


int _tmain(int argc, TCHAR* argv[])
{
   ESContext esContext;
   UserData  userData;

   GLenum err = glGetError();

   esInitContext(&esContext);
   esContext.userData = &userData;

   esCreateWindow(&esContext, TEXT("Draw Graph"), draw_graph_app::SCREEN_WIDTH, draw_graph_app::SCREEN_HEIGHT,
	   ES_WINDOW_RGB | ES_WINDOW_DEPTH);
   
   if(!userData.app.initialise(&esContext))
   {
	   return 0;
   }

   esRegisterKeyFunc(&esContext, ext_input);
   esRegisterUpdateFunc(&esContext, ext_update);
   esRegisterDrawFunc(&esContext, ext_draw);
   
   esMainLoop(&esContext);
}
