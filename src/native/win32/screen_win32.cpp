#include <shellscalingapi.h>
#include "native/screen.h"
#include "string/str.h"
#include "win32.h"
#pragma comment(lib, "Shcore.lib")

namespace ema::native::screen {
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

	BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
		MONITORINFOEX mi;
		mi.cbSize = sizeof(mi);
		if (!GetMonitorInfo(hMonitor, &mi)) return FALSE;
		auto& screens				= *reinterpret_cast<std::unordered_map<std::string, ScreenInfo>*>(dwData);
		auto name					= String::from_wstring(mi.szDevice);
		auto taskbar_side			= TaskbarSide::Unknown;
		unsigned int dpiX			= GetMonitorDPI(hMonitor);
		unsigned int taskbar_height = 0;

		if (mi.rcWork.top > mi.rcMonitor.top) {
			taskbar_side   = TaskbarSide::Top;
			taskbar_height = mi.rcWork.top - mi.rcMonitor.top;
		}
		else if (mi.rcWork.bottom < mi.rcMonitor.bottom) {
			taskbar_side   = TaskbarSide::Bottom;
			taskbar_height = mi.rcMonitor.bottom - mi.rcWork.bottom;
		}
		else if (mi.rcWork.left > mi.rcMonitor.left) {
			taskbar_side   = TaskbarSide::Left;
			taskbar_height = mi.rcWork.left - mi.rcMonitor.left;
		}
		else if (mi.rcWork.right < mi.rcMonitor.right) {
			taskbar_side   = TaskbarSide::Right;
			taskbar_height = mi.rcMonitor.right - mi.rcWork.right;
		}
		else taskbar_side = TaskbarSide::Unknown;

		screens[name] = ScreenInfo{
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

	std::unordered_map<std::string, ScreenInfo> GetScreens() {
		std::unordered_map<std::string, ScreenInfo> screens;
		EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, reinterpret_cast<LPARAM>(&screens));
		return screens;
	}
}  // namespace ema::native::screen
