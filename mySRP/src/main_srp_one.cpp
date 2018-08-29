//
//  main.cpp
//  myRayTracing
//
//  Created by lizhen on 23/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#include <iostream>
#include <fstream>

#include "GRayTracer.hpp"
#include "GKDTree.hpp"

int main( int argc, const char * argv[] )
{
    
//    
//    GPixelArray mypixa;
//    mypixa.lat = 0.0*M_PI/180.0;
//    mypixa.lon = 90.0*M_PI/180.0;
//    mypixa.buildUVW();
//    GVector myxyz,myuvw;
//    myuvw = mypixa.XYZ2UVW(myxyz);
//    
//    std::vector<GVector> points={ GVector(0,0,0),GVector(0,0,2),GVector(1,0,2),GVector(1,0,0) };
//    
//    GPolygon polygon(points);
//    
//    polygon.computeBoundingBox();
//    
//    GRay ray;
//    ray.start = GVector(0,0.5,1);
//    ray.direction = GVector(1,-1, 0);
//    ray.direction.normalise();
//    
//    GVector intersection, normal;
//    
//    bool stat = polygon.intersection(ray, intersection, normal);
    
    GBoundingBox bx;
    bx.minLimit.set(-1, -1, -1);
    bx.maxLimit.set(3, 3, 1);
    
    
    GRayTracer myraytracer;
    double lat = 0.0, lon = 0.0;
    
    //argv[1] is the spacecraft description file
    //argv[2] is the longitude in degree
    //argv[3] is the pixel array resolution, 0.1 or 0.01 or 0.001 in meters 
    //argv[4] is the output file path
    myraytracer.objects.loadUserfile(argv[1]);  
    lon = atof(argv[2]);
    lat = atof(argv[3]);
    GPixelArray::pixelSize = atof(argv[4]); 
    //一定要重设面积元素
    GPixelArray::pixelArea = GPixelArray::pixelSize*GPixelArray::pixelSize;
    std::string filepath = argv[5];
    
    char testFN[1024]={0};
    sprintf(testFN, "LAT%06.2f.txt",lat);
    std::string outputFN=argv[5];
    std::string filename(testFN);
    outputFN = outputFN + filename;	
    printf("filename: %s\n", outputFN.c_str());
    FILE* output = fopen(outputFN.c_str(),"w+");		
    //std::ofstream output(outputFN);
    if(!output) 
    {
    	printf("the output file failed\n");
    	return 0;
    }
    
#ifdef USE_KDTREE
    //for the kd tree
    myraytracer.makeTree();
#endif
    //int depth = 0;
    //myraytracer.kdtree.showTree(myraytracer.kdtree.root,depth );
    
    
    
    
    double lon_rad = lon*M_PI/180.0;
    double t0 = clock();
    printf("lat lon ray_intersected totalRay ratio area_hit pixelarray_x pixelarray_y x y z magnitude\n");
    fprintf(output,"lat lon ray_intersected totalRay ratio area_hit pixelarray_x pixelarray_y x y z magnitude\n");
    
    //for( int i = 0 ; i< 181; i++) //from -90 to 90
    {
        //lat = -90 + i;
        myraytracer.rayIntersected = 0;
        myraytracer.area_hit = 0.0;
        
        myraytracer.pixelarray.buildPixelArray(lat*M_PI/180.0, lon_rad, myraytracer.objects.boundingBox);
        
        GVector force = myraytracer.run();
        
        printf("%.3f %.3f %ld %lu %f %f %.3f %.3f  %.16f  %.16f %.16f   %.16f\n", lat, lon,
               myraytracer.rayIntersected ,
               myraytracer.pixelarray.totalRay,
               double(myraytracer.rayIntersected)/myraytracer.pixelarray.totalRay,
               myraytracer.area_hit,
               myraytracer.pixelarray.len_x,
               myraytracer.pixelarray.len_y,
               force.x, force.y, force.z, force.norm());
       
       fprintf(output,"%.3f %.3f %ld %lu %f %f %.3f %.3f  %.16f  %.16f %.16f   %.16f\n",
       					lat, lon,
       					myraytracer.rayIntersected ,
               myraytracer.pixelarray.totalRay,
               double(myraytracer.rayIntersected)/myraytracer.pixelarray.totalRay,
               myraytracer.area_hit,
               myraytracer.pixelarray.len_x,
               myraytracer.pixelarray.len_y,
               force.x, force.y, force.z, force.norm()
       					);
       
        
    }
    
    if(output != NULL)
   	{
   		 fclose(output);
   		 output = NULL;
    }
    
    double t1 = clock();
    
    double time = (t1-t0)/((double) CLOCKS_PER_SEC);
    
    std::cout<< "time:" << time <<" second" << std::endl;
    
    // hello test
    // insert code here...
    //std::cout << "total rays: " << myraytracer.pixelarray.rays.size()<< std::endl;
    return 0;
    
}
