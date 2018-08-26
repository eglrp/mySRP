//
//  GSphere.cpp
//  myRayTracing
//
//  Created by lizhen on 24/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#include "GSphere.hpp"

/*compute the bounding box of a sphere*/
void GSphere::computeBoundingBox() {
  // just use the same directions of BFS
  cs = centre;
  cx.set(1, 0, 0);
  cy.set(0, 1, 0);
  cz.set(0, 0, 1);

  // updating the bounding box in geometry local system
  boundingBox.minLimit_l.set(-radius, -radius, -radius);
  boundingBox.maxLimit_l.set(radius, radius, radius);

  GVector box[8];

  boundingBox.generateLocalBox(box);

  GVector pnt;
  for (int i = 0; i < 8; i++) {
    pnt.x = cx.x * box[i].x + cy.x * box[i].y + cz.x * box[i].z;
    pnt.y = cx.y * box[i].x + cy.y * box[i].y + cz.y * box[i].z;
    pnt.z = cx.z * box[i].x + cy.z * box[i].y + cz.z * box[i].z;

    pnt += cs;
    boundingBox.updateBoundingBoxLimits(pnt, boundingBox.minLimit,
                                        boundingBox.maxLimit);
  }
  boundingBox.computeCenter();
}

bool GSphere::intersection(GRay &ray, GVector &intersection,
                           gfc::GVector &normal) {
  GVector ret;
  double t = 0.0, t0 = 0, t1 = 0;

  //    GVector L = centre - ray.start;
  //    double tca = dotproduct(L, ray.direction);
  //    double d2 = dotproduct(L, L) - tca*tca;
  //    if(d2 > radius*radius) return false;
  //    double thc = sqrt(radius*radius - d2);
  //    t0 = tca - thc;
  //    t1 = tca + thc;
  //
  //    if (t0 > t1) std::swap(t0, t1);
  //
  //    if (t0 < 0)
  //    {
  //        t0 = t1; // if t0 is negative, let's use t1 instead
  //        if (t0 < 0) return false; // both t0 and t1 are negative
  //    }
  //
  //    t = t0;

  GVector p = ray.start - centre;
  double ptp = p.norm2();

  if (fabs(ptp - radius_squared) <
      GGeometry::EPS)  // the ray is just on this sphere
  {
    return false;
  }

  double dtd =
      ray.direction
          .norm2();  // should always be 1.0, since direction is unit vector
  double ptd =
      p.x * ray.direction.x + p.y * ray.direction.y + p.z * ray.direction.z;
  // form the quadratic equation
  double A = dtd, B = 2.0 * ptd, C = ptp - radius_squared;
  double D = B * B - 4.0 * A * C;

  if (D <= 0.0)  // no intersection, here including the tangent case
  {
    return false;
  }
  double tp1 = -B / (2.0 * A);
  double tp2 = sqrt(D) / (2.0 * A);

  t0 = tp1 + tp2;
  t1 = tp1 - tp2;

  // choosing the closest one, here t has to be greater than 0
  // if t1 <0 , that means it is in the opposite direction of the ray which
  // could happen when the ray start is inside a sphere
  //    if( t0*t1<0.0 ) // obtain the positive solution
  //    {
  //        t = t0>=0.0?t0:t1;
  //    }

  if (t0 >= t1) std::swap(t0, t1);
  if (t0 >= 0.0) {
    t = t0;
  } else {
    if (t1 >= 0.0) {
      t = t1;
    } else {
      return false;
    }
  }

  if (t < 0.0) {
    return false;
  }

  if (fabs(t) < GGeometry::EPS) {
    return false;
  }

  intersection.set(ray.start.x + t * ray.direction.x,
                   ray.start.y + t * ray.direction.y,
                   ray.start.z + t * ray.direction.z);
  // calculate normal
  normal = intersection - centre;
  normal.normalise();

  return true;
}
