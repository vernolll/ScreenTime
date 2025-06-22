#include "ScreenTime.h"
#include <windows.h>

int main(int argc, char* argv[])
{
    ScreenTimeAPI screenTimeTracker("../screen_time.json");
    screenTimeTracker.trackScreenTime();
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    return main(__argc, __argv); 
}
