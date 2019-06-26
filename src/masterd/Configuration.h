#ifndef MASTEROPTIONS_CONFIGURATION_H
#define MASTEROPTIONS_CONFIGURATION_H

#include "EventHandler.h"

class Configuration
{
public:
    Configuration(const char* config_file);
    Configuration(std::map<uint16_t, ButtonAction*> c) : buttonActions (std::move(c)) {}
    std::map<uint16_t, ButtonAction*> buttonActions;
};

extern const Configuration* config;

#endif //MASTEROPTIONS_CONFIGURATION_H
