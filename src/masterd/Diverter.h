#ifndef MASTEROPTIONS_DIVERTER_H
#define MASTEROPTIONS_DIVERTER_H

struct divert_option
{
    uint16_t cid;
    bool divert_button;
    bool divert_xy;
};

const divert_option DIVERT_OPTIONS[] ={
        {0xc3, true, true},
        {0xc4, true, false}
};

void divert_buttons(const char* path, HIDPP::DeviceIndex index);

#endif //MASTEROPTIONS_DIVERTER_H
