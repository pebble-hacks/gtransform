#pragma once

#include <pebble.h>
#include "math_fixed.h"

#define GPOINT_PRECISE_MAX        0x2000 // 12 bit resolution
#define GPOINT_PRECISE_PRECISION  FIXED_S16_3_PRECISION

//! Internal respresentation of a point
//! 1 bit for sign, 12 bits represent the coordinate, 3 bits represent the precision
//! Supports -4096.000 px to 4095.875 px resolution
typedef struct __attribute__ ((__packed__)) GPointPrecise {
  //! The x-coordinate.
  Fixed_S16_3 x;
  //! The y-coordinate.
  Fixed_S16_3 y;
} GPointPrecise;

//! Convenience macro to make a GPointPrecise.
#define GPointPrecise(x, y) ((GPointPrecise){{(x)}, {(y)}})

//! Convenience macro to convert from GPoint to GPointPrecise.
#define GPointPreciseFromGPoint(point) \
        GPointPrecise((point.x % GPOINT_PRECISE_MAX) << GPOINT_PRECISE_PRECISION, \
                      (point.y % GPOINT_PRECISE_MAX) << GPOINT_PRECISE_PRECISION)

//! Convenience macro to convert from GPointPrecise to GPoint.
#define GPointFromGPointPrecise(pointP) \
        GPoint(pointP.x.raw_value >> GPOINT_PRECISE_PRECISION, \
               pointP.y.raw_value >> GPOINT_PRECISE_PRECISION)

//! Tests whether 2 precise points are equal.
//! @param pointP_a Pointer to the first precise point
//! @param pointP_b Pointer to the second precise point
//! @return `true` if both points are equal, `false` if not.
bool gpointprecise_equal(const GPointPrecise * const pointP_a,
                         const GPointPrecise * const pointP_b);

//! Represents a vector in a 2-dimensional coordinate system.
typedef struct GVector {
  //! The x-coordinate of the vector.
  int16_t dx;
  //! The y-coordinate of the vector.
  int16_t dy;
} GVector;

#define GVector(dx, dy) ((GVector){(dx), (dy)})

#define GVECTOR_PRECISE_MAX            GPOINT_PRECISE_MAX
#define GVECTOR_PRECISE_PRECISION      GPOINT_PRECISE_PRECISION

//! Represents a transformed vector in a 2-dimensional coordinate system.
typedef struct GVectorPrecise {
  //! The x-coordinate of the vector.
  Fixed_S16_3 dx;
  //! The y-coordinate of the vector.
  Fixed_S16_3 dy;
} GVectorPrecise;

//! Convenience macro to make a GVectorPrecise.
#define GVectorPrecise(dx, dy) ((GVectorPrecise){{(dx)}, {(dy)}})

//! Convenience macro to convert from GVector to GVectorPrecise.
#define GVectorPreciseFromGVector(vector) \
        GVectorPrecise((vector.dx % GVECTOR_PRECISE_MAX) << GVECTOR_PRECISE_PRECISION, \
                      (vector.dy % GVECTOR_PRECISE_MAX) << GVECTOR_PRECISE_PRECISION)

//! Convenience macro to convert from GVectorPrecise to GVector.
#define GVectorFromGVectorPrecise(vectorP) \
        GVector(vectorP.dx.raw_value >> GVECTOR_PRECISE_PRECISION, \
                vectorP.dy.raw_value >> GVECTOR_PRECISE_PRECISION)

//! Tests whether 2 precise vectors are equal.
//! @param vectorP_a Pointer to the first precise vector
//! @param vectorP_b Pointer to the second precise vector
//! @return `true` if both vectors are equal, `false` if not.
bool gvectorprecise_equal(const GVectorPrecise * const vectorP_a,
                          const GVectorPrecise * const vectorP_b);

//! @internal
//! Internal representation of a transformation matrix coefficient
typedef Fixed_S32_16 GTransformNumber;

//! @internal
//! Data structure that contains the internal representation of a 3x3 tranformation matrix
//! The transformation matrix will be expressed as follows:
//! [ a  b  0 ]
//! [ c  d  0 ]
//! [ tx ty 1 ]
//! However, internally we do not need to store the last row since we only support two
//! dimensions (x,y). Thus the last row is omitted from the internal storage.
//! Data values are in 16.16 fixed point representation
typedef struct __attribute__ ((__packed__)) GTransform {
  GTransformNumber a;
  GTransformNumber b;
  GTransformNumber c;
  GTransformNumber d;
  GTransformNumber tx;
  GTransformNumber ty;
} GTransform;
