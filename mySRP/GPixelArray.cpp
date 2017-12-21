//
//  GPixelArray.cpp
//  myRayTracing
//
//  Created by lizhen on 24/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#include "GPixelArray.hpp"


double GPixelArray::pixelSize = 0.01; // set the spacing 1 cm by default
double GPixelArray::pixelArea = GPixelArray::pixelSize*GPixelArray::pixelSize;

double GPixelArray::d0 = 100.0;  // set the distance 100 m by default


/*
 build the UVW coordinates for the pixel array
 W is the ray direction from pixel array to the objects
 steps:
 1. rotate aroud z with angle lambda, make BFS x in the projection of ray on BFS xoy plane
 2. rotate aroud y with angle 3pi/2 - phi, make BFS z axis pointing to the ray direction(W).
 
 s
 z   /|
 |  / |
 | /  |
 |----|-----y
 /\   |
 /  \  |
 /    \ |
 x      \|
 p
 
 在这个定义下，星固系下的光线的方向（从pixel array 到卫星）转到UVW中为(0,0,1)
 
 主要问题是：pixel array 在采样时，如何尽量减小采样误差（主要表现为横截面积误差）
 
 */

void GPixelArray::buildUVW()
{
    /*
     从XYZ转到UVW的转换矩阵为
     
     -sin(phi)cos(lambda)  -sin(phi)sin(lambda)   cos(phi)
     R= -sin(lambda)           cos(lambda)            0
     -cos(phi)cos(lambda)  -cos(phi)sin(lambda)    -sin(phi)
     
     */
    
    if( fabs(lat -M_PI/2.0) < 1.0E-12  ) // lat = 90
    {
        lon = 0.0;
        u.set(-1, 0, 0);
        v.set(0, 1, 0);
        w.set(0,0, -1);
    }
    else if(fabs(lat + M_PI/2.0) < 1.0E-12)
    {
        lon = 0.0;
        u.set(1, 0, 0);
        v.set(0, 1, 0);
        w.set(0,0, 1);
    }
    else
    
    {
        u.set(-sin(lat)*cos(lon), -sin(lat)*sin(lon), cos(lat));
        v.set(-sin(lon), cos(lon), 0.0);
        w.set(-cos(lat)*cos(lon), -cos(lat)*sin(lon), -sin(lat));
    }
    
    s.set(GPixelArray::d0*cos(lat)*cos(lon),
          GPixelArray::d0*cos(lat)*sin(lon),
          GPixelArray::d0*sin(lat));
    
}

GVector  GPixelArray::XYZ2UVW( GVector xyz )
{
    GVector uvw;
    xyz = xyz - s;
    
    uvw.x = u.x*xyz.x + u.y*xyz.y + u.z*xyz.z;
    uvw.y = v.x*xyz.x + v.y*xyz.y + v.z*xyz.z;
    uvw.z = w.x*xyz.x + w.y*xyz.y + w.z*xyz.z;
    
    
    return uvw;
}

GVector GPixelArray::UVW2XYZ(GVector uvw)
{
    GVector xyz;
    xyz.x = u.x*uvw.x + v.x*uvw.y + w.x*uvw.z;
    xyz.y = u.y*uvw.x + v.y*uvw.y + w.y*uvw.z;
    xyz.z = u.z*uvw.x + v.z*uvw.y + w.z*uvw.z;
    
    xyz += s;
    
    return xyz;
}



/*
 here aabb is the axis aligned bounding box of the whole satellite in bfs
 the principle of building this pixel array is:
 as big enough for the spacecraft
 as small as possible for the sake of computation speed
 */
void GPixelArray::buildPixelArray(double Lat, double Lon, GBoundingBox& aabb)
{
    
    lat = Lat;
    lon = Lon;
    
    buildUVW();
    
    
    GVector box[8]; // in Body Fixed Frame
    GVector minLimit(GBoundingBox::infinite,GBoundingBox::infinite,GBoundingBox::infinite); //in UVW system
    GVector maxLimit(-GBoundingBox::infinite,-GBoundingBox::infinite,-GBoundingBox::infinite); // in UVW system
    aabb.generateLocalBox(box);
    //convert the aabb in BFS to UVW
    GVector pnt;
    for( int i = 0 ; i< 8 ; i++ )
    {
        pnt = XYZ2UVW(box[i]);
        
        //box[i] = pnt;
        //here, probably there is no need to find the AABB in UVW system
        //find the bounding box for UVW
        aabb.updateBoundingBoxLimits(pnt, minLimit, maxLimit);
    }
    
    // to determine the size of the pixel array
    // find the minimum u point and the minimum v point as one axis
    // the other axis is the minimum u point and the maximum v point
    GVector refpoint[4] ={
        GVector(minLimit.x,minLimit.y,0),
        GVector(maxLimit.x,minLimit.y,0),
        GVector(minLimit.x,maxLimit.y,0),
        GVector(maxLimit.x,maxLimit.y,0)
    };  // refpoint[0]-->minimum u, refpoint[1]-->minimum v, refpoint[2]-->maximum v
    
    // two directions of the rectangle pixel array in UVW, dx is perpendicular to dy
    dx = refpoint[1] - refpoint[0];
    dy = refpoint[2] - refpoint[0];
    
    pixelArrayCenter.set(0.0, 0.0, 0.0);
    for(int i = 0 ; i< 4; i++ )
    {
        pixelArrayCenter += refpoint[i];
    }
    pixelArrayCenter = pixelArrayCenter/4.0;
    
    //firstPixelPos = refpoint[0];
    
    // the size of pixel array need to be determined
    len_x = dx.norm();
    len_y = dy.norm();
    dx.normalise();
    dy.normalise();
    
    
    num_x = int(len_x / GPixelArray::pixelSize)+2;
    num_y = int(len_y / GPixelArray::pixelSize)+2;
    firstPixel = refpoint[0] + 0.5*GPixelArray::pixelSize*dx + 0.5*GPixelArray::pixelSize*dy;
    
    
    /*
    num_x = int(len_x / GPixelArray::pixelSize);
    num_y = int(len_y / GPixelArray::pixelSize);
    //make sure that num_u and num_v are both odd
    if( num_x % 2 == 0 ) num_x = num_x + 1;
    if( num_y % 2 == 0 ) num_y = num_y + 1;
    
    //calculate the minus u and minus v pixel position which is the minimum coordinate
    firstPixel.set(-(num_x-1)/2.0*GPixelArray::pixelSize, -(num_y-1)/2.0*GPixelArray::pixelSize, 0.0);
    firstPixel += pixelArrayCenter;
    */
    
    
    
    i_index = 0;
    j_index = 0;
    totalRay = 0;
    stop = false;
    //一次性生成所有光线太占内存
   // generateRays();
    
    int testc = 0;
    
}


bool GPixelArray::getRay(GRay& ray)
{
    
    ray.start.set(0.0, 0.0, 0.0);
    ray.direction.set(0.0, 0.0, 0.0);
    ray.inv_direction.set(0.0, 0.0, 0.0);
    ray.energy = 1.0;
    
    ray.reflectionNum = 0;
    
    int c_i = i_index;
    int c_j = j_index;
    
    if(stop == true)
    {
        return false;
    }
    
    
    GVector pixelPos ;
    pixelPos = firstPixel + c_i*GPixelArray::pixelSize*dx + c_j*GPixelArray::pixelSize*dy ;
    
    //printf("i:%d, j:%d\n", c_i,c_j);
    ray.start =  UVW2XYZ(pixelPos);
    ray.direction = w;
    ray.inv_direction.set(1.0/w.x, 1.0/w.y, 1.0/w.z);
    
    
    if(i_index++ >= num_x-1)
    {
        i_index=0;
        
        if(j_index++ >= num_y-1)
        {
            stop = true;
        }
    }

    
    return true;
}


/*
 generate all the rays for the pixel array

 this function need to be optimized later
 acoording to the proper shape of the outline of the object to generate the rays
 this can help to reduce the total number of rays
 */
void GPixelArray::generateRays()
{
    
    GVector pixelPos ;
    GRay myray;
    myray.direction = w;
    myray.inv_direction.set(1.0/w.x, 1.0/w.y, 1.0/w.z);
    
    rays.resize(num_x*num_y);
    
    int k =0; // the count of rays
    for(int i = 0; i< num_x; i++ )
    {
        for( int j = 0 ; j< num_y; j++ )
        {
            
            pixelPos = firstPixel + i*GPixelArray::pixelSize*dx + j*GPixelArray::pixelSize*dy ;
            
            //pixelPos.x = firstPixelPos.x + i*GPixelArray::pixelSize;
            //pixelPos.y = firstPixelPos.y + j*GPixelArray::pixelSize;
            
            //printf("%.3f %.3f\n", pixelPos.x, pixelPos.y);
            
            myray.start = UVW2XYZ(pixelPos);
            
            rays[k++] = myray;
            
        }
    }
    
}





