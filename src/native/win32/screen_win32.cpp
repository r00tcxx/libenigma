#include <shellscalingapi.h>
#include "native/screen.h"
#include "win32.h"

#pragma comment(lib, "Shcore.lib")

namespace enigma::native::screen {

	UINT GetMonitorDPI(HMONITOR hMonitor) {
		UINT dpiX = 0;
		if (SUCCEEDED(GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, nullptr))) {
			return dpiX;
		}
		HDC hDC = GetDC(NULL);
		dpiX	= GetDeviceCaps(hDC, LOGPIXELSX);
		ReleaseDC(NULL, hDC);
		return dpiX;
	}

	BOOL CALLBACK monitor_enum_proc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
		MONITORINFOEX mi;
		mi.cbSize = sizeof(mi);
		if (!GetMonitorInfo(hMonitor, &mi)) return FALSE;
		auto& screens	   = *reinterpret_cast<hash_map<string, screen_info>*>(dwData);
		auto name		   = string::from_wstring(mi.szDevice);
		auto taskbar_side  = taskbar_side::unknown;
		u32 dpiX		   = GetMonitorDPI(hMonitor);
		u32 taskbar_height = 0;

		if (mi.rcWork.top > mi.rcMonitor.top) {
			taskbar_side   = taskbar_side::top;
			taskbar_height = mi.rcWork.top - mi.rcMonitor.top;
		}
		else if (mi.rcWork.bottom < mi.rcMonitor.bottom) {
			taskbar_side   = taskbar_side::bottom;
			taskbar_height = mi.rcMonitor.bottom - mi.rcWork.bottom;
		}
		else if (mi.rcWork.left > mi.rcMonitor.left) {
			taskbar_side   = taskbar_side::left;
			taskbar_height = mi.rcWork.left - mi.rcMonitor.left;
		}
		else if (mi.rcWork.right < mi.rcMonitor.right) {
			taskbar_side   = taskbar_side::right;
			taskbar_height = mi.rcMonitor.right - mi.rcWork.right;
		}
		else taskbar_side = taskbar_side::unknown;

		screens[name] = screen_info{
			.name				  = name,
			.x					  = mi.rcMonitor.left,
			.y					  = mi.rcMonitor.top,
			.width				  = mi.rcMonitor.right - mi.rcMonitor.left,
			.height				  = mi.rcMonitor.bottom - mi.rcMonitor.top,
			.dpi				  = dpiX,
			.primary			  = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0,
			.win32_taskbar_side	  = taskbar_side,
			.win32_taskbar_height = taskbar_height,
		};
		return TRUE;
	}

	hash_map<string, screen_info> get_screens() {
		hash_map<string, screen_info> screens;
		EnumDisplayMonitors(NULL, NULL, monitor_enum_proc, reinterpret_cast<LPARAM>(&screens));
		return screens;
	}
}  // namespace enigma::native::screen
