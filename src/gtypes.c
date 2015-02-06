#include <pebble.h>

#include "gtypes.h"

bool gpointprecise_equal(const GPointPrecise * const pointP_a,
                         const GPointPrecise * const pointP_b) {
  return ((pointP_a->x.raw_value == pointP_b->x.raw_value) &&
          (pointP_a->y.raw_value == pointP_b->y.raw_value));
}

bool gvectorprecise_equal(const GVectorPrecise * const vectorP_a,
                          const GVectorPrecise * const vectorP_b) {
  return ((vectorP_a->dx.raw_value == vectorP_b->dx.raw_value) &&
          (vectorP_a->dy.raw_value == vectorP_b->dy.raw_value));
}

