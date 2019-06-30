#ifndef MASTEROPTIONS_CONFIGURATION_H
#define MASTEROPTIONS_CONFIGURATION_H

#include <libconfig.h++>
#include "EventHandler.h"

class Configuration
{
public:
    Configuration(const char* config_file);
    Configuration(std::map<uint16_t, ButtonAction*> c) : buttonActions (std::move(c)) {}
    std::map<uint16_t, ButtonAction*> buttonActions;
private:
    libconfig::Config cfg;
};

ButtonAction* parse_action(Action action, const libconfig::Setting* action_config, bool is_gesture=false);

Direction string_to_direction(std::string s);
GestureMode string_to_gesturemode(std::string s);
Action string_to_action(std::string s);

extern Configuration* config;

#endif //MASTEROPTIONS_CONFIGURATION_H
