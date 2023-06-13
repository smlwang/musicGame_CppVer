#pragma onc
#include <windows.h>

struct mainViewInfo {
    const COLORREF default_color = RGB(255, 255, 255);
    struct helper {
        RECT rect = {150, 50, 700, 500};
    } helper;
    struct song {
        const int width = 800;
        const int high = 50;
        const int x = 30, y = 400;
        RECT midRect = {x, y, x + width, y + high};
        int calY(int no) { return y + high * no; }
    } song;
    struct highlight {
        COLORREF color = RGB(255, 255, 0);
    } highlight;
} minfo;