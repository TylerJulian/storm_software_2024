
#ifndef HELPER_H
#define HELPER_H

uint16_t interpolate_f2uint16(float val, float in_min, float in_max, uint16_t out_min, uint16_t out_max)
{
  uint16_t retval;
  float scale, offset;
  scale = (out_max - out_min) / (in_max - in_min); // 1000 / 2 = 500
  offset = out_min - in_min; // 1001

  retval = (uint16_t)((scale * (val - in_min)) + offset);

  return retval;
}

int16_t interpolate_f2int16(float val, float in_min, float in_max, int16_t out_min, int16_t out_max)
{
  int16_t retval;
  float scale, offset;
  scale = (out_max - out_min) / (in_max - in_min); // 1000 / 2 = 500
  offset = out_min - in_min; // 1001

  retval = (uint16_t)((scale * (val - in_min)) + offset);

  return retval;
}


#endif