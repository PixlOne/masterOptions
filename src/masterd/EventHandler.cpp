#include <cstdint>
#include <cstdio>
#include <memory>
#include <cmath>
#include <hidpp/SimpleDispatcher.h>
#include <hidpp20/Device.h>
#include <unistd.h>
#include <hidpp20/Error.h>

#include "EventHandler.h"
#include "Configuration.h"
#include "Logger.h"

#define PI 3.14159265

void press_button(uint16_t cid)
{
    auto action = config->buttonActions.find(cid)->second;
    switch(action->type)
    {
        case Action::None:
            ((NoAction*)action)->press();
            break;
        case Action::Keypress:
            ((KeyAction*)action)->press();
            break;
        case Action::Gestures:
            ((GestureAction*)action)->press();
            break;
        case Action::ToggleSmartshift:
            ((SmartshiftAction*)action)->press();
            break;
        case Action::ToggleSmoothScroll:
            ((SmoothscrollAction*)action)->press();
            break;
        case Action::SwapDPI:
            ((SwapDPIAction*)action)->press();
            break;
        case Action::ChangeDPI:
            ((ChangeDPIAction*)action)->press();
            break;
    }
}

void release_button(uint16_t cid)
{
    auto action = config->buttonActions.find(cid)->second;
    switch(action->type)
    {
        case Action::None:
            ((NoAction*)action)->release();
            break;
        case Action::Keypress:
            ((KeyAction*)action)->release();
            break;
        case Action::Gestures:
            ((GestureAction*)action)->release();
            break;
        case Action::ToggleSmartshift:
            ((SmartshiftAction*)action)->release();
            break;
        case Action::ToggleSmoothScroll:
            ((SmoothscrollAction*)action)->release();
            break;
        case Action::SwapDPI:
            ((SwapDPIAction*)action)->release();
            break;
        case Action::ChangeDPI:
            ((ChangeDPIAction*)action)->release();
            break;
    }
}

void move_diverted(uint16_t cid, HIDPP20::IReprogControlsV4::Move m)
{
    auto action = config->buttonActions.find(cid)->second;
    switch(action->type)
    {
        case Action::None:
            ((NoAction*)action)->move(m);
            break;
        case Action::Keypress:
            ((KeyAction*)action)->move(m);
            break;
        case Action::Gestures:
            ((GestureAction*)action)->move(m);
            break;
        case Action::ToggleSmartshift:
            ((SmartshiftAction*)action)->move(m);
            break;
        case Action::ToggleSmoothScroll:
            ((SmoothscrollAction*)action)->move(m);
            break;
        case Action::SwapDPI:
            ((SwapDPIAction*)action)->move(m);
            break;
        case Action::ChangeDPI:
            ((ChangeDPIAction*)action)->move(m);
            break;
    }
}

void KeyAction::press()
{
    //KeyPress event for each in keys
    for(unsigned int i : keys)
        log_printf(DEBUG, "%d pressed.", i);
}

void KeyAction::release()
{
    //KeyRelease event for each in keys
    for(unsigned int i : keys)
        log_printf(DEBUG, "%d released.", i);
}

void GestureAction::press()
{
    held = true;
    x = 0;
    y = 0;
}

void GestureAction::move(HIDPP20::IReprogControlsV4::Move m)
{
    x += m.x;
    y += m.y;
}

void GestureAction::release()
{
    held = false;

    std::string direction;

    auto d = get_direction(x, y);

    if(d == Direction::Down) direction = "down";
    else if(d == Direction::Up) direction = "up";
    else if(d == Direction::Left) direction = "left";
    else if(d == Direction::Right) direction = "right";
    else if(d == Direction::None) direction = "nowhere";

    log_printf(DEBUG, "Gesture button released, moved %s (%d, %d).", direction.c_str(), x, y);
}

Direction get_direction(int x, int y)
{
    if(abs(x) < 50 && abs(y) < 50) return Direction::None;

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

    if(315 < angle || angle <= 45) return Direction::Right;
    else if(45 < angle && angle <= 135) return Direction::Down;
    else if(135 < angle && angle <= 225) return Direction::Left;
    else if(225 < angle && angle <= 315) return Direction::Up;

    return Direction::None;
}