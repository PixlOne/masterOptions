#include <hid/DeviceMonitor.h>
#include <hidpp/SimpleDispatcher.h>
#include <hidpp/DispatcherThread.h>
#include <hidpp20/Device.h>
#include <hidpp20/Error.h>
#include <hidpp20/IReprogControlsV4.h>
#include <hidpp20/UnsupportedFeature.h>
#include <algorithm>

#include "DeviceFinder.h"
#include "Listener.h"

using namespace HIDPP20;

void ButtonHandler::handleEvent (const HIDPP::Report &event)
{
    switch (event.function())
    {
        case IReprogControlsV4::Event::DivertedButtonEvent:
            new_states = IReprogControlsV4::divertedButtonEvent(event);
            if(states.empty())
            {
                for(uint16_t i : new_states)
                    printf("%x pressed\n", i);
                states = new_states;
                break;
            }
            std::vector<uint16_t>::iterator it;
            std::vector<uint16_t> cids(states.size() + new_states.size());
            it = std::set_union( states.begin(), states.end(), new_states.begin(), new_states.end(), cids.begin());
            cids.resize(it-cids.begin());
            for(uint16_t i : cids)
            {
                if(std::find(new_states.begin(), new_states.end(), i) != new_states.end())
                {
                    if(std::find(states.begin(), states.end(), i) == states.end())
                    {
                        printf("%x pressed\n", i);
                    }
                }
                else
                {
                    printf("%x released\n", i);
                }
            }
            states = new_states;
            break;
    }
}

void EventListener::removeEventHandlers ()
{
    for (const auto &p: iterators)
        dispatcher->unregisterEventHandler(p.second);
    handlers.clear();
    iterators.clear();
}

EventListener::~EventListener()
{
    removeEventHandlers();
}

void EventListener::addEventHandler (std::unique_ptr<EventHandler> &&handler)
{
    uint8_t feature = handler->feature()->index();
    EventHandler *ptr = handler.get();
    handlers.emplace(feature, std::move(handler));
    auto it = dispatcher->registerEventHandler(index, feature, [ptr](const HIDPP::Report &report)
    {
        ptr->handleEvent(report);
        return true;
    });
}

void SimpleListener::start()
{
    dispatcher->listen();
}

void SimpleListener::stop()
{
    dispatcher->stop();
}

bool SimpleListener::event (EventHandler *handler, const HIDPP::Report &report)
{
    handler->handleEvent (report);
    return true;
}

void listen(const char* path, HIDPP::DeviceIndex index, EventListener** listener)
{
    std::unique_ptr<HIDPP::Dispatcher> dispatcher;
    std::unique_ptr<HIDPP20::Device> dev;

    auto *d = new HIDPP::SimpleDispatcher(path);
    *listener = new SimpleListener(d, index);
    dev = std::make_unique<HIDPP20::Device>(d, index);
    dispatcher.reset (d);

    (*listener)->addEventHandler( std::make_unique<ButtonHandler> (dev.get()));

    (*listener)->start();
}
