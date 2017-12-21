//
//  GDisc.hpp
//  myRayTracing
//
//  Created by lizhen on 24/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#ifndef GDisc_hpp
#define GDisc_hpp

#include <stdio.h>
#include "GGeometry.hpp"



// the circle geometry
class GDisc : public GGeometry
{
    
public:
    
    double radius;  // the radius
    
    GVector center; // the center
    
    // the vertices on the disc edge in counter clock wise order when looking down from the normal
    // at least 2 points
    std::vector<GVector> vertices;
    
    void computeBoundingBox();
    bool intersection(GRay &ray, GVector& intersection, gfc::GVector &normal) ;
    
};


#endif /* GDisc_hpp */
