#include <hid/DeviceMonitor.h>
#include <hidpp/SimpleDispatcher.h>
#include <hidpp/DispatcherThread.h>
#include <hidpp20/Device.h>
#include <hidpp20/Error.h>
#include <hidpp20/IReprogControlsV4.h>
#include <hidpp20/UnsupportedFeature.h>
#include <algorithm>
#include <hidpp20/IFeatureSet.h>

#include "Configuration.h"
#include "Listener.h"

using namespace HIDPP20;

void ButtonHandler::handleEvent (const HIDPP::Report &event)
{
    switch (event.function())
    {
        case IReprogControlsV4::Event::DivertedButtonEvent:
        {
            new_states = IReprogControlsV4::divertedButtonEvent(event);
            if (states.empty())
            {
                for (uint16_t i : new_states)
                    std::thread{[=]()
                                { press_button(i, _dev); }}.detach();
                states = new_states;
                break;
            }
            std::vector<uint16_t>::iterator it;
            std::vector<uint16_t> cids(states.size() + new_states.size());
            it = std::set_union(states.begin(), states.end(), new_states.begin(), new_states.end(), cids.begin());
            cids.resize(it - cids.begin());
            for (uint16_t i : cids)
            {
                if (std::find(new_states.begin(), new_states.end(), i) != new_states.end())
                {
                    if (std::find(states.begin(), states.end(), i) == states.end())
                        std::thread{[=]()
                                    { press_button(i, _dev); }}.detach();
                } else
                    std::thread{[=]()
                                { release_button(i, _dev); }}.detach();
            }
            states = new_states;
            break;
        }
        case IReprogControlsV4::Event::DivertedRawXYEvent:
        {
            auto raw_xy = IReprogControlsV4::divertedRawXYEvent(event);

            for(uint16_t i : states)
                std::thread{[=]()
                    { move_diverted(i, raw_xy, _dev); }}.detach();
            break;
        }
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
    dispatcher->registerEventHandler(index, feature, [ptr](const HIDPP::Report &report)
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

void ListenerThread::listen()
{
    std::unique_ptr<HIDPP::Dispatcher> dispatcher;
    std::unique_ptr<HIDPP20::Device> HPP2dev;

    auto *d = new HIDPP::SimpleDispatcher(dev->path);
    listener = new SimpleListener(d, dev->index);

    HPP2dev = std::make_unique<HIDPP20::Device>(d, dev->index);
    dispatcher.reset (d);

    listener->addEventHandler( std::make_unique<ButtonHandler> (HPP2dev.get(), dev) );

    listener->start();
}

void ListenerThread::stop()
{
    listener->stop();
}

std::map<uint16_t, uint8_t> get_features(const char* path, HIDPP::DeviceIndex index)
{
    std::map<uint16_t, uint8_t> features;

    std::unique_ptr<HIDPP::Dispatcher> dispatcher = std::make_unique<HIDPP::SimpleDispatcher>(path);
    HIDPP20::Device dev(dispatcher.get(), index);

    HIDPP20::IFeatureSet ifs (&dev);

    unsigned int feature_count = ifs.getCount();

    for(unsigned int i = 1; i <= feature_count; i++)
        features.insert({ifs.getFeatureID(i), i});

    return features;
}