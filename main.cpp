#include "ScreenTime.h"

int main() {
    ScreenTimeAPI screenTimeTracker("screen_time.json");
    screenTimeTracker.trackScreenTime();
    return 0;
}