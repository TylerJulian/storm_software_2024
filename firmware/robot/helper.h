
#ifndef HELPER_H
#define HELPER_H

uint16_t interpolate_f2uint16(float val, float in_min, float in_max, uint16_t out_min, uint16_t out_max)
{
  uint16_t retval;
  float scale, offset;
  scale = (out_max - out_min) / (in_max - in_min); // 500 / 2 = 250
  offset = out_min - in_min; // 1500

  retval = (uint16_t)((scale * val) + offset);

  return retval;
}

#endif