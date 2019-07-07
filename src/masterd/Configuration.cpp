#include <cstdint>
#include <vector>
#include <map>
#include <linux/input-event-codes.h>
#include <libevdev/libevdev.h>
#include <libconfig.h++>
#include <algorithm>
#include <cstring>

#include "Logger.h"
#include "EventHandler.h"
#include "Configuration.h"

using namespace libconfig;

Configuration::Configuration(const char *config_file)
{
    try
    {
        cfg.readFile(config_file);
    }
    catch(const FileIOException &e)
    {
        log_printf(ERROR, "%s", "I/O Error while reading configuration file!");
        throw e;
    }
    catch(const ParseException &e)
    {
        log_printf(ERROR, "Parse error in %s, line %d: %s", e.getFile(), e.getLine(), e.getError());
        throw e;
    }

    const Setting &root = cfg.getRoot();

    Setting* buttons;
    try
    {
        buttons = &root["buttons"];
    }
    catch(const SettingNotFoundException &e)
    {
        log_printf(WARN, "No button configuration found, reverting to null config.");
        buttonActions = {};
        return;
    }

    for(int i = 0; i < buttons->getLength(); i++)
    {
        const Setting &button = (*buttons)[i];

        int cid;
        try { button.lookupValue("cid", cid); }
        catch(SettingNotFoundException &e)
        {
            log_printf(WARN, "Entry on line %d is missing a cid", button.getSourceLine());
            continue;
        }

        if(buttonActions.find(cid) != buttonActions.end())
        {
            log_printf(WARN, "Duplicate entries for cid 0x%x, skipping entry on line %d", cid, button.getSourceLine());
            continue;
        }

        Setting* action_config;
        try { action_config = &button["action"]; }
        catch(SettingNotFoundException &e)
        {
            log_printf(WARN, "cid 0x%x is missing an action, not diverting!", cid);
            continue;
        }

        Action action_type;
        try
        {
            std::string action_type_str;
            action_config->lookupValue("type", action_type_str);
            action_type = string_to_action(action_type_str);
        }
        catch(SettingNotFoundException &e)
        {
            log_printf(WARN, "cid 0x%x is missing an action type, not diverting!", cid);
            continue;
        }
        catch(std::invalid_argument &e)
        {
            log_printf(WARN, "Line %d: %s", (*action_config)["type"].getSourceLine(), e.what());
            continue;
        }

        try
        {
            buttonActions.insert({cid, parse_action(action_type, action_config)});
        }
        catch(std::exception &e)
        {
            log_printf(ERROR, "%s", e.what());
            continue;
        }
    }
}

ButtonAction* parse_action(Action type, const Setting* action_config, bool is_gesture)
{
    if(type == Action::None) return new NoAction();
    if(type == Action::Keypress)
    {
        std::vector<unsigned int> keys;
        try
        {
            const Setting &keys_config = (*action_config)["keys"];
            for (int i = 0; i < keys_config.getLength(); i++)
            {
                unsigned int keycode = libevdev_event_code_from_code_name(keys_config[i]);
                if(keycode == -1)
                {
                    const char* keyname = keys_config[i];
                    log_printf(WARN, "%s is not a valid keycode, skipping", keyname);
                }
                else keys.push_back(keycode);
            }
        }
        catch(SettingNotFoundException &e)
        {
            log_printf(WARN, "Expected keys parameter on line %d", action_config->getSourceLine());
        }

        return new KeyAction(keys);
    }
    else if(type == Action::Gestures)
    {
        std::map<Direction, Gesture*> gestures;

        Setting* gestures_config;

        try { gestures_config = &(*action_config)["gestures"]; }
        catch(SettingNotFoundException &e)
        {
            log_printf(WARN, "No gestures parameter for line %d, skipping", action_config->getSourceLine());
            throw e;
        }

        for(int i = 0; i < gestures_config->getLength(); i++)
        {
            const Setting &gesture_config = (*gestures_config)[i];

            std::string direction_str;
            Direction direction;
            try
            {
                gesture_config.lookupValue("direction", direction_str);
                direction = string_to_direction(direction_str);
            }
            catch(SettingNotFoundException &e)
            {
                log_printf(WARN, "No direction set on line %d", gesture_config.getSourceLine());
                continue;
            }
            catch(std::invalid_argument &e)
            {
                log_printf(WARN, "Line %d: %s", gesture_config["direction"].getSourceLine(), e.what());
                continue;
            }

            if(gestures.find(direction) != gestures.end())
            {
                log_printf(WARN, "Entry on line %d is a duplicate, skipping...", gesture_config["direction"].getSourceLine());
                continue;
            }

            GestureMode mode;
            try
            {
                std::string mode_str;
                gesture_config.lookupValue("mode", mode_str);

                mode = string_to_gesturemode(mode_str);
            }
            catch (SettingNotFoundException &e)
            {
                log_printf(INFO, "Gesture mode on line %d not found, defaulting to OnRelease", gesture_config.getSourceLine());
                mode = GestureMode::OnRelease;
            }

            if(mode == GestureMode::NoPress)
            {
                gestures.insert({direction, new Gesture(new NoAction(), mode)});
                continue;
            }

            Setting* g_action;
            try { g_action = &gesture_config["action"]; }
            catch(SettingNotFoundException &e)
            {
                log_printf(WARN, "No action set for %s", direction_str.c_str());
                continue;
            }

            Action g_type;
            try
            {
                std::string type_str;
                g_action->lookupValue("type", type_str);
                g_type = string_to_action(type_str);
            }
            catch(SettingNotFoundException &e)
            {
                log_printf(INFO, "Missing an action type on line %d, skipping", g_action->getSourceLine());
                continue;
            }
            catch(std::invalid_argument &e)
            {
                log_printf(WARN, "Line %d: %s", (*g_action)["type"].getSourceLine(), e.what());
                continue;
            }

            ButtonAction* ba;

            try { ba = parse_action(g_type, g_action, true); }
            catch(std::exception &e) { continue; }

            if(mode == GestureMode::OnFewPixels)
            {
                try
                {
                    int pp;
                    gesture_config.lookupValue("pixels", pp);
                    gestures.insert({direction, new Gesture(ba, mode, pp)});
                }
                catch(SettingNotFoundException &e)
                {
                    log_printf(WARN, "Line %d: OnFewPixels requires a 'pixels' field.", gesture_config.getSourceLine());
                }
            }
            else gestures.insert({direction, new Gesture(ba, mode)});
        }

        return new GestureAction(gestures);
    }
    else if(type == Action::ToggleSmartshift) return new SmartshiftAction();
    else if(type == Action::ToggleHiresScroll) return new HiresScrollAction();
    else if(type == Action::CycleDPI)
    {
        std::vector<int> dpis;
        try
        {
            const Setting &keys_config = (*action_config)["dpis"];
            for (int i = 0; i < keys_config.getLength(); i++)
                dpis.push_back(keys_config[i]);
        }
        catch(SettingNotFoundException &e)
        {
            log_printf(ERROR, "Line %d: CycleDPI action is missing 'dpis' field, defaulting to NoAction.", action_config->getSourceLine());
        }

        return new CycleDPIAction(dpis);
    }
    else if(type == Action::ChangeDPI)
    {
        int inc;
        try
        {
            action_config->lookupValue("inc", inc);
        }
        catch(SettingNotFoundException &e)
        {
            log_printf(ERROR, "Line %d: ChangeDPI action is missing an 'inc' field, defaulting to NoAction.",action_config->getSourceLine());
            return new NoAction();
        }

        return new ChangeDPIAction(inc);
    }

    log_printf(ERROR, "This shouldn't have happened. Unhandled action type? Defaulting to NoAction");
    return new NoAction();
}

Direction string_to_direction(std::string s)
{
    const char* original_str = s.c_str();
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);

    if(s == "none") return Direction::None;
    if(s == "up") return Direction::Up;
    if(s == "down") return Direction::Down;
    if(s == "left") return Direction::Left;
    if(s == "right") return Direction::Right;

    s = original_str;

    throw std::invalid_argument(s + " is an invalid direction.");
}

GestureMode string_to_gesturemode(std::string s)
{
    const char* original_str = s.c_str();
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);

    if(s == "nopress") return GestureMode::NoPress;
    if(s == "onrelease") return GestureMode::OnRelease;
    if(s == "onfewpixels") return GestureMode::OnFewPixels;

    s = original_str;

    log_printf(INFO, "%s is an invalid gesture mode. Defaulting to OnRelease", original_str);

    return GestureMode::OnRelease;
}

Action string_to_action(std::string s)
{
    std::string original_str = s;
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);

    if(s == "none") return Action::None;
    if(s == "keypress") return Action::Keypress;
    if(s == "gestures") return Action::Gestures;
    if(s == "togglesmartshift") return Action::ToggleSmartshift;
    if(s == "togglehiresscroll") return Action::ToggleHiresScroll;
    if(s == "cycledpi") return Action::CycleDPI;
    if(s == "changedpi") return Action::ChangeDPI;

    throw std::invalid_argument(original_str + " is an invalid action.");
}