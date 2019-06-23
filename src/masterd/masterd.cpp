#include <hidpp/SimpleDispatcher.h>
#include <hidpp/DispatcherThread.h>
#include <hidpp20/Device.h>
#include <hidpp20/Error.h>
#include <hidpp20/IReprogControlsV4.h>
#include <hidpp20/UnsupportedFeature.h>
#include <hid/DeviceMonitor.h>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <csignal>

#include "DeviceFinder.h"
#include "Listener.h"
#include "Diverter.h"

EventListener* listener;

void sigint(int)
{
    listener->stop();
}

int main()
{
    device target;

    try
    {
        target = find_target_device();
    }
    catch (DeviceNotFoundException &e)
    {
        fprintf(stderr, "%s\n", e.what());
        return EXIT_FAILURE;
    }

    divert_buttons(target.path.c_str(), target.index);

    struct sigaction sa, oldsa;
    memset (&sa, 0, sizeof (struct sigaction));
    sa.sa_handler = sigint;
    sigaction (SIGINT, &sa, &oldsa);
    try
    {
        listen(target.path.c_str(), target.index, &listener);
    }
    catch (HIDPP20::UnsupportedFeature &e)
    {
        fprintf (stderr, "%s\n", e.what ());
        return EXIT_FAILURE;
    }
    catch (std::exception &e)
    {
        fprintf(stderr, "Initialisation failed: %s\n", e.what());
        delete listener;
        return EXIT_FAILURE;
    }

    listener->removeEventHandlers ();
    sigaction (SIGINT, &oldsa, nullptr);
    delete listener;

    return EXIT_SUCCESS;
}