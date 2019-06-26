#include <cstdint>
#include <vector>
#include <map>
#include <linux/input-event-codes.h>

#include "EventHandler.h"
#include "Configuration.h"

const Configuration* config = new Configuration(
{
    {0xc3, new GestureAction({})},
    {0xc4, new KeyAction({KEY_A})}
});

Configuration::Configuration(const char *config_file)
{

}