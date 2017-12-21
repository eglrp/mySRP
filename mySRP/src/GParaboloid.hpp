//
//  GParaboloid.hpp
//  myRayTracing
//
//  Created by lizhen on 25/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#ifndef GParaboloid_hpp
#define GParaboloid_hpp

#include <stdio.h>
#include "GGeometry.hpp"

class GParaboloid : public GGeometry
{
    
public:
    
    double height; // the depth of the cone
    double depth;  // the distance from the bottom vertex to the top vertex
    double radius_b; // the radius of the bottom circle
    double radius_t; // the radius of the top circle, if it not zeros, it its truncated
    GVector center_b;  // the center of the bottom circle
    GVector center_t;  // the center of the top circle, if it is not truncated, it is the vertex
    bool   in_out;  // the inward pointing(true) or outward pointing(false), by default it is false, this will determine the direction of normal vector
    GParaboloid()
    {
        depth = 0.0;
        height =0.0;
        radius_b = 0.0;
        radius_t = 0.0;
        in_out = false;
    }
    
    void computeBoundingBox();
    bool intersection(GRay &ray, GVector& intersection, gfc::GVector &normal) ;
    
    
};


#endif /* GParaboloid_hpp */
