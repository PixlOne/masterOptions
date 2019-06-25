#include <cstdint>
#include <hidpp20/IReprogControlsV4.h>
#include <cmath>

#include "EventHandler.h"

#define PI 3.14159265

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
        std::string direction = "nowhere";

        auto d = get_direction(GESTURE_X, GESTURE_Y);

        if(d == DOWN) direction = "down";
        else if(d == UP) direction = "up";
        else if(d == LEFT) direction = "left";
        else if(d == RIGHT) direction = "right";

        printf("%x released, moved %s\n", cid, direction.c_str());
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

Direction get_direction(int x, int y)
{
    if(abs(x) < 50 && abs(y) < 50) return NONE;

    double angle;

    if(x == 0 && y > 0) angle = 90; // Y+
    else if(x == 0 && y < 0) angle = 270; // Y-
    else if(x > 0 && y == 0) angle = 0; // X+
    else if(x < 0 && y == 0) angle = 180; // X+
    else
    {
        angle = fabs(atan((double)y/(double)x) * 180.0 / PI);

        if(x < 0 && y > 0) angle = 180.0 - angle; //Q2
        else if(x < 0 && y < 0) angle += 180; // Q3
        else if(x > 0 && y < 0) angle = 360.0 - angle; // Q4
    }

    if(315 < angle || angle <= 45) return RIGHT;
    else if(45 < angle && angle <= 135) return DOWN;
    else if(135 < angle && angle <= 225) return LEFT;
    else if(225 < angle && angle <= 315) return UP;

    return NONE;
}