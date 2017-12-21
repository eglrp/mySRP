//
//  GObjects.hpp
//  myRayTracing
//
//  Created by lizhen on 23/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#ifndef GObjects_hpp
#define GObjects_hpp

#include <iostream>
#include <stdio.h>
#include <memory>
#include <vector>
#include <string>
#include <string.h>
#include <fstream>

#include "GPixelArray.hpp"

#include "GPolygon.hpp"
#include "GSphere.hpp"
#include "GDisc.hpp"
#include "GRing.hpp"
#include "GCylinder.hpp"
#include "GCone.hpp"
#include "GParaboloid.hpp"

using namespace std;
// the class to define the object which contains many geometries
// GObjects contains all the geometries
class GScene : public GGeometry
{
    
public:
    
    
    // the components of the spacecraft
    // only c++14 support this unique_prt
    std::vector< std::unique_ptr<GGeometry> > components;
    
    void computeBoundingBox();
    
    //loading the components from UCL user file
    void loadUserfile(std::string filename);
    
    
};



#endif /* GObjects_hpp */
