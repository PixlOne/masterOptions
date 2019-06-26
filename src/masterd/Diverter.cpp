#include <hidpp20/IReprogControlsV4.h>
#include <cstdio>
#include <memory>
#include <unistd.h>
#include <hidpp/SimpleDispatcher.h>
#include <hidpp20/Error.h>
#include <linux/input-event-codes.h>

#include "EventHandler.h"
#include "Diverter.h"
#include "Configuration.h"

using namespace HIDPP20;

void divert_buttons(const char* path, HIDPP::DeviceIndex index)
{
    std::unique_ptr<HIDPP::Dispatcher> dispatcher;
    const int max_tries = 5;
    const int try_delay = 200000;
    for(int i = 0; i < max_tries; i++)
    {
        try
        {
            dispatcher = std::make_unique<HIDPP::SimpleDispatcher>(path);
            break;
        }
        catch (std::exception &e)
        {
            if(i >= max_tries - 1)
                fprintf(stderr, "Failed to open device: %s.\n", e.what());
            else usleep(try_delay);
        }
    }
    Device dev(dispatcher.get(), index);
    for(int i = 0; i < max_tries; i++)
    {
        try
        {
            IReprogControlsV4 irc4(&dev);
            for(auto it = config->buttonActions.begin(); it != config->buttonActions.end(); ++it)
            {
                uint8_t flags = 0;
                flags |= IReprogControlsV4::ChangeTemporaryDivert;
                flags |= IReprogControlsV4::TemporaryDiverted;
                if(it->second->type == Action::Gestures)
                {
                    flags |= IReprogControlsV4::ChangeRawXYDivert;
                    flags |= IReprogControlsV4::RawXYDiverted;
                }
                it->first;
                irc4.setControlReporting(it->first, flags, it->first);
            }
            break;
        }
        catch (Error &e)
        {
            if(i >= max_tries - 1)
                fprintf(stderr, "Could not divert buttons. Error code: %d: %s\n", e.errorCode(), e.what());
            else usleep(try_delay);
        }
    }
}