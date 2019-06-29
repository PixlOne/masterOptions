#include <cstdint>
#include <vector>
#include <map>
#include <linux/input-event-codes.h>
#include <libconfig.h++>

#include "Logger.h"
#include "EventHandler.h"
#include "Configuration.h"

using namespace libconfig;

/*const Configuration* config = new Configuration(
{
    {0xc3, new GestureAction({})},
    {0xc4, new KeyAction({KEY_A})}
});*/

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

        ButtonAction* action;

        if(action_type == Action::NoAction) action = new NoAction();
        if(action_type == Action::Keypresses)
        {
            std::vector<unsigned int> keys;
            try
            {
                const Setting &keys_config = (*action_config)["keys"];
                for (int i = 0; i < keys_config.getLength(); i++)
                    keys.push_back(keys_config[i]);
            }
            catch(SettingNotFoundException &e)
            {
                log_printf(WARN, "No keys parameter for cid 0x%x!", cid);
            }

            action = new KeyAction(keys);
        }

        buttonActions.insert({cid, action});
    }
}

Action string_to_action(std::string s)
{
    if(s == "NoAction") return Action::NoAction;
    else if(s == "Keypresses") return Action::Keypresses;
    else if(s == "Gestures") return Action::Gestures;
    else if(s == "ToggleSmartshift") return Action::ToggleSmartshift;
    else if(s == "ToggleSmoothscroll") return Action::ToggleSmoothScroll;
    else if(s == "SwapDPI") return Action::SwapDPI;
    else if(s == "ChangeDPI") return Action::ChangeDPI;

    throw std::invalid_argument(s + " is an invalid action.");
}