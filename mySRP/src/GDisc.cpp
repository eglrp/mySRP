//
//  GDisc.cpp
//  myRayTracing
//
//  Created by lizhen on 24/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#include "GDisc.hpp"

void GDisc::computeBoundingBox() {
  // building the local coordinate system
  cs = center;
  cx = vertices[0] - center;
  cx.normalise();
  GVector a1 = vertices[1] - center;
  cz = crossproduct(cx, a1);  // make sure that z is at the normal of the circle
  cz.normalise();
  cy = crossproduct(cz, cx);
  cy.normalise();

  // calculating bounding box
  // the edge points on local coordinate system

  boundingBox.minLimit_l.set(-radius, -radius, 0.0);
  boundingBox.maxLimit_l.set(radius, radius, 0.0);
  GVector box[8];
  GVector pnt;
  boundingBox.generateLocalBox(box);

  // convert back the local bounding box limits to the BFS
  // convert the local box to BFS box and update limits to get the AABB in BFS
  // //convert back the local bounding box limits to the BFS
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

bool GDisc::intersection(GRay &ray, gfc::GVector &intersection,
                         gfc::GVector &normal) {
  bool ret = false;
  GVector x0 = center;
  GVector p = ray.start - x0;

  double ptn = dotproduct(p, cz);
  // Ntn is used to check the direction of ray, if it is from the back side of
  // the polygon
  double Ntn = dotproduct(ray.direction, cz);

  // assuming the normal of the this circle is at the z direction
  //    if( Ntn > 0.0)
  //    {
  //        return false;
  //    }

  if (fabs(Ntn) < GGeometry::EPS)  // ray is parallel to the plane
  {
    return false;
  }

  if (fabs(ptn) < GGeometry::EPS)  // ray starts from the plane
  {
    return false;
  }

  double t = -ptn / Ntn;

  if (t <= 0.0) {
    return false;
  }

  if (fabs(t) < GGeometry::EPS) {
    return false;
  }

  // the possible intersection
  GVector candidate = ray.start + t * ray.direction;

  // test if candidate is inside the disc
  double dis = (candidate - center).norm();
  if (dis < radius) {
    intersection = candidate;

    //       normal = cz;
    //
    // the ray is from the back side // the ray is from the back side, according
    // to marek's phd thesis, this ray should be rejected
    if (Ntn >= 0.0) {
      normal = -cz;
    } else  // ray is from the front side
    {
      normal = cz;
    }

    ret = true;
  }

  return ret;
}
