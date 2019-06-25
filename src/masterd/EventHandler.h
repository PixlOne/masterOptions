#ifndef MASTEROPTIONS_EVENTHANDLER_H
#define MASTEROPTIONS_EVENTHANDLER_H

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
};

void press_button(uint16_t cid);
void release_button(uint16_t cid);
void move_diverted(HIDPP20::IReprogControlsV4::Move move);

Direction get_direction(int x, int y);

#endif //MASTEROPTIONS_EVENTHANDLER_H
