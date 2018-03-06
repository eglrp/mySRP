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
    
    // the default standard deviation of the optical properties
    static double default_std ;
    
    // for all the radiation spectrum
    double solar_absorptivity;
    double solar_absorptivity_std; // the uncertainty of optical property
    
    double solar_reflectivity;
    double solar_reflectivity_std;
    
    double solar_specularity;
    double solar_specularity_std;
    
    double solar_emissivity;
    double solar_emissivity_std;
    
    
    
    // for the infrared radiation spectrum
    double ir_absorptivity;
    double ir_absorptivity_std;
    
    double ir_reflectivity;
    double ir_reflectivity_std;
    
    
    double ir_specularity;
    double ir_specularity_std;
    
    
    double ir_emissivity;
    double ir_emissivity_std;
    
    
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
        
        solar_emissivity_std = default_std;
        solar_specularity_std = default_std;
        solar_absorptivity_std = default_std;
        solar_reflectivity_std = default_std;
        
        ir_emissivity_std = default_std;
        ir_specularity_std = default_std;
        ir_absorptivity_std = default_std;
        ir_reflectivity_std = default_std;
        
        mli_type = 0;
    }
    
    
};


#endif /* GRay_h */
