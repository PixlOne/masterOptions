#include <hidpp20/IReprogControlsV4.h>
#include <cstdio>
#include <memory>
#include <hidpp/SimpleDispatcher.h>
#include <hidpp20/Error.h>

#include "Diverter.h"

using namespace HIDPP20;

void divert_buttons(const char* path, HIDPP::DeviceIndex index)
{
    std::unique_ptr<HIDPP::Dispatcher> dispatcher;
    try
    {
        dispatcher = std::make_unique<HIDPP::SimpleDispatcher>(path);
    }
    catch (std::exception &e)
    {
        fprintf(stderr, "Failed to open device: %s.\n", e.what());
        std::exit(EXIT_FAILURE);
    }
    Device dev(dispatcher.get(), index);
    try
    {
        IReprogControlsV4 irc4(&dev);
        for(uint16_t cid : DIVERT_OPTIONS)
        {
            uint8_t flags = 0;
            flags |= IReprogControlsV4::ChangeTemporaryDivert;
            flags |= IReprogControlsV4::TemporaryDiverted;
            irc4.setControlReporting(cid, flags, cid);
        }
    }
    catch(Error &e)
    {
        fprintf(stderr, "Could not divert buttons. Error code: %d: %s\n", e.errorCode(), e.what());
        std::exit(e.errorCode());
    }
}