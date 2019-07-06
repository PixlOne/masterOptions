#ifndef MASTEROPTIONS_EVENTHANDLER_H
#define MASTEROPTIONS_EVENTHANDLER_H

#include <hidpp20/IReprogControlsV4.h>

enum class Direction
{
    None,
    Up,
    Down,
    Left,
    Right
};

enum class GestureMode
{
    NoPress,
    OnRelease,
    OnFewPixels
};

enum class KeyMode
{
    MapPressAndRelease,
    PressOnRelease
};

enum class Action
{
    None,
    Keypress,
    Gestures,
    ToggleSmartshift,
    ToggleHiresScroll,
    CycleDPI,
    ChangeDPI
};

std::map<uint16_t, uint8_t> get_features(const char* path, HIDPP::DeviceIndex index);

struct device
{
    const char* path;
    const HIDPP::DeviceIndex index;
    const std::map<uint16_t, uint8_t> features = get_features(path, index);
};

class ButtonAction
{
public:
    Action type;
    void press(const device* d) {};
    void release(const device* d) {};
protected:
    ButtonAction(Action a) : type (a) {};
};

class Gesture
{
public:
    Gesture(ButtonAction* ba, GestureMode m) : action (ba), mode (m) {}
    Gesture(ButtonAction* ba, GestureMode m, int pp) : action (ba), mode (m), per_pixel (pp)  {}

    ButtonAction* action;

    GestureMode mode;
    int per_pixel;
};

class NoAction : public ButtonAction
{
public:
    NoAction() : ButtonAction(Action::None) {};
};

class KeyAction : public ButtonAction
{
public:
    KeyAction(std::vector<unsigned int> k) : ButtonAction(Action::Keypress), keys (std::move(k)) {}
    virtual void press(const device* d);
    virtual void release(const device* d);
private:
    std::vector<unsigned int> keys;
};

class GestureAction : public ButtonAction
{
public:
    GestureAction(std::map<Direction, Gesture*> g) : ButtonAction(Action::Gestures), gestures (std::move(g)) {}
    std::map<Direction, Gesture*> gestures;
    virtual void press(const device* d);
    void move(HIDPP20::IReprogControlsV4::Move m, const device* d);
    virtual void release(const device* d);
private:
    bool held;
    int x;
    int y;
};

class SmartshiftAction : public ButtonAction
{
public:
    SmartshiftAction() : ButtonAction(Action::ToggleSmartshift) {}
    virtual void press(const device* d);
};

class HiresScrollAction : public ButtonAction
{
public:
    HiresScrollAction() : ButtonAction(Action::ToggleHiresScroll) {}
    virtual void press(const device* d);
};

class CycleDPIAction : public ButtonAction
{
public:
    CycleDPIAction(std::vector<int> d) : ButtonAction(Action::CycleDPI), dpis (d) {}
    virtual void press(const device* d);
private:
    const std::vector<int> dpis;
};

class ChangeDPIAction : public ButtonAction
{
public:
    ChangeDPIAction(int i) : ButtonAction(Action::ChangeDPI), dpi_inc (i) {}
    virtual void press(const device* d);
private:
    int dpi_inc;
};

void press_button(uint16_t cid, const device* d);
void press_button(ButtonAction* action, const device* d);
void release_button(uint16_t cid, const device* d);
void release_button(ButtonAction* action, const device* d);
void move_diverted(uint16_t cid, HIDPP20::IReprogControlsV4::Move move, const device* d);

Direction get_direction(int x, int y);

#endif //MASTEROPTIONS_EVENTHANDLER_H