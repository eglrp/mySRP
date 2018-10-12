//
//  main.cpp
//  myRayTracing
//
//  Created by lizhen on 23/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#include <fstream>
#include <iostream>

#include "GKDTree.hpp"
#include "GRayTracer.hpp"

std::vector<std::string> split(const std::string& s, const std::string& delim) {
  std::vector<std::string> elems;
  size_t pos = 0;
  size_t len = s.length();
  size_t delim_len = delim.length();
  if (delim_len == 0) return elems;
  if (len == 0) return elems;
  while (pos < len) {
    int find_pos = s.find(delim, pos);
    if (find_pos < 0) {
      elems.push_back(s.substr(pos, len - pos));
      break;
    }
    elems.push_back(s.substr(pos, find_pos - pos));
    pos = find_pos + delim_len;
  }
  return elems;
}

int main(int argc, const char* argv[]) {
  printf("avgv[1]:  %s\n", argv[1]);
  printf("avgv[2]:  %s\n", argv[2]);
  printf("avgv[3]:  %s\n", argv[3]);
  printf("avgv[4]:  %s\n", argv[4]);
  printf("avgv[5]:  %s\n", argv[5]);
  printf("avgv[6]:  %s\n", argv[6]);

  std::vector<std::string> test = split("24", ",");

  // printf("test: %s\n",test[0].c_str());

  //
  //    argv[1] =
  //    "/Users/lizhen/study/the_way_to_PhD/projects/myRayTracing/fast_SRP/testGPSIIR/GPSIIR_NONAP.txt";
  //
  //    argv[2] = "1";
  //
  //    argv[3] = "0.01";
  //
  //    argv[4] =
  //    "/Users/lizhen/study/the_way_to_PhD/projects/myRayTracing/fast_SRP/testGPSIIR";
  //
  //    argv[5] = "12,34";
  //
  //	  argv[6] ="."

  // argv[1] is the spacecraft description file
  // argv[2] is the maximum depth
  // argv[3] is the pixel array resolution, 0.1 or 0.01 or 0.001 in meters
  // argv[4] is the output file path
  // argv[5] is the latitude
  // argv[6] is the longitude

  GRayTracer myraytracer;
  double lat = 0.0, lon = 0.0;
  myraytracer.objects.loadUserfile(argv[1]);

  int max_depth = atoi(argv[2]);

  GRayTracer::MAX_DEPTH = max_depth;

  GPixelArray::pixelSize = atof(argv[3]);

  //一定要重设面积元素
  GPixelArray::pixelArea = GPixelArray::pixelSize * GPixelArray::pixelSize;

  std::string filepath = argv[4];

  std::vector<double> lat_vec;
  std::vector<double> lon_vec;
  std::vector<std::string> lat_str_vec;
  std::vector<std::string> lon_str_vec;

  std::string lat_str = argv[5];
  std::string lon_str = argv[6];

  if (!strcmp(argv[5], "."))  // lat
  {
    for (int i = -90; i <= 90; i++) {
      lat_vec.push_back(i);
    }
    // printf("lat NULL \n");
  } else {
    lat_str_vec = split(lat_str, ",");

    for (int i = 0; i < lat_str_vec.size(); i++) {
      lat_vec.push_back(atof(lat_str_vec[i].c_str()));
      // printf("%s ", lat_str_vec[i].c_str());
    }
  }

  if (!strcmp(argv[6], "."))  // lon
  {
    for (int i = -180; i <= 180; i++) {
      lon_vec.push_back(i);
    }
    // printf("lon NULL \n");
  } else {
    lon_str_vec = split(lon_str, ",");
    for (int i = 0; i < lon_str_vec.size(); i++) {
      lon_vec.push_back(atof(lon_str_vec[i].c_str()));
      // printf("%s ", lon_str_vec[i].c_str());
    }
  }

  //
  //   printf("\n");
  //   for(int i = 0 ; i< lat_vec.size(); i++ )
  //   {
  //
  //   		printf("%f ", lat_vec[i]);
  //   }
  //
  //   printf("\n ");
  //
  //   for(int i = 0 ; i< lon_vec.size(); i++ )
  //   {
  //   		printf("%f ", lon_vec[i]);
  //   }
  //
  //   printf("\n ");

  // return 0;

#ifdef USE_KDTREE
  // for the kd tree
  myraytracer.makeTree();
#endif
  // int depth = 0;
  // myraytracer.kdtree.showTree(myraytracer.kdtree.root,depth );

  double t0 = clock();

  printf(
      "lat lon ray_intersected totalRay ratio area_hit pixelarray_x "
      "pixelarray_y x y z magnitude\n");

  for (int i = 0; i < lat_vec.size(); i++) {
    for (int j = 0; j < lon_vec.size(); j++) {
      lat = lat_vec[i];
      lon = lon_vec[j];
      char testFN[1024] = {0};
      sprintf(testFN, "LAT%06.2fLON%06.2f.txt", lat, lon);
      std::string filename(testFN);
      std::string outputFN = filepath;
      outputFN = outputFN + filename;
      // printf("filename: %s\n", outputFN.c_str());
      FILE* output = fopen(outputFN.c_str(), "w+");
      // std::ofstream output(outputFN);
      if (!output) {
        printf("the output file failed\n");
        return 0;
      }

      double lon_rad = lon * M_PI / 180.0;
      double lat_rad = lat * M_PI / 180.0;

      fprintf(output,
              "lat lon ray_intersected totalRay ratio area_hit pixelarray_x "
              "pixelarray_y x y z magnitude\n");

      myraytracer.rayIntersected = 0;
      myraytracer.area_hit = 0.0;

      myraytracer.pixelarray.buildPixelArray(lat_rad, lon_rad,
                                             myraytracer.objects.boundingBox);

      GVector force = myraytracer.run();

      printf(
          "%.3f %.3f %ld %lu %f %f %.3f %.3f  %.16E  %.16E  %.16E  %.16E\n",
          lat, lon, myraytracer.rayIntersected, myraytracer.pixelarray.totalRay,
          double(myraytracer.rayIntersected) / myraytracer.pixelarray.totalRay,
          myraytracer.area_hit, myraytracer.pixelarray.len_x,
          myraytracer.pixelarray.len_y, force.x, force.y, force.z,
          force.norm());

      fprintf(
          output,
          "%.3f %.3f %ld %lu %f %f %.3f %.3f  %.16E  %.16E %.16E   %.16E\n",
          lat, lon, myraytracer.rayIntersected, myraytracer.pixelarray.totalRay,
          double(myraytracer.rayIntersected) / myraytracer.pixelarray.totalRay,
          myraytracer.area_hit, myraytracer.pixelarray.len_x,
          myraytracer.pixelarray.len_y, force.x, force.y, force.z,
          force.norm());

      if (output != NULL) {
        fclose(output);
        output = NULL;
      }
    }
  }

  double t1 = clock();

  double time = (t1 - t0) / ((double)CLOCKS_PER_SEC);

  std::cout << "time:" << time << " second" << std::endl;

  // hello test
  // insert code here...
  // std::cout << "total rays: " << myraytracer.pixelarray.rays.size()<<
  // std::endl;
  return 0;
}