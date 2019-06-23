#ifndef MASTEROPTIONS_DIVERTER_H
#define MASTEROPTIONS_DIVERTER_H

const uint16_t DIVERT_OPTIONS[] = {0xc3, 0xc4} ;

void divert_buttons(const char* path, HIDPP::DeviceIndex index);

#endif //MASTEROPTIONS_DIVERTER_H
