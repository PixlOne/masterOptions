#include <cstdint>
#include <hidpp20/IReprogControlsV4.h>

#include "EventHandler.h"

int16_t GESTURE_X;
int16_t GESTURE_Y;

void press_button(uint16_t cid)
{
    if(cid == 0xc3)
    {
        GESTURE_X = 0;
        GESTURE_Y = 0;
    }
    printf("%x pressed\n", cid);
}

void release_button(uint16_t cid)
{
    if(cid == 0xc3)
    {
        printf("%x released, moved (%d, %d) \n", cid, GESTURE_X, GESTURE_Y);
    }
    else
    {
        printf("%x pressed\n", cid);
    }
}

void move_diverted(HIDPP20::IReprogControlsV4::Move move)
{
    GESTURE_X += move.x;
    GESTURE_Y += move.y;
}