#ifndef MASTEROPTIONS_EVENTHANDLER_H
#define MASTEROPTIONS_EVENTHANDLER_H

void press_button(uint16_t cid);
void release_button(uint16_t cid);
void move_diverted(HIDPP20::IReprogControlsV4::Move move);

#endif //MASTEROPTIONS_EVENTHANDLER_H
