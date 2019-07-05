#include <cstdint>
#include <cstdio>
#include <memory>
#include <cmath>
#include <hidpp/SimpleDispatcher.h>
#include <hidpp20/Device.h>
#include <unistd.h>
#include <hidpp20/Error.h>
#include <linux/input-event-codes.h>

#include "EventHandler.h"
#include "Configuration.h"
#include "Logger.h"
#include "EvdevDevice.h"

#define PI 3.14159265

void press_button(uint16_t cid)
{
    if(config->buttonActions.find(cid) == config->buttonActions.end())
    {
        log_printf(WARN, "0x%x was pressed but no action was found.", cid);
        return;
    }

    press_button(config->buttonActions.find(cid)->second);
}

void press_button(ButtonAction* action)
{
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
        case Action::CycleDPI:
            ((CycleDPIAction*)action)->press();
            break;
        case Action::ChangeDPI:
            ((ChangeDPIAction*)action)->press();
            break;
    }
}

void release_button(uint16_t cid)
{
    if(config->buttonActions.find(cid) == config->buttonActions.end())
    {
        log_printf(WARN, "0x%x was released but no action was found.", cid);
        return;
    }
    release_button(config->buttonActions.find(cid)->second);
}

void release_button(ButtonAction* action)
{
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
        case Action::CycleDPI:
            ((CycleDPIAction*)action)->release();
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
        case Action::Gestures:
            ((GestureAction*)action)->move(m);
            break;
        default:
            break;
    }
}

void KeyAction::press()
{
    //KeyPress event for each in keys
    for(unsigned int i : keys)
        global_evdev->send_event(EV_KEY, i, 1);
}

void KeyAction::release()
{
    //KeyRelease event for each in keys
    for(unsigned int i : keys)
        global_evdev->send_event(EV_KEY, i, 0);
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
    auto d = get_direction(x, y);

    auto g = gestures.find(d);

    if(g != gestures.end() && g->second->mode == GestureMode::OnRelease)
    {
        press_button(g->second->action);
        release_button(g->second->action);
    }
}

/*void SmartshiftAction::press()
{

}*/

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