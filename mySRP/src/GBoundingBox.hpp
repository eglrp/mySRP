//
//  GBoundingBox.hpp
//  myRayTracing
//
//  Created by lizhen on 28/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#ifndef GBoundingBox_hpp
#define GBoundingBox_hpp

#include <stdio.h>
#include "GVector.hpp"
#include "GRay.h"

using namespace gfc;

/*
 the class for the bounding box
 every geometry should have a bounding box
 bounding box has to be in BFS
 minLimit and maxLimit is in BFS for the oriented bounding box
 minLimit_l and maxLimit_l is in local system of the AABB
 
 */
class GBoundingBox
{
    
public:
    static double infinite;
    // the bounding box for the geometry in the BFS coordinate system
    GVector minLimit;
    GVector maxLimit;
    
    // the limit in the geometry local coordinate system
    // basically, this is the AABB of local system
    GVector minLimit_l;
    GVector maxLimit_l;
    
    GVector center;
    
    GBoundingBox();
    
    void computeCenter();
    
    //updating the limits of bounding box
    // p is input
    // min and max are both output
    void updateBoundingBoxLimits(GVector& p, GVector& min, GVector& max);
    
    //generate 8 vertices of AABB box in local system
    void generateLocalBox( GVector box[8]);
    
    bool intersect(GRay& ray,double& dis);
    
};


#endif /* GBoundingBox_hpp */
