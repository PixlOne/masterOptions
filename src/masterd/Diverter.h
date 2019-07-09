#ifndef MASTEROPTIONS_DIVERTER_H
#define MASTEROPTIONS_DIVERTER_H

#include "Configuration.h"

void apply_config(const device* d, bool scanning=false);

void divert_buttons(const device* d, bool scanning=false);

void set_smartshift(smartshift_options ops, const device* d, bool scanning=false);

void set_hiresscroll(bool b, const device* d, bool scanning=false);
void set_dpi(int dpi, const device* d, bool scanning=false);

#endif //MASTEROPTIONS_DIVERTER_H
