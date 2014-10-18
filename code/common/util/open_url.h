// open_url.h

#ifndef __WB_UTIL_OPEN_URL_H
#define __WB_UTIL_OPEN_URL_H

// TODO: Platform independent solution
#include <windows.h>
#include <shellapi.h>
//#include <string.h>

#include <string>


namespace detail {
	
	long GetRegKey(HKEY key, char *subkey, char *retdata)
	{
		long retval;
		HKEY hkey;

		retval = RegOpenKeyExA(key, subkey, 0, KEY_QUERY_VALUE, &hkey);

		if(retval == ERROR_SUCCESS)
		{
			long datasize = MAX_PATH;
			char data[MAX_PATH];

			RegQueryValueA(hkey, NULL, (LPSTR)data, &datasize);

			lstrcpyA(retdata, data);
			RegCloseKey(hkey);
		}

		return retval;
	}
	
}

bool open_url(std::string const& url, int showcmd)
{
	char key[MAX_PATH + MAX_PATH];
	BOOL retflag = FALSE;

	SHELLEXECUTEINFOA se_inf;
	ZeroMemory(&se_inf, sizeof(se_inf));
	se_inf.cbSize = sizeof(se_inf);
	se_inf.fMask = SEE_MASK_FLAG_NO_UI;
	se_inf.lpVerb = "open";
	// TODO: configurable by function arguments. also dependent on browser, currently just assuming chrome
	auto params = std::string{ "--new-window --app=" };
	params += url;
	se_inf.lpParameters = params.c_str();
	se_inf.lpFile = url.c_str();
	se_inf.nShow = showcmd;

	if(ShellExecuteExA(&se_inf) == FALSE)
	{
		/* get the .htm regkey and lookup the program */
		if(detail::GetRegKey(HKEY_CLASSES_ROOT, ".htm", key) == ERROR_SUCCESS)
		{
			lstrcatA(key, "\\shell\\open\\command");
			if(detail::GetRegKey(HKEY_CLASSES_ROOT, key, key) == ERROR_SUCCESS)
			{
				std::string path(key);
				std::string const stuff = " -- \"%1\"";
				auto pos = path.find(stuff);
				if(pos != std::string::npos)
				{
					path.erase(pos, stuff.length());
				}

				se_inf.lpFile = path.c_str();
				auto parameters = std::string{};
				// TODO: configurable by function arguments. also dependent on browser, currently just assuming chrome
				parameters += "--new-window ";
				auto quoted_url = '\"' + url + '\"';
				parameters += quoted_url;
				se_inf.lpParameters = parameters.c_str();
				retflag = ShellExecuteExA(&se_inf);
			}
		}
	}
	else
	{
		retflag = TRUE;
	}

	return retflag == TRUE;
}



#endif

