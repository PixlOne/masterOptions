#ifndef MASTEROPTIONS_LISTENER_H
#define MASTEROPTIONS_LISTENER_H

#include <hidpp20/FeatureInterface.h>
#include <hidpp20/IReprogControlsV4.h>
#include "EventHandler.h"

class EventHandler
{
public:
    virtual const HIDPP20::FeatureInterface *feature() const = 0;
    virtual void handleEvent (const HIDPP::Report &event) = 0;
};

class ButtonHandler : public EventHandler
{
public:
    ButtonHandler (HIDPP20::Device *dev, const device* d):
            _irc4 (dev),
            _dev (d),
            states (0) {}
    const HIDPP20::FeatureInterface *feature () const
    {
        return &_irc4;
    }
    void handleEvent (const HIDPP::Report &event);
protected:
    HIDPP20::IReprogControlsV4 _irc4;
    std::vector<uint16_t> states;
    std::vector<uint16_t> new_states;
    const device* _dev;
    std::future<void> current_future;
};

class EventListener
{
    HIDPP::Dispatcher *dispatcher;
    HIDPP::DeviceIndex index;
    std::map<uint8_t, std::unique_ptr<EventHandler>> handlers;
    std::map<uint8_t, HIDPP::Dispatcher::listener_iterator> iterators;
public:
    EventListener (HIDPP::Dispatcher *dispatcher, HIDPP::DeviceIndex index):
            dispatcher (dispatcher),
            index (index)
    {
    }

    virtual void removeEventHandlers ();
    virtual ~EventListener();
    virtual void addEventHandler (std::unique_ptr<EventHandler> &&handler);

    virtual void start () = 0;
    virtual void stop () = 0;

protected:
    virtual bool event (EventHandler* handler, const HIDPP::Report &report) = 0;
};

class SimpleListener : public EventListener
{
    HIDPP::SimpleDispatcher *dispatcher;

public:
    SimpleListener (HIDPP::SimpleDispatcher* dispatcher, HIDPP::DeviceIndex index):
            EventListener (dispatcher, index),
            dispatcher (dispatcher)
    {
    }

    virtual void start();
    virtual void stop();

protected:
    virtual bool event (EventHandler* handler, const HIDPP::Report &report);
};

class ListenerThread
{
public:
    ListenerThread (const device* d): dev (d) {};
    void stop();
    void listen();
protected:
    const device* dev;
    EventListener* listener;
};

std::map<uint16_t, uint8_t> get_features(const char* path, HIDPP::DeviceIndex index);

#endif //MASTEROPTIONS_LISTENER_H