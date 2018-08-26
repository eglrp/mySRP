//
//  GBoundingBox.cpp
//  myRayTracing
//
//  Created by lizhen on 28/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#include "GBoundingBox.hpp"
double GBoundingBox::infinite = 999999999;

GBoundingBox::GBoundingBox() {
  double limit = GBoundingBox::infinite;

  minLimit.set(limit, limit, limit);
  maxLimit.set(-limit, -limit, -limit);

  minLimit_l.set(limit, limit, limit);
  maxLimit_l.set(-limit, -limit, -limit);
}

void GBoundingBox::computeCenter() { center = (minLimit + maxLimit) / 2.0; }

// update min and max according to p
// so p is the input while min and max are outputs
void GBoundingBox::updateBoundingBoxLimits(gfc::GVector& p, gfc::GVector& min,
                                           gfc::GVector& max) {
  if (p.x <= min.x) {
    min.x = p.x;
  }
  if (p.y <= min.y) {
    min.y = p.y;
  }
  if (p.z <= min.z) {
    min.z = p.z;
  }

  if (p.x >= max.x) {
    max.x = p.x;
  }

  if (p.y >= max.y) {
    max.y = p.y;
  }

  if (p.z >= max.z) {
    max.z = p.z;
  }
}

// generate 8 vertices of AABB box in local system
void GBoundingBox::generateLocalBox(gfc::GVector box[8]) {
  // point1
  box[0].x = maxLimit_l.x;
  box[0].y = minLimit_l.y;
  box[0].z = minLimit_l.z;

  // point2
  box[1].x = maxLimit_l.x;
  box[1].y = maxLimit_l.y;
  box[1].z = minLimit_l.z;

  // point3
  box[2].x = minLimit_l.x;
  box[2].y = maxLimit_l.y;
  box[2].z = minLimit_l.z;

  // point4
  box[3].x = minLimit_l.x;
  box[3].y = minLimit_l.y;
  box[3].z = minLimit_l.z;

  // point5
  box[4].x = maxLimit_l.x;
  box[4].y = minLimit_l.y;
  box[4].z = maxLimit_l.z;

  // point6
  box[5].x = maxLimit_l.x;
  box[5].y = maxLimit_l.y;
  box[5].z = maxLimit_l.z;

  // point7
  box[6].x = minLimit_l.x;
  box[6].y = maxLimit_l.y;
  box[6].z = maxLimit_l.z;

  // point8
  box[7].x = minLimit_l.x;
  box[7].y = minLimit_l.y;
  box[7].z = maxLimit_l.z;
}

// check whether the ray can hit this aabb bounding box
// ref
// http://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
//  http://blog.csdn.net/i_dovelemon/article/details/38342739

// this function is the key point to make ray tracer run fast

bool GBoundingBox::intersect(GRay& ray, double& dis) {
  //    double eps = 1.0E-12;
  //    double tmin = 0.0;
  //    double tmax = GBoundingBox::infinite ;
  //
  //    //The plane perpendicular to x-axie
  //    if( fabs(ray.direction.x) <= eps ) //If the ray parallel to the plane
  //    {
  //        //If the ray is not within AABB box, then not intersecting
  //        if( ray.start.x < minLimit.x || ray.start.x > maxLimit.x )
  //            return false ;
  //    }
  //    else
  //    {
  //        //Compute the distance of ray to the near plane and far plane
  //        //float ood = 1.0f / dir.x ;
  //        double t1 = (minLimit.x - ray.start.x) * ray.inv_direction.x ;
  //        double t2 = (maxLimit.x - ray.start.x) * ray.inv_direction.x ;
  //
  //        //Make t1 be intersecting with the near plane, t2 with the far plane
  //        if(t1 >= t2)
  //        {
  //            double temp = t1 ;
  //            t1 = t2 ;
  //            t2 = temp ;
  //        }
  //
  //        //Compute the intersection of slab intersection intervals
  //        if(t1 >= tmin) tmin = t1 ;
  //        if(t2 <= tmax) tmax = t2 ;
  //
  //        //Exit with no collision as soon as slab intersection becomes empty
  //        if(tmin > tmax) return false ;
  //    }// end for perpendicular to x-axie
  //
  //    tmin = 0.0 ;
  //    tmax = GBoundingBox::infinite ;
  //    //The plane perpendicular to y-axie
  //    if(fabs(ray.direction.y) <= eps) //If the ray parallel to the plane
  //    {
  //        //If the ray is not within AABB box, then not intersecting
  //        if(ray.start.y < minLimit.y || ray.start.y > maxLimit.y)
  //            return false ;
  //    }
  //    else
  //    {
  //        //Compute the distance of ray to the near plane and far plane
  //        //float ood = 1.0f / dir.y ;
  //        double t1 = (minLimit.y - ray.start.y) * ray.inv_direction.y ;
  //        double t2 = (maxLimit.y - ray.start.y) * ray.inv_direction.y ;
  //
  //        //Make t1 be intersecting with the near plane, t2 with the far plane
  //        if(t1 >= t2)
  //        {
  //            double temp = t1 ;
  //            t1 = t2 ;
  //            t2 = temp ;
  //        }
  //
  //        //Compute the intersection of slab intersection intervals
  //        if(t1 >= tmin) tmin = t1 ;
  //        if(t2 <= tmax) tmax = t2 ;
  //
  //        //Exit with no collision as soon as slab intersection becomes empty
  //        if(tmin > tmax) return false ;
  //    }// end for perpendicular to y-axie
  //
  //    tmin = 0.0 ;
  //    tmax = GBoundingBox::infinite ;
  //
  //    //The plane perpendicular to z-axie
  //    if(fabs(ray.direction.z) <= eps) //If the ray parallel to the plane
  //    {
  //        //If the ray is not within AABB box, then not intersecting
  //        if(ray.start.z < minLimit.z || ray.start.z > maxLimit.z )
  //            return false ;
  //    }
  //    else
  //    {
  //        //Compute the distance of ray to the near plane and far plane
  //        //float ood = 1.0f / dir.z ;
  //        double t1 = (minLimit.z - ray.start.z) * ray.inv_direction.z  ;
  //        double t2 = (maxLimit.z - ray.start.z) * ray.inv_direction.z  ;
  //
  //        //Make t1 be intersecting with the near plane, t2 with the far plane
  //        if(t1 >= t2)
  //        {
  //            double temp = t1 ;
  //            t1 = t2 ;
  //            t2 = temp ;
  //        }
  //
  //        //Compute the intersection of slab intersection intervals
  //        if(t1 > tmin) tmin = t1 ;
  //        if(t2 < tmax) tmax = t2 ;
  //
  //        //Exit with no collision as soon as slab intersection becomes empty
  //        if(tmin > tmax) return false ;
  //    }// end for perpendicular to z-axie
  //
  //    dis = tmin;
  //    return true ;
  //

  // ref:
  // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
  double tmin, tmax, tymin, tymax, tzmin, tzmax;
  if (ray.inv_direction.x >= 0.0) {
    tmin = (minLimit.x - ray.start.x) * ray.inv_direction.x;
    tmax = (maxLimit.x - ray.start.x) * ray.inv_direction.x;
  } else {
    tmin = (maxLimit.x - ray.start.x) * ray.inv_direction.x;
    tmax = (minLimit.x - ray.start.x) * ray.inv_direction.x;
  }
  if (ray.inv_direction.y >= 0.0) {
    tymin = (minLimit.y - ray.start.y) * ray.inv_direction.y;
    tymax = (maxLimit.y - ray.start.y) * ray.inv_direction.y;
  } else {
    tymin = (maxLimit.y - ray.start.y) * ray.inv_direction.y;
    tymax = (minLimit.y - ray.start.y) * ray.inv_direction.y;
  }

  if ((tmin > tymax) || (tymin > tmax)) return false;
  if (tymin > tmin) tmin = tymin;
  if (tymax < tmax) tmax = tymax;

  if (ray.inv_direction.z >= 0.0) {
    tzmin = (minLimit.z - ray.start.z) * ray.inv_direction.z;
    tzmax = (maxLimit.z - ray.start.z) * ray.inv_direction.z;
  } else {
    tzmin = (maxLimit.z - ray.start.z) * ray.inv_direction.z;
    tzmax = (minLimit.z - ray.start.z) * ray.inv_direction.z;
  }

  if ((tmin > tzmax) || (tzmin > tmax)) return false;
  if (tzmin > tmin) tmin = tzmin;
  if (tzmax < tmax) tmax = tzmax;

  dis = tmin;

  return true;

  //   the following code is problematic
  //    // lb is the corner of AABB with minimal coordinates - left bottom, rt
  //    is maximal corner
  //    // r.org is origin of ray
  //    dis = 0.0; // the length of the ray, this is useful to check the first
  //    intersection point
  //
  //    double t1 = (minLimit.x - ray.start.x)*ray.inv_direction.x;
  //    double t2 = (maxLimit.x - ray.start.x)*ray.inv_direction.x;
  //    double t3 = (minLimit.y - ray.start.y)*ray.inv_direction.y;
  //    double t4 = (maxLimit.y - ray.start.y)*ray.inv_direction.y;
  //    double t5 = (minLimit.z - ray.start.z)*ray.inv_direction.z;
  //    double t6 = (maxLimit.z - ray.start.z)*ray.inv_direction.z;
  //
  //    double tmin = fmax(fmax(fmin(t1, t2), fmin(t3, t4)), fmin(t5, t6));
  //    double tmax = fmin(fmin(fmax(t1, t2), fmax(t3, t4)), fmax(t5, t6));
  //
  //    // if tmax < 0, ray (line) is intersecting AABB, but whole AABB is
  //    behing us if (tmax <= 0.0)
  //    {
  //        dis = tmax;
  //        return false;
  //    }
  //
  //    // if tmin > tmax, ray doesn't intersect AABB
  //    if (tmin >= tmax)
  //    {
  //        dis = tmax;
  //        return false;
  //    }
  //
  //    dis = tmin;
  //    return true;
}
