//
//  GPolygon.hpp
//  myRayTracing
//
//  Created by lizhen on 24/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#ifndef GPolygon_hpp
#define GPolygon_hpp

#include <stdio.h>
#include "GGeometry.hpp"

class GPolygon : public GGeometry
{
    
public:
    
    //these vertices should be in counter clock wise order
    // this is used to define the normal of polygon
    // these coordinates are in global coordinate system
    std::vector<GVector> vertices;
    
    // the plane coordinate of all the vertices in local coordinate system
    std::vector<GVector> points;
    
    GPolygon() {}
    
    GPolygon( std::vector<GVector> points )
    {
        //call the constructor of the father class
        GGeometry();
        
        vertices = points;
    }
    
    void computeBoundingBox();
    bool intersection(GRay &ray, GVector& intersection, gfc::GVector &normal) ;
    
    //test whether the point p is inside the this polygon
    bool PnPoly(gfc::GVector& p);
    
    double isLeft(GVector& p0, GVector& p1, GVector& p2);
    
};



#endif /* GPolygon_hpp */
