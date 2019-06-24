#include <hidpp20/IReprogControlsV4.h>
#include <cstdio>
#include <memory>
#include <unistd.h>
#include <hidpp/SimpleDispatcher.h>
#include <hidpp20/Error.h>

#include "Diverter.h"

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
            for (uint16_t cid : DIVERT_OPTIONS)
            {
                uint8_t flags = 0;
                flags |= IReprogControlsV4::ChangeTemporaryDivert;
                flags |= IReprogControlsV4::TemporaryDiverted;
                irc4.setControlReporting(cid, flags, cid);
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