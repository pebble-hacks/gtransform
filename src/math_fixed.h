#pragma once

#include <pebble.h>

#include <inttypes.h>
#include <stdbool.h>

////////////////////////////////////////////////////////////////
/// Fixed_S16_3 = 1 bit sign, 12 bits integer, 3 bits fraction
////////////////////////////////////////////////////////////////
// Note the fraction is unsigned and represents a positive addition
// to the integer. So for example:
// The value -1.125 will be stored as (-2 + 7*0.125) ==> integer = -2, fraction = 7
// The value 1.125 will be stored as (1 + 1*0.125) ==> integer = 1, fraction = 1
// This representation allows for direct addition/multiplication between numbers to happen
// without any complicated logic.
// The same representation for negative numbers applies for all fixed point representations
// in this file (i.e. fraction component is a positive addition to the integer).
typedef union __attribute__ ((__packed__)) Fixed_S16_3 {
  int16_t raw_value;
  struct {
    uint16_t fraction:3;
    int16_t integer:13;
  };
} Fixed_S16_3;

#define Fixed_S16_3(raw) ((Fixed_S16_3){ .raw_value = (raw) })
#define FIXED_S16_3_PRECISION 3

#define FIXED_S16_3_ZERO ((Fixed_S16_3){ .integer = 0, .fraction = 0 })
#define FIXED_S16_3_ONE ((Fixed_S16_3){ .integer = 1, .fraction = 0 })

static __inline__ Fixed_S16_3 Fixed_S16_3_add(Fixed_S16_3 a, Fixed_S16_3 b) {
  return Fixed_S16_3(a.raw_value + b.raw_value);
}

static __inline__ Fixed_S16_3 Fixed_S16_3_sub(Fixed_S16_3 a, Fixed_S16_3 b) {
  return Fixed_S16_3(a.raw_value - b.raw_value);
}

static __inline__ Fixed_S16_3 Fixed_S16_3_add3(Fixed_S16_3 a, Fixed_S16_3 b, Fixed_S16_3 c) {
  return Fixed_S16_3(a.raw_value + b.raw_value + c.raw_value);
}

static __inline__ bool Fixed_S16_3_equal(Fixed_S16_3 a, Fixed_S16_3 b) {
  return (a.raw_value == b.raw_value);
}

////////////////////////////////////////////////////////////////
/// Fixed_S32_16 = 1 bit sign, 15 bits integer, 16 bits fraction
////////////////////////////////////////////////////////////////
typedef union __attribute__ ((__packed__)) Fixed_S32_16 {
  int32_t raw_value;
  struct {
    uint16_t fraction:16;
    int16_t integer:16;
  };
} Fixed_S32_16;

#define Fixed_S32_16(raw) ((Fixed_S32_16){ .raw_value = (raw) })
#define FIXED_S32_16_PRECISION 16

#define FIXED_S32_16_ONE ((Fixed_S32_16){ .integer = 1, .fraction = 0 })

static __inline__ Fixed_S32_16 Fixed_S32_16_mul(Fixed_S32_16 a, Fixed_S32_16 b) {
  Fixed_S32_16 x;

  x.raw_value = (int32_t)((((int64_t) a.raw_value * (int64_t) b.raw_value)) >>
                FIXED_S32_16_PRECISION);
  return x;
}

static __inline__ Fixed_S32_16 Fixed_S32_16_add(Fixed_S32_16 a, Fixed_S32_16 b) {
  return Fixed_S32_16(a.raw_value + b.raw_value);
}

static __inline__ Fixed_S32_16 Fixed_S32_16_add3(Fixed_S32_16 a, Fixed_S32_16 b, Fixed_S32_16 c) {
  return Fixed_S32_16(a.raw_value + b.raw_value + c.raw_value);
}

////////////////////////////////////////////////////////////////
/// Mixed operations
////////////////////////////////////////////////////////////////
// This function muliples a Fixed_S16_3 and Fixed_S32_16 and returns result in Fixed_S16_3 format
static __inline__ Fixed_S16_3 Fixed_S16_3_S32_16_mul(Fixed_S16_3 a, Fixed_S32_16 b) {
  return Fixed_S16_3( a.raw_value * b.raw_value >> FIXED_S32_16_PRECISION );
}
