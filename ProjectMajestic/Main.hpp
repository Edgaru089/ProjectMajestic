#pragma once

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <mutex>
#include <atomic>
#include <map>
#include <list>
#include <vector>
#include <queue>

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#ifdef SFML_SYSTEM_WINDOWS
#include <Windows.h>
#endif

#include "ImGui\imgui-setup.h"

#include "LogSystem.hpp"
#include "Uuid.hpp"
#include "Lockable.hpp"

#define USING_NAMESPACE \
using namespace std;\
using namespace sf;

USING_NAMESPACE;

//#define USE_DEFAULT_WINMAIN_ENTRYPOINT
//#define USE_ASYNC_RENDERING
//#define USE_DISCRETE_GPU

#define OUTPUT_LOG_TO_STDOUT
#define OUTPUT_LOG_TO_FILE
#define LOG_FILENAME "latest.log"
#define LOG_IGNORE_LEVEL -1

//Marcos & Typedefs
#define var auto
#define AUTOLOCK(a) lock_guard<mutex> lock(a)
#define AUTOLOCKTYPE(type, a) lock_guard<type> lock(a)
#define AUTOLOCKABLE(a) lock_guard<Lockable> lockable(a)
#define AUTOLOCKABLE_NAMED(a,name) lock_guard<Lockable> name(a)
typedef Vector2<double> Vector2d;
typedef sf::Rect<Uint32> UintRect;
typedef sf::Rect<double> DoubleRect;

//Constants
const int majorVersion = 0, minorVersion = 0, patchVersion = 0;
const string projectCode = "Project Majestic", projectSuffix = "Internal Release", releaseStage = "Alpha";
const string compileTime = string(__DATE__) + " " + string(__TIME__);

const double PI = 3.1415926535897932385;
const double eps = 1e-6;

//Resources
RenderWindow win;
#ifdef OUTPUT_LOG_TO_FILE
ofstream logout(LOG_FILENAME);
#endif // OUTPUT_LOG_TO_FILE

//Locks & Mutexs
mutex renderLock, logicDataLock;

//Global Variables
atomic_bool isProgramRunning;
int logicTickPerSecond, logicTickCounter, framePerSecond, frameCounter, eventTickPerSecond, eventTickCounter;
Clock logicTickCounterClock, frameCounterClock, eventTickCounterClock;
Clock programRunTimeClock;  //Nerer resets; started as time (for this process) begins
atomic_bool isReady;

Clock desktopUpdate;

//Utilities
const double sqr(double x) { return x * x; }

const double getDisSquared(double posX1, double posY1, double posX2, double posY2) {
	return abs(posX1 - posX2)*abs(posX1 - posX2) + abs(posY1 - posY2)*abs(posY1 - posY2);
}

const double getDisSquared(Vector2d posX, Vector2d posY) {
	return getDisSquared(posX.x, posX.y, posY.x, posY.y);
}

const double getDis(double posX1, double posY1, double posX2, double posY2) {
	return sqrt(getDisSquared(posX1, posY1, posX2, posY2));
}

const double getDis(Vector2d posX, Vector2d posY) {
	return getDis(posX.x, posX.y, posY.x, posY.y);
}

const bool isSame(double x, double y) {
	if (abs(x - y) < eps)
		return true;
	else
		return false;
}

const double getAngle(Vector2d begin, Vector2d end) {
	return atan((begin.y - end.y) / (begin.x - end.x)) / PI * 180.0;
}

template<class Type>
void resetVector2D(vector<vector<Type>>& vec, int Xsize, int Ysize, Type value) {
	vec.resize(Xsize);
	for (vector<Type>& i : vec) {
		i.resize(Ysize, value);
	}
}

template<typename Type1, typename Type2>
void waitUntilEqual(const Type1& val, const Type2 equal) {
	while (val != equal) sleep(milliseconds(2));
}

template<typename Type>
class Vector2Less {
public:
	const bool operator() (const Vector2<Type> x, const Vector2<Type> y) const {
		if (x.x < y.x)
			return true;
		else if (x.x == y.x&&x.y < y.y)
			return true;
		else
			return false;
	}
};

template<typename Type>
VertexArray renderRect(Rect<Type> rect, Color color = Color::White) {
	VertexArray arr;
	arr.append(Vertex(Vector2f(rect.left, rect.top), color));
	arr.append(Vertex(Vector2f(rect.left + rect.width, rect.top), color));
	arr.append(Vertex(Vector2f(rect.left + rect.width, rect.top + rect.height), color));
	arr.append(Vertex(Vector2f(rect.left, rect.top + rect.height), color));
	arr.append(Vertex(Vector2f(rect.left, rect.top), color));
	arr.setPrimitiveType(PrimitiveType::LinesStrip);
	return arr;
}

template<typename RectType, typename ValType>
Rect<RectType> operator * (Rect<RectType> rect, ValType val) {
	return Rect<RectType>(rect.left*val, rect.top*val, rect.width*val, rect.height*val);
}

double rand01() {
	return (double)rand() / (double)RAND_MAX;
}

// x % y
double modDouble(double x, double y) {
	int f = x / y;
	return x - f * y;
}

template<typename Type1, typename Type2>
Type2 min(const Type1& x, const Type2& y) { if (x < y)return x; else return y; }

template<typename Type1, typename Type2>
Type2 max(const Type1& x, const Type2& y) { if (x > y)return x; else return y; }

#define ENTER_IMGUI_DEBUG \
imgui::PushStyleColor(ImGuiCol_ChildBg, Color(0, 0, 0, 0));\
imgui::PushStyleColor(ImGuiCol_FrameBg, Color(0, 0, 0, 0));\
imgui::PushStyleColor(ImGuiCol_FrameBgActive, Color(0, 0, 0, 0));\
imgui::PushStyleColor(ImGuiCol_FrameBgHovered, Color(0, 0, 0, 0));\
imgui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);\
imgui::SetNextWindowPos(ImVec2(-1, -1), ImGuiCond_Always);\
imgui::SetNextWindowSize(Vector2f(win.getView().getSize()), ImGuiCond_Always);\
imgui::SetNextWindowBgAlpha(0);\
imgui::Begin("Debug", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|\
ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoScrollbar| ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoInputs)

#define LEAVE_IMGUI_DEBUG \
imgui::End();\
imgui::PopStyleColor(4);\
imgui::PopStyleVar()


// Platform-Depedent: Windows
#ifdef SFML_SYSTEM_WINDOWS

/*
const string getEnvironmentVariable(string name) {
	char buffer[2048];
	int size;
	size = GetEnvironmentVariable(name.c_str(), buffer, 2048);
	return size == 0 ? ""s : string(buffer);
}

void startProcess(string command, string workingDir = "") {
	LogMessage() << "Calling external runtime: " << command << dlog;

	if (workingDir == "") {
		size_t pos = command.find_last_of('\\');
		if (pos == string::npos)
			workingDir = "";
		else
			workingDir = command.substr(0, pos + 1);
	}

	mlog << "Runtime working directory: " << workingDir << dlog;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process.
	if (!CreateProcess(NULL,                  // No module name (use command line)
		const_cast<LPTSTR>(command.c_str()), // Command line
		NULL,                                // Process handle not inheritable
		NULL,                                // Thread handle not inheritable
		FALSE,                               // Set handle inheritance to FALSE
		DETACHED_PROCESS,                    // Detach console if possible
		NULL,                                // Use parent's environment block
		workingDir == "" ? NULL : workingDir.c_str(),
		&si,                                 // Pointer to STARTUPINFO structure
		&pi)                                 // Pointer to PROCESS_INFORMATION structure
		) {
		mlog << Log::Error << "CreateProcess failed (" << (int)GetLastError() << ")." << dlog;
		return;
	}

	// Close process and thread handles.
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

const float getHighDpiScaleFactor() {
	UINT dpi = 96;
	DPI_AWARENESS dpiAwareness = GetAwarenessFromDpiAwarenessContext(GetThreadDpiAwarenessContext());
	switch (dpiAwareness) {
		// Scale the window to the system DPI
	case DPI_AWARENESS_SYSTEM_AWARE:
		dpi = GetDpiForSystem();
		break;

		// Scale the window to the monitor DPI
	case DPI_AWARENESS_PER_MONITOR_AWARE:
		dpi = GetDpiForWindow(win.getSystemHandle());
		break;
	}

	return dpi / 96.0f;
}
*/

////////////////////////////////////////////////////////////
// Inform the Nvidia/AMD driver that this SFML application
// could benefit from using the more powerful discrete GPU
////////////////////////////////////////////////////////////
#ifdef USE_DISCRETE_GPU
extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif // USE_DISCRETE_GPU

extern int main(int argc, char* argv[]);

////////////////////////////////////////////////////////////
// Windows specific: we define the WinMain entry point,
// so that developers can use the standard main function
// even in a Win32 Application project, and thus keep a
// portable code
////////////////////////////////////////////////////////////
#ifdef USE_DEFAULT_WINMAIN_ENTRYPOINT
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT) {
	return main(__argc, __argv);
}
#endif // USE_DEFAULT_WINMAIN_ENTRYPOINT

#endif // SFML_SYSTEM_WINDOWS


#include "App.hpp"


App* app;
