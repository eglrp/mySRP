//
//  GGeometry.hpp
//  myRayTracing
//
//  Created by lizhen on 23/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#ifndef GGeometry_hpp
#define GGeometry_hpp

#include <stdio.h>
#include "GVector.hpp"
#include <vector>
#include <memory>
#include "GBoundingBox.hpp"
#include "GPixelArray.hpp"

using namespace gfc;




// the father class for all the geometry
// every geometry should have a surface normal to indicate the front side
class GGeometry
{
    
public:
    
    static double EPS;
    static double BoundingLimit;
    long index;
    // the vecor of the local coorindate system axis and origin in BFS
    // this coordinate is defined to describe the geometry
    GVector cx;
    GVector cy;
    GVector cz;
    GVector cs;
    
    // the optical property
    GOpticalProperty op;
    
    GBoundingBox boundingBox;
    
    GGeometry();
    virtual ~GGeometry() {}  // virtual destructor
    //void updateBoundingBoxLimits(GVector& p);
    
    // this function return the intersection point and surface normal at that intersection
    virtual bool intersection(GRay &ray, GVector& intersection, gfc::GVector &normal) {return false;}
    virtual void computeBoundingBox() { }
    
};


















#endif /* GGeometry_hpp */
