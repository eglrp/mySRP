//
//  GRay.h
//  myRayTracing
//
//  Created by lizhen on 28/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#ifndef GRay_h
#define GRay_h
#include "GVector.hpp"
#include <vector>
using namespace gfc;

// the ray class
class GRay
{
    
public:
    
    GVector start;
    
    GVector direction;  // direction is unit vector
    
    GVector inv_direction;  // the inverse of the direction = 1/direction, used at the intersection of AABB and ray
    
    //int sign[3];
    
    int reflectionNum;  // the reflectionNum, start from 0
    
    double energy;  // the energy of the ray, it is oringally 100%, it will be reduced after one reflection
    
    // the intersection routine
    std::vector<GVector> routine;
    
    //double length;
    
    GRay()
    {
        reflectionNum = 0;
        
        energy = 1.0;
        
        routine.reserve(10);
        
        //sign[0] = 0;
        //sign[1] = 0;
        //sign[2] = 0;
        
        //length = 0;
    }
    
    
};



class GOpticalProperty
{
    
public:
    
    // for all the radiation spectrum
    double solar_absorptivity;
    double solar_reflectivity;
    double solar_specularity;
    double solar_emissivity;
    
    // for the infrared radiation spectrum
    double ir_absorptivity;
    double ir_reflectivity;
    double ir_specularity;
    double ir_emissivity;
    int    mli_type;   // the type of the mli, 0= not mli, 1 for the first mli, 2 for the second mli and so forth
    GOpticalProperty()
    {
        solar_emissivity = 0.0;
        solar_specularity = 0.0;
        solar_absorptivity = 0.0;
        solar_reflectivity = 0.0;
        
        ir_emissivity = 0.0;
        ir_specularity = 0.0;
        ir_absorptivity = 0.0;
        ir_reflectivity = 0.0;
        mli_type = 0;
    }
    
    
};



#endif /* GRay_h */
