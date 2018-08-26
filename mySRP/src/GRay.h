//
//  GRay.h
//  myRayTracing
//
//  Created by lizhen on 28/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#ifndef GRay_h
#define GRay_h
#include <vector>
#include "GVector.hpp"
using namespace gfc;

// the ray class
class GRay {
 public:
  GVector start;

  GVector direction;  // direction is unit vector

  GVector inv_direction;  // the inverse of the direction = 1/direction, used at
                          // the intersection of AABB and ray

  // int sign[3];

  int reflectionNum;  // the reflectionNum, start from 0

  double energy;  // the energy of the ray, it is oringally 100%, it will be
                  // reduced after one reflection

  // the intersection routine
  std::vector<GVector> routine;

  // double length;

  GRay() {
    reflectionNum = 0;

    energy = 1.0;

    routine.reserve(10);

    // sign[0] = 0;
    // sign[1] = 0;
    // sign[2] = 0;

    // length = 0;
  }
};

class GOpticalProperty {
 public:
  // the default standard deviation of the optical properties
  // It is initilised in GGeometry,  GOpticalProperty::default_cov  =
  // 0.0001*0.0001;
  static double default_std_v;  // default std for reflectivity
  static double default_std_u;  // default std for the speculairy
  static double default_std_d;  // default std for other optical properties

  // for all the radiation spectrum
  double solar_absorptivity;
  double solar_absorptivity_cov;  // the uncertainty of optical property

  double solar_reflectivity;
  double solar_reflectivity_cov;

  double solar_specularity;
  double solar_specularity_cov;

  double solar_emissivity;
  double solar_emissivity_cov;

  // for the infrared radiation spectrum
  double ir_absorptivity;
  double ir_absorptivity_cov;

  double ir_reflectivity;
  double ir_reflectivity_cov;

  double ir_specularity;
  double ir_specularity_cov;

  double ir_emissivity;
  double ir_emissivity_cov;

  int mli_type;  // the type of the mli, 0= not mli, 1 for the first mli, 2 for
                 // the second mli and so forth
  GOpticalProperty() {
    solar_emissivity = 0.0;
    solar_specularity = 0.0;
    solar_absorptivity = 0.0;
    solar_reflectivity = 0.0;

    ir_emissivity = 0.0;
    ir_specularity = 0.0;
    ir_absorptivity = 0.0;
    ir_reflectivity = 0.0;

    solar_emissivity_cov = default_std_d * default_std_d;
    solar_specularity_cov = default_std_u * default_std_u;
    solar_absorptivity_cov = default_std_v * default_std_v;
    solar_reflectivity_cov = default_std_v * default_std_v;

    ir_emissivity_cov = default_std_d * default_std_d;
    ir_specularity_cov = default_std_u * default_std_u;
    ir_absorptivity_cov = default_std_v * default_std_v;
    ir_reflectivity_cov = default_std_v * default_std_v;

    mli_type = 0;
  }
};

#endif /* GRay_h */
