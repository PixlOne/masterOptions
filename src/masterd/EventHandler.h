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

class Gesture
{
public:
    Gesture(std::vector<unsigned int> k, GestureMode m) : keys (k), mode (m) {}
    Gesture(std::vector<unsigned int> k, GestureMode m, int p) : keys (k), mode (m), per_pixel (p) {}

    void action();
private:
    std::vector<unsigned int> keys;
    GestureMode mode;
    int per_pixel;
};

enum class Action
{
    NoAction,
    Keypresses,
    Gestures,
    ToggleSmartshift,
    ToggleSmoothScroll,
    SwapDPI,
    ChangeDPI
};

class ButtonAction
{
public:
    Action type;
    void press() {};
    void move(HIDPP20::IReprogControlsV4::Move m) {};
    void release() {};
protected:
    ButtonAction(Action a) : type (a) {};
};

class NoAction : public ButtonAction
{
public:
    NoAction() : ButtonAction(Action::NoAction) {};
};

class KeyAction : public ButtonAction
{
public:
    KeyAction(std::vector<unsigned int> k) : ButtonAction(Action::Keypresses), keys (std::move(k)) {}
    virtual void press();
    virtual void release();
private:
    std::vector<unsigned int> keys;
};

class GestureAction : public ButtonAction
{
public:
    GestureAction(std::vector<Gesture> g) : ButtonAction(Action::Gestures), gestures (std::move(g)) {};
    std::vector<Gesture> gestures;
    virtual void press();
    virtual void move(HIDPP20::IReprogControlsV4::Move m);
    virtual void release();
private:
    bool held;
    int x;
    int y;
};

class SmartshiftAction : public ButtonAction
{
public:
    SmartshiftAction() : ButtonAction(Action::ToggleSmartshift) {}
    virtual void press();
};

class SmoothscrollAction : public ButtonAction
{
public:
    SmoothscrollAction() : ButtonAction(Action::ToggleSmoothScroll) {}
    virtual void press();
};

class SwapDPIAction : public ButtonAction
{
public:
    SwapDPIAction(int d1, int d2) : ButtonAction(Action::ChangeDPI) {}
    virtual void press();
    virtual void release();
private:
    int dpi1, dpi2;
};

class ChangeDPIAction : public ButtonAction
{
public:
    ChangeDPIAction(int i) : ButtonAction(Action::SwapDPI), dpi_inc (i) {}
    virtual void press();
    virtual void release();
private:
    int dpi_inc;
};

void press_button(uint16_t cid);
void release_button(uint16_t cid);
void move_diverted(uint16_t cid, HIDPP20::IReprogControlsV4::Move move);

Direction get_direction(int x, int y);

#endif //MASTEROPTIONS_EVENTHANDLER_H