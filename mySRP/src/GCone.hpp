//
//  GCone.hpp
//  myRayTracing
//
//  Created by lizhen on 25/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#ifndef GCone_hpp
#define GCone_hpp

#include <stdio.h>
#include "GGeometry.hpp"
class GCone : public GGeometry {
 public:
  double depth;      // the distance from the bottom to the top vertex(can be
                     // virtual for the trancated cone)
  double height;     // the depth of the cone
  double radius_b;   // the radius of the bottom circle
  double radius_t;   // the radius of the top circle, if it not zeros, it its
                     // truncated
  GVector center_b;  // the center of the bottom circle
  GVector center_t;  // the center of the top circle, if it is not truncated, it
                     // is the vertex
  bool in_out;       // the inward pointing(true) or outward pointing(false), by
                // default it is false, this will determine the direction of
                // normal vector
  GCone() {
    depth = 0.0;
    height = 0.0;
    radius_b = 0.0;
    radius_t = 0.0;
    in_out = false;
  }

  void computeBoundingBox();
  bool intersection(GRay &ray, GVector &intersection, gfc::GVector &normal);
};

#endif /* GCone_hpp */
