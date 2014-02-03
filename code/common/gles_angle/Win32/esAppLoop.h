// esAppLoop.h

#include <tchar.h>


template < typename AppType >
class app_loop
{
private:
	typedef AppType app_t;

	struct UserData
	{
		app_t app;
	};

public:
	bool run()
	{
		ESContext esContext;
		UserData  userData;

		esInitContext(&esContext);
		esContext.userData = &userData;

		esCreateWindow(&esContext, TEXT("TODO"), app_t::SCREEN_WIDTH, app_t::SCREEN_HEIGHT, ES_WINDOW_RGB | ES_WINDOW_DEPTH);

		if(!initialize_raw_input(esContext.hWnd))
		{
			return false;
		}

		if(!userData.app.initialise(&esContext))
		{
			return false;
		}

		esRegisterKeyFunc(&esContext, ext_key_input);
		esRegisterMouseMoveFunc(&esContext, ext_mouse_input);
		esRegisterUpdateFunc(&esContext, ext_update);
		esRegisterDrawFunc(&esContext, ext_draw);

		esMainLoop(&esContext);

		return true;
	}

	bool initialize_raw_input(HWND hWnd)
	{
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

		RAWINPUTDEVICE Rid[1];
		Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC; 
		Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE; 
		Rid[0].dwFlags = RIDEV_INPUTSINK;   
		Rid[0].hwndTarget = hWnd;
		RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

		return true;
	}

public:
	static void ext_key_input(ESContext* context, unsigned char c, int x, int y)
	{
		UserData* ud = (UserData*)context->userData;

		ud->app.process_keyboard_input(c, x, y);
	}

	static void ext_mouse_input(ESContext* context, int x, int y)
	{
		UserData* ud = (UserData*)context->userData;

		ud->app.process_mouse_movement(x, y);
	}

	static void ext_update(ESContext* context, float dtime)
	{
		UserData* ud = (UserData*)context->userData;

		ud->app.update(context, dtime);
	}
	
	static void ext_draw(ESContext* context)
	{
		UserData* ud = (UserData*)context->userData;

		ud->app.draw(context);
	}
};

