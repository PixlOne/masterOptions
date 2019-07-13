#include <hidpp/SimpleDispatcher.h>
#include <hidpp10/Error.h>
#include <future>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <condition_variable>

#include "Listener.h"
#include "DeviceHandler.h"
#include "Configuration.h"
#include "Diverter.h"
#include "Logger.h"

using namespace std::chrono_literals;

bool SCANNING_DEVICE = false;

void DeviceHandler::start()
{
    while(SCANNING_DEVICE) {}

    const device dev({path.c_str(), index});

    apply_config(&dev);

    ListenerThread listener(&dev);
    auto listener_future = std::async(std::launch::async, &ListenerThread::listen, &listener);

    HIDPP::SimpleDispatcher dispatcher(path.c_str());
    HIDPP20::Device d(&dispatcher, index);
    HIDPP20::IReprogControlsV4 irc4(&d);
    while(!DeviceRemoved)
    {
        std::mutex m;
        std::condition_variable cv;
        std::vector<uint8_t> results;

         std::thread t([&cv, &results, &d]()
         {
             try
             {
                 results = d.callFunction(0x00, 0x00);
             }
             catch(HIDPP10::Error &e)
             {
                 usleep(1000000);
             }
             cv.notify_one();
         });
         t.detach();

        std::unique_lock<std::mutex> l(m);
        if(cv.wait_for(l, 1s) == std::cv_status::timeout)
        {
            while(!DeviceRemoved)
            {
                try
                {
                    apply_config(&dev);
                    break;
                }
                catch(std::exception e) {}
            }
        }

        usleep(200000);
    }

    listener.stop();
    listener_future.wait();
}
