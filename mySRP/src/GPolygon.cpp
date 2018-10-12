//
//  GPolygon.cpp
//  myRayTracing
//
//  Created by lizhen on 24/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#include "GPolygon.hpp"

/*compute the bounding box of a polygon*/
void GPolygon::computeBoundingBox() {
  // building the local system first

  // the origin of local system
  cs = vertices[0];

  cx = vertices[1] - cs;
  cx.normalise();

  GVector a2 = vertices[2] - cs;
  cz = crossproduct(cx, a2);
  cz.normalise();

  cy = crossproduct(cz, cx);
  cy.normalise();

  GVector pnt;
  // //calculating bounding box, project all the vertices onto the local plane
  points.resize(vertices.size());
  for (int i = 0; i < points.size(); i++) {
    // minuse the origin first to get the vector, than rotate the vector
    pnt = vertices[i] - cs;

    points[i].x = cx.x * pnt.x + cx.y * pnt.y + cx.z * pnt.z;
    points[i].y = cy.x * pnt.x + cy.y * pnt.y + cy.z * pnt.z;

    points[i].z = cz.x * pnt.x + cz.y * pnt.y + cz.z * pnt.z;

    // set the z cooridnate of local system to the limit
    // points[i].z = BoundingLimit;

    // updating the bounding box of local system which will get the AABB of this
    // geometry in local system

    boundingBox.updateBoundingBoxLimits(points[i], boundingBox.minLimit_l,
                                        boundingBox.maxLimit_l);
  }

  // generate the bounding box contains 8 verices, than convert all of them to
  // BFS and update the limits to get the AABB in BFS
  GVector box[8];
  boundingBox.generateLocalBox(box);

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

/*

 // isLeft(): tests if a point is Left|On|Right of an infinite line.
 //    Input:  three points P0, P1, and P2
 //    Return: >0 for P2 left of the line through P0 and P1
 //            =0 for P2  on the line
 //            <0 for P2  right of the line
 //    See: Algorithm 1 "Area of Triangles and Polygons",
 http://geomalgorithms.com/a01-_area.html

 */
double GPolygon::isLeft(gfc::GVector& p0, gfc::GVector& p1, gfc::GVector& p2) {
  return ((p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y));
}

/*


 the winding number algorithm in topology
 参考  http://blog.csdn.net/jq_develop/article/details/44981127

 http://geomalgorithms.com/a03-_inclusion.html

 */
bool GPolygon::PnPoly(gfc::GVector& p) {
  bool ret = false;
  unsigned int size = vertices.size();

  // project the point p onto the local coordinates
  GVector pt, pp = p - cs;
  pt.x = cx.x * pp.x + cx.y * pp.y + cx.z * pp.z;
  pt.y = cy.x * pp.x + cy.y * pp.y + cy.z * pp.z;
  pt.z = cz.x * pp.x + cz.y * pp.y + cz.z * pp.z;

  // after this projection, all the points should have 0 z coordinate on the
  // projection plane applying the winding number algorithm
  int winding_number = 0;
  for (int i = 0; i < size; i++) {
    int j = i + 1;
    if (j == size) j = 0;

    if (points[i].y <= pt.y) {
      if (points[j].y > pt.y) {
        if (isLeft(points[i], points[j], pt) > 0.0) {
          ++winding_number;
        }
      }
    } else {
      if (points[j].y <= pt.y) {
        if (isLeft(points[i], points[j], pt) < 0.0) {
          --winding_number;
        }
      }
    }
  }

  if (winding_number == 0)  // outside
  {
    ret = false;
  } else {
    ret = true;
  }

  return ret;
}

/*
 the intersection of ray and polygon

 首先计算出与平面的交点，在判断点是否在多变形内部，采用拓扑学中的回转数概念(winding
 number algorithm in topology) 平面方程是： (x-x0)Tn = 0
 */
bool GPolygon::intersection(GRay& ray, gfc::GVector& intersection,
                            gfc::GVector& normal) {
  bool ret = false;
  // arbitrary vertex
  GVector x0 = vertices[0];
  GVector p = ray.start - x0;
  double ptn = dotproduct(p, cz);

  // Ntn is used to check the direction of ray, if it is from the back side of
  // the polygon
  double Ntn = dotproduct(ray.direction, cz);

  // assuming the normal of the vector is in cz direction
  // the ray is from the back side, according to marek's phd thesis, this ray
  // should be rejected
  //    if(Ntn >= 0.0)
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

  if (t < 0.0) {
    return false;
  }

  if (fabs(t) < GGeometry::EPS) {
    return false;
  }

  // the possible intersection
  GVector candidate = ray.start + t * ray.direction;

  // check if that intersection is inside the polygon or not

  ret = PnPoly(candidate);

  if (ret == true) {
    intersection = candidate;

    // ray is from the front side
    normal = cz;

    // the ray is from the back side // the ray is from the back side, according
    // to marek's phd thesis, this ray should be rejected, but this case is accepted in this software to deal with multiple reflections
    if (Ntn >= 0.0) {
      normal = -cz;
    } else  // ray is from the front side
    {
      normal = cz;
    }
  }

  return ret;
}
