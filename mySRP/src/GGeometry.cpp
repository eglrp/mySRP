//
//  GGeometry.cpp
//  myRayTracing
//
//  Created by lizhen on 23/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#include "GGeometry.hpp"

//static values

//double GGeometry::EPS = std::numeric_limits<double>::epsilon();  // the EPS value
double GGeometry::EPS = 1.0E-12;  // the EPS value, probably it is enough
double GGeometry::BoundingLimit = 0.001; // the default minimum value for the bounding box of plane


GGeometry::GGeometry()
{
    //specularity = 0.0;
    //reflectivity = 0.0;
    //emmisivity = 0.0;
    //absorptivity = 0.0;
    
    //isMLI = false;
    index = 0;
}











