#include <cstdint>
#include <cstdio>
#include <memory>
#include <cmath>
#include <hidpp/SimpleDispatcher.h>
#include <hidpp20/Device.h>
#include <unistd.h>
#include <hidpp20/Error.h>
#include <linux/input-event-codes.h>
#include <hidpp20/IFeatureSet.h>
#include <hidpp20/IAdjustableDPI.h>

#include "EventHandler.h"
#include "Configuration.h"
#include "Logger.h"
#include "EvdevDevice.h"

#define PI 3.14159265

void press_button(uint16_t cid, const device* d)
{
    if(config->buttonActions.find(cid) == config->buttonActions.end())
    {
        log_printf(WARN, "0x%x was pressed but no action was found.", cid);
        return;
    }

    press_button(config->buttonActions.find(cid)->second, d);
}

void press_button(ButtonAction* action, const device* d)
{
    switch(action->type)
    {
        case Action::None:
            ((NoAction*)action)->press(d);
            break;
        case Action::Keypress:
            ((KeyAction*)action)->press(d);
            break;
        case Action::Gestures:
            ((GestureAction*)action)->press(d);
            break;
        case Action::ToggleSmartshift:
            ((SmartshiftAction*)action)->press(d);
            break;
        case Action::ToggleHiresScroll:
            ((HiresScrollAction*)action)->press(d);
            break;
        case Action::CycleDPI:
            ((CycleDPIAction*)action)->press(d);
            break;
        case Action::ChangeDPI:
            ((ChangeDPIAction*)action)->press(d);
            break;
    }
}

void release_button(uint16_t cid, const device* d)
{
    if(config->buttonActions.find(cid) == config->buttonActions.end())
    {
        log_printf(WARN, "0x%x was released but no action was found.", cid);
        return;
    }
    release_button(config->buttonActions.find(cid)->second, d);
}

void release_button(ButtonAction* action, const device* d)
{
    switch(action->type)
    {
        case Action::None:
            ((NoAction*)action)->release(d);
            break;
        case Action::Keypress:
            ((KeyAction*)action)->release(d);
            break;
        case Action::Gestures:
            ((GestureAction*)action)->release(d);
            break;
        case Action::ToggleSmartshift:
            ((SmartshiftAction*)action)->release(d);
            break;
        case Action::ToggleHiresScroll:
            ((HiresScrollAction*)action)->release(d);
            break;
        case Action::CycleDPI:
            ((CycleDPIAction*)action)->release(d);
            break;
        case Action::ChangeDPI:
            ((ChangeDPIAction*)action)->release(d);
            break;
    }
}

void move_diverted(uint16_t cid, HIDPP20::IReprogControlsV4::Move m, const device* d)
{
    auto action = config->buttonActions.find(cid)->second;
    switch(action->type)
    {
        case Action::Gestures:
            ((GestureAction*)action)->move(m, d);
            break;
        default:
            break;
    }
}

void KeyAction::press(const device* d)
{
    //KeyPress event for each in keys
    for(unsigned int i : keys)
        global_evdev->send_event(EV_KEY, i, 1);
}

void KeyAction::release(const device* d)
{
    //KeyRelease event for each in keys
    for(unsigned int i : keys)
        global_evdev->send_event(EV_KEY, i, 0);
}

void GestureAction::press(const device* d)
{
    held = true;
    x = 0;
    y = 0;
}

void GestureAction::move(HIDPP20::IReprogControlsV4::Move m, const device* d)
{
    x += m.x;
    y += m.y;
}

void GestureAction::release(const device* d)
{
    held = false;
    auto direction = get_direction(x, y);

    auto g = gestures.find(direction);

    if(g != gestures.end() && g->second->mode == GestureMode::OnRelease)
    {
        press_button(g->second->action, d);
        release_button(g->second->action, d);
    }
}

void SmartshiftAction::press(const device* d)
{
    std::unique_ptr<HIDPP::Dispatcher> dispatcher = std::make_unique<HIDPP::SimpleDispatcher>(d->path);
    HIDPP20::Device dev(dispatcher.get(), d->index);

    if(d->features.find(0x2110) == d->features.end())
    {
        log_printf(DEBUG, "Error with Smart shift action, feature is non-existent.");
        return;
    }
    const uint8_t f_index = d->features.find(0x2110)->second;
    std::vector<uint8_t> results;

    try
    {
        results = dev.callFunction(f_index, 0x00);
        if(results[0] == 0x02)
            results = dev.callFunction(f_index, 0x01, {0x01});
        else if(results[0] == 0x01)
            results = dev.callFunction(f_index, 0x01, {0x02});
        else
            results = dev.callFunction(f_index, 0x01, {0x01});
    }
    catch(HIDPP20::Error &e)
    {
        log_printf(ERROR, "Smart shift error, code %d: %s\n", e.errorCode(), e.what());
        return;
    }
}

void HiresScrollAction::press(const device* d)
{
    std::unique_ptr<HIDPP::Dispatcher> dispatcher = std::make_unique<HIDPP::SimpleDispatcher>(d->path);
    HIDPP20::Device dev(dispatcher.get(), d->index);

    if(d->features.find(0x2110) == d->features.end())
    {
        log_printf(DEBUG, "Error with Smooth scroll action, feature is non-existent.");
        return;
    }
    const uint8_t f_index = d->features.find(0x2121)->second;
    std::vector<uint8_t> results;

    try
    {
        results = dev.callFunction(f_index, 0x01);
        if(results[0] == 0x02)
            results = dev.callFunction(f_index, 0x02, {0x00});
        else if(results[0] == 0x00)
            results = dev.callFunction(f_index, 0x02, {0x02});
        else
            results = dev.callFunction(f_index, 0x02, {0x02});
    }
    catch(HIDPP20::Error &e)
    {
        log_printf(ERROR, "Smart shift error, code %d: %s\n", e.errorCode(), e.what());
        return;
    }
}

void CycleDPIAction::press(const device* d)
{
    std::unique_ptr<HIDPP::Dispatcher> dispatcher = std::make_unique<HIDPP::SimpleDispatcher>(d->path);
    HIDPP20::Device dev(dispatcher.get(), d->index);

    HIDPP20::IAdjustableDPI iad(&dev);

    try
    {
        for (int i = 0; i < iad.getSensorCount(); i++)
        {
            int current_dpi = std::get<0>(iad.getSensorDPI(i));
            bool found = false;
            for (int j = 0; j < dpis.size(); j++)
            {
                if (dpis[j] == current_dpi)
                {
                    if (j == dpis.size() - 1)
                        iad.setSensorDPI(i, dpis[0]);
                    else
                        iad.setSensorDPI(i, dpis[j + 1]);
                    found = true;
                    break;
                }
            }
            if (found) break;
            if (dpis.empty()) iad.setSensorDPI(i, std::get<1>(iad.getSensorDPI(i)));
            else iad.setSensorDPI(i, dpis[0]);
        }
    }
    catch(HIDPP20::Error &e)
    {
        log_printf(ERROR, "Error while cycling DPI: %s", e.what());
    }
}

void ChangeDPIAction::press(const device* d)
{
    std::unique_ptr<HIDPP::Dispatcher> dispatcher = std::make_unique<HIDPP::SimpleDispatcher>(d->path);
    HIDPP20::Device dev(dispatcher.get(), d->index);

    HIDPP20::IAdjustableDPI iad(&dev);

    try
    {
        for(int i = 0; i < iad.getSensorCount(); i++)
        {
            int current_dpi = std::get<0>(iad.getSensorDPI(i));
            iad.setSensorDPI(i, current_dpi + dpi_inc);
        }
    }
    catch(HIDPP20::Error &e)
    {
        log_printf(ERROR, "Error while incrementing DPI: %s", e.what());
    }
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