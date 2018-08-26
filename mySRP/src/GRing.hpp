//
//  GRing.hpp
//  myRayTracing
//
//  Created by lizhen on 25/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#ifndef GRing_hpp
#define GRing_hpp

#include <stdio.h>
#include "GGeometry.hpp"

class GRing : public GGeometry {
 public:
  double radius_external;  // the external radius
  double radius_internal;  // the internal radius

  GVector center;  // the center

  // the vertices on the disc edge in counter clock wise order when looking down
  // from the normal at least 2 points
  std::vector<GVector> vertices;

  void computeBoundingBox();
  bool intersection(GRay &ray, GVector &intersection, gfc::GVector &normal);
};

#endif /* GRing_hpp */
