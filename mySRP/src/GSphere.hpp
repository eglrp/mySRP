//
//  GSphere.hpp
//  myRayTracing
//
//  Created by lizhen on 24/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#ifndef GSphere_hpp
#define GSphere_hpp

#include <stdio.h>
#include "GGeometry.hpp"

// the sphere geometry
class GSphere : public GGeometry {
 public:
  GVector centre;

  double radius;
  double radius_squared;
  void computeBoundingBox();
  bool intersection(GRay &ray, GVector &intersection, gfc::GVector &normal);
};

#endif /* GSphere_hpp */
