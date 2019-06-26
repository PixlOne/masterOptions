#include <hidpp/SimpleDispatcher.h>
#include <future>
#include <unistd.h>

#include "Listener.h"
#include "DeviceHandler.h"
#include "Configuration.h"
#include "Diverter.h"

bool SCANNING_DEVICE = false;

void DeviceHandler::start()
{
    while(SCANNING_DEVICE) {}

    divert_buttons(path.c_str(), index);

    auto listener = new ListenerThread(path.c_str(), index);
    auto listener_future = std::async(std::launch::async, &ListenerThread::listen, listener);

    HIDPP::SimpleDispatcher dispatcher(path.c_str());
    HIDPP20::Device d(&dispatcher, index);
    while(!DeviceRemoved)
    {
        if(!config->buttonActions.empty())
        {
            while (SCANNING_DEVICE) {}
            HIDPP20::IReprogControlsV4 irc4(&d);
            uint8_t flags;
            irc4.getControlReporting(config->buttonActions.begin()->first, flags);
            if (!(flags & HIDPP20::IReprogControlsV4::TemporaryDiverted))
                divert_buttons(path.c_str(), index);
            usleep(20000);
        }
    }

    listener->stop();
    listener_future.wait();
}