//
//  GGeometry.cpp
//  myRayTracing
//
//  Created by lizhen on 23/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#include "GGeometry.hpp"

// static values

// double GGeometry::EPS = std::numeric_limits<double>::epsilon();  // the EPS
// value
double GGeometry::EPS = 1.0E-12;  // the EPS value, probably it is enough
double GGeometry::BoundingLimit = 0.001;  // the default minimum value for the bounding box of plane

// the default standard deviation of the optical properties defined in GRay.h
double GOpticalProperty::default_std_v = 0.01;
double GOpticalProperty::default_std_u = 0.01;
double GOpticalProperty::default_std_d = 0.01;

GGeometry::GGeometry() { index = 0; }
