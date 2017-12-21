//
//  GCylinder.hpp
//  myRayTracing
//
//  Created by lizhen on 25/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#ifndef GCylinder_hpp
#define GCylinder_hpp

#include <stdio.h>
#include "GGeometry.hpp"


// this cylinder should include the ordinary cylinder
class GCylinder : public GGeometry
{
    // by default, the cyclinder is open end(both)
public:
    double radius;
    GVector vertices[2];  // 0 for the bottom center and 1 for the top center
    double height; // the height of the cylinder
    GVector normal; // the normal vector of the cylinder
    
    void computeBoundingBox();
    bool intersection(GRay &ray, GVector& intersection, gfc::GVector &normal) ;
    
};


#endif /* GCylinder_hpp */
