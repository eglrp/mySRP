//
//  GPixelArray.hpp
//  myRayTracing
//
//  Created by lizhen on 24/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#ifndef GPixelArray_hpp
#define GPixelArray_hpp

#include <stdio.h>
#include <vector>

#include "GBoundingBox.hpp"


// the pixel array class
// pixel array uses UVW cooridnate, with W from array to the spacecraft
// the origin of the solar array is defined at geometric center of the solar array
// assuming all the rays from the pixel array are parallel, same direction, different start point
class GPixelArray
{
    
public:
    
    static double pixelSize ;  // the size of one pixel, pixel is
    static double pixelArea ;  // the area of pixel, a square
    
    static double d0;    // the distance from pixel array to the spacecraft model origin
    
    // the two directions of pixel array, different for different ray
    // this is really important when the origin of BFS is not at the geometry centre
    GVector dx;
    GVector dy;
    GVector pixelArrayCenter;
    GVector firstPixel;
    double len_x;  // the length of pixel array in X direction
    double len_y; //  the length of pixel array in Y direction
    
    //these are used to generate the ray
    bool stop;
    int i_index;
    int j_index;
    long num_x;
    long num_y;
    long totalRay; // the number of total rays
    
    // the UVW axis in body fixed frame
    GVector u;
    GVector v;
    GVector w;
    GVector s; // the origin of uvw(center of pixel array) in BFS
    
    double lat; // the lattitude is the angle between W and xoy
    
    double lon; // the longitude is the angle between W and z
    
    std::vector<GRay> rays;
    
    // the default constructor
    GPixelArray() {}
    
    GVector  XYZ2UVW( GVector xyz );
    GVector  UVW2XYZ( GVector uvw);
    
    void buildUVW();
    
    void generateRays();
    
    void buildPixelArray(double Lat, double Lon, GBoundingBox& aabb);
    
    bool getRay(GRay& ray);

    
};



#endif /* GPixelArray_hpp */
