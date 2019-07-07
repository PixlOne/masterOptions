#ifndef MASTEROPTIONS_DIVERTER_H
#define MASTEROPTIONS_DIVERTER_H

void apply_config(const device* d);

void divert_buttons(const device* d);

void set_smartshift(bool b, const device* d);

void set_hiresscroll(bool b, const device* d);
void set_dpi(int dpi, const device* d);

#endif //MASTEROPTIONS_DIVERTER_H
