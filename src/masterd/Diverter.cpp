#include <hidpp20/IReprogControlsV4.h>
#include <cstdio>
#include <memory>
#include <unistd.h>
#include <hidpp/SimpleDispatcher.h>
#include <hidpp20/Error.h>
#include <linux/input-event-codes.h>
#include <hidpp20/IAdjustableDPI.h>

#include "EventHandler.h"
#include "Diverter.h"
#include "Logger.h"

using namespace HIDPP20;

void apply_config(const device* d, bool scanning)
{
    if(config->dpi != nullptr)
        set_dpi(*config->dpi, d, scanning);
    if(config->smartshift != nullptr)
        set_smartshift(*config->smartshift, d, scanning);
    if(config->hiresscroll != nullptr)
        set_hiresscroll(*config->hiresscroll, d, scanning);
    divert_buttons(d);
}

void divert_buttons(const device* d, bool scanning)
{
    std::unique_ptr<HIDPP::Dispatcher> dispatcher;
    try
    {
        dispatcher = std::make_unique<HIDPP::SimpleDispatcher>(d->path);
    }
    catch (std::exception &e)
    {
        if(scanning)
            throw e;
        log_printf(ERROR, "Failed to open device: %s", e.what());
    }
    Device dev(dispatcher.get(), d->index);
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
}

void set_smartshift(smartshift_options ops, const device* d, bool scanning)
{
    std::vector<uint8_t> parameters;
    parameters.push_back(ops.on == nullptr ? 0 : (*ops.on)? 2 : 1);
    if(ops.threshold != nullptr)
    {
        parameters.push_back(*ops.threshold);
        parameters.push_back(*ops.threshold);
    }

    std::unique_ptr<HIDPP::Dispatcher> dispatcher = std::make_unique<HIDPP::SimpleDispatcher>(d->path);
    HIDPP20::Device dev(dispatcher.get(), d->index);

    if(d->features.find(0x2110) == d->features.end())
    {
        log_printf(DEBUG, "Error toggling smart shift, feature is non-existent.");
        return;
    }
    const uint8_t f_index = d->features.find(0x2110)->second;

    try { dev.callFunction(f_index, 0x01, parameters); }
    catch (HIDPP20::Error &e)
    {
        if(scanning)
            throw e;
        log_printf(ERROR, "Error setting smartshift options, code %d: %s\n", e.errorCode(), e.what());
    }
}

void set_hiresscroll(bool b, const device* d, bool scanning)
{

}

void set_dpi(int dpi, const device* d, bool scanning)
{
    std::unique_ptr<HIDPP::Dispatcher> dispatcher = std::make_unique<HIDPP::SimpleDispatcher>(d->path);
    HIDPP20::Device dev(dispatcher.get(), d->index);

    HIDPP20::IAdjustableDPI iad(&dev);

    try { for(int i = 0; i < iad.getSensorCount(); i++) iad.setSensorDPI(i, dpi); }
    catch (HIDPP20::Error &e)
    {
        if(scanning)
            throw e;
        log_printf(ERROR, "Error while setting DPI: %s", e.what());
    }
}