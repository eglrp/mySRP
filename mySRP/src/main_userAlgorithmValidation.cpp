

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "GRayTracer.hpp"

using namespace std;

bool readGridFile(std::string file_name, double data[181][361]) {
  std::ifstream grid_file(file_name);

  if (grid_file.fail() || !grid_file.is_open()) {
    std::cerr << "Error opening SRP grid file" << file_name << std::endl;
    return false;
  }

  uint_fast16_t n_cols, n_rows;  // no. of horizontal and vertical grid nodes.
  double min_lon, max_lon, min_lat, max_lat;  // grid file ranges.
  double min_acc, max_acc;

  // First header line:
  std::string scrap;
  getline(grid_file, scrap);  // DSAA designation from Surfer6 format.

  // Check grid resolution:
  grid_file >> n_cols;  // 361

  grid_file >> n_rows;  // 181

  // Check grid value ranges:
  grid_file >> min_lon;

  grid_file >> max_lon;

  grid_file >> min_lat;

  grid_file >> max_lat;

  grid_file >> min_acc;

  grid_file >> max_acc;

  // cout<<file_name.c_str()<<std::endl;
  // Read in grid node values.
  // Populate grid with decreasing row index - maintaining the Surfer6 format
  // (upside Mercator).
  // data.resize(grid_rows);
  for (uint_fast16_t i = 0; i < n_rows; ++i)  // 181 rows
  {
    // data[i].resize(grid_cols);
    for (uint_fast16_t j = 0; j < n_cols; ++j)  // 361 column
    {
      grid_file >> data[i][j];
      // printf("%.16E ", data(i,j) );
    }
    // printf("\n");
  }
  grid_file.close();
  return true;
}

/*
 the biliear interpolation function
 latitude: -90 - 90
 longitude: -180 - 180
 //  https://en.wikipedia.org/wiki/Bilinear_interpolation
 */
GVector mybilinear_interp(double x_grid[181][361], double y_grid[181][361],
                          double z_grid[181][361], double longitude,
                          double latitude) {
  // first find out the corresponding griddata
  GVector acc;

  double min_longitude = -180, max_longitude = 180;
  double min_latitude = -90, max_latitude = 90;

  // Check input for validity.
  if (longitude < min_longitude) {
    longitude = min_longitude;
  } else if (longitude > max_longitude) {
    longitude = max_longitude;
  }

  if (latitude < min_latitude) {
    latitude = min_latitude;
  } else if (latitude > max_latitude) {
    latitude = max_latitude;
  }

  // find the minimum grid first;
  // the grid is 1 by 1 degree
  double u_interval = 1.0;
  double v_interval = 1.0;

  int u = floor((latitude - min_latitude) / u_interval);    // latitude
  int v = floor((longitude - min_longitude) / v_interval);  // longitude

  // make del_u and del_v both less than 1.0
  double del_u = (latitude - min_latitude - u * u_interval) / u_interval;
  double del_v = (longitude - min_longitude - v * v_interval) / v_interval;

  double a_u1 = 0.0, a_u2 = 0.0;
  a_u1 = (1.0 - del_u) * x_grid[u][v] + del_u * x_grid[u + 1][v];
  a_u2 = (1.0 - del_u) * x_grid[u][v + 1] + del_u * x_grid[u + 1][v + 1];
  acc.x = (1.0 - del_v) * a_u1 + del_v * a_u2;

  a_u1 = (1.0 - del_u) * y_grid[u][v] + del_u * y_grid[u + 1][v];
  a_u2 = (1.0 - del_u) * y_grid[u][v + 1] + del_u * y_grid[u + 1][v + 1];
  acc.y = (1.0 - del_v) * a_u1 + del_v * a_u2;

  a_u1 = (1.0 - del_u) * z_grid[u][v] + del_u * z_grid[u + 1][v];
  a_u2 = (1.0 - del_u) * z_grid[u][v + 1] + del_u * z_grid[u + 1][v + 1];
  acc.z = (1.0 - del_v) * a_u1 + del_v * a_u2;

  //    if(v+1 >= 361 )
  //    {
  //        if(u+1 >= 181 ) // only one point
  //        {
  //            acc.x = x_grid[u][v];
  //            acc.y = y_grid[u][v];
  //            acc.z = z_grid[u][v];
  //        }
  //        else  // interpolate with latitude
  //        {
  //            acc.x = x_grid[u][v] + del_u*(x_grid[u12][v] - x_grid[u][v] );
  //            acc.y = y_grid[u][v] + del_u*(y_grid[u12][v] - y_grid[u][v] );
  //            acc.z = z_grid[u][v] + del_u*(z_grid[u12][v] - z_grid[u][v] );
  //
  //        }
  //    }
  //    else
  //    {
  //        if(u+1 >= 181 ) // inerpolate with longitude
  //        {
  //            acc.x = x_grid[u][v] + del_v*(x_grid[u][v12] - x_grid[u][v] );
  //            acc.y = y_grid[u][v] + del_v*(y_grid[u][v12] - y_grid[u][v] );
  //            acc.z = z_grid[u][v] + del_v*(z_grid[u][v12] - z_grid[u][v] );
  //        }
  //        else  // normal situation
  //        {
  //
  //            //find the 4 points for the grid and do the interpolation
  //            double row1 =0.0, row2 =0.0;
  //            row1 = x_grid[u][v] + del_v*(x_grid[u][v+1] - x_grid[u][v] );
  //            row2 = x_grid[u][v] + del_v*(x_grid[u+1][v] - x_grid[u][v] );
  //            acc.x =  row1 + (row2 - row1) * del_u ;
  //
  //            row1 = y_grid[u][v] + del_v*(y_grid[u][v+1] - y_grid[u][v] );
  //            row2 = y_grid[u][v] + del_v*(y_grid[u+1][v] - y_grid[u][v] );
  //            acc.y =  row1 + (row2 - row1) * del_u ;
  //
  //            row1 = z_grid[u][v] + del_v*(z_grid[u][v+1] - z_grid[u][v] );
  //            row2 = z_grid[u][v] + del_v*(z_grid[u+1][v] - z_grid[u][v] );
  //            acc.z =  row1 + (row2 - row1) * del_u ;
  //        }
  //
  //    }
  //
  //

  return acc;
}

GVector bilinear_interp(double x_grid[181][361], double y_grid[181][361],
                        double z_grid[181][361], double longitude,
                        double latitude) {
  double min_longitude = -180, max_longitude = 180;
  double min_latitude = -90, max_latitude = 90;
  GVector force;
  // Check input for validity.
  if (longitude < min_longitude) {
    longitude = min_longitude;
  } else if (longitude > max_longitude) {
    longitude = max_longitude;
  }

  if (latitude < min_latitude) {
    latitude = min_latitude;
  } else if (latitude > max_latitude) {
    latitude = max_latitude;
  }

  // Raw horizontal and vertical indexed in array (0 indexing).
  double u_raw = longitude - min_longitude;
  double v_raw = latitude - min_latitude;

  double u_floor = std::floor(u_raw);  // longitude
  double v_floor = std::floor(v_raw);  // latitude

  double del_u = u_raw - u_floor;
  double del_v = v_raw - v_floor;

  double tol = 0x1.0p-30;  // ~10^-9

  uint_fast16_t i1, i2, j1, j2;

  i1 = static_cast<uint_fast16_t>(v_floor);  // latitude
  i2 = i1 + 1;

  j1 = static_cast<uint_fast16_t>(u_floor);  // longtidue
  j2 = j1 + 1;

  // Perform interpolation based on appropriate neighbors:

  if (del_u > tol) {
    if (del_v > tol) {
      // Interpolation point falls between grid points in both directions:
      double row1, row2;

      // Horizontal interpolation along upper border.
      row1 = (x_grid[i1][j2] - x_grid[i1][j1]) * del_u + x_grid[i1][j1];
      // Horizontal interpolation along lower border.
      row2 = (x_grid[i2][j2] - x_grid[i2][j1]) * del_u + x_grid[i2][j1];
      // Vertical interpolation (downward) between row1 and row2.
      force.x = (row2 - row1) * del_v + row1;

      row1 = (y_grid[i1][j2] - y_grid[i1][j1]) * del_u + y_grid[i1][j1];
      row2 = (y_grid[i2][j2] - y_grid[i2][j1]) * del_u + y_grid[i2][j1];
      force.y = (row2 - row1) * del_v + row1;

      row1 = (z_grid[i1][j2] - z_grid[i1][j1]) * del_u + z_grid[i1][j1];
      row2 = (z_grid[i2][j2] - z_grid[i2][j1]) * del_u + z_grid[i2][j1];
      force.z = (row2 - row1) * del_v + row1;

    } else {
      // Interpolation point (pretty much) falls on horizontal grid line:
      // 1D rightward interpolation (increasing longitude).
      force.x = (x_grid[i1][j2] - x_grid[i1][j1]) * del_u + x_grid[i1][j1];
      force.y = (y_grid[i1][j2] - y_grid[i1][j1]) * del_u + y_grid[i1][j1];
      force.z = (z_grid[i1][j2] - z_grid[i1][j1]) * del_u + z_grid[i1][j1];
    }
  } else {
    if (del_v > tol) {
      // Interpolation point (pretty much) falls on vertical grid line:
      // 1D downward interpolation (increasing latitude).
      force.x = (x_grid[i2][j1] - x_grid[i1][j1]) * del_v + x_grid[i1][j1];
      force.y = (y_grid[i2][j1] - y_grid[i1][j1]) * del_v + y_grid[i1][j1];
      force.z = (z_grid[i2][j1] - z_grid[i1][j1]) * del_v + z_grid[i1][j1];
    } else {
      // Interpolation point falls on grid node exactly (ish):
      force.x = x_grid[i1][j1];
      force.y = y_grid[i1][j1];
      force.z = z_grid[i1][j1];
    }
  }

  return force;
}  // end of bilinear_interp

int main(int argc, char* argv[]) {
  // userfile
  //    argv[1] =
  //    "/Users/lizhen/projects/mySRP/mySRP/GalileoIOV/GalileoIOV.txt";
  //    // reflection number
  //    argv[2] = "1";
  //    // pixel resolution
  //    argv[3] = "0.01";
  //
  //    argv[4] = "100";
  //
  //    argv[5] =
  //    "/Users/lizhen/projects/mySRP/mySRP/GalileoIOV/xgrid_GalileoIOV.grd";
  //    argv[6] =
  //    "/Users/lizhen/projects/mySRP/mySRP/GalileoIOV/ygrid_GalileoIOV.grd";
  //    argv[7] =
  //    "/Users/lizhen/projects/mySRP/mySRP/GalileoIOV/zgrid_GalileoIOV.grd";
  //    argv[8] = "696.815";

  // initialise the ray tracer
  GRayTracer myraytracer;

  myraytracer.objects.loadUserfile(argv[1]);
  int max_depth = atoi(argv[2]);
  GRayTracer::MAX_DEPTH = max_depth;
  GPixelArray::pixelSize = atof(argv[3]);
  //一定要重设面积元素
  GPixelArray::pixelArea = GPixelArray::pixelSize * GPixelArray::pixelSize;

  int N = atoi(argv[4]);

  std::string filename_grid_x = argv[5];
  std::string filename_grid_y = argv[6];
  std::string filename_grid_z = argv[7];
  double mass = atof(argv[8]);

  std::vector<double> lat_N;
  std::vector<double> lon_N;
  lat_N.reserve(N);
  lon_N.reserve(N);

  std::vector<GVector> diff;
  diff.reserve(N);

  double data_x[181][361], data_y[181][361], data_z[181][361];

  myraytracer.makeTree();

  // 1. generate the random latitude and longtitude

  // latitude
  // srand((unsigned)time(NULL));
  for (int i = 0; i < N; i++) {
    // in the range of -90 to 90
    double x = rand() / (double)(RAND_MAX / 180.0) - 89.0;
    lat_N.push_back(x);
  }
  // longitude
  // srand((unsigned)time(NULL));
  for (int i = 0; i < N; i++) {
    // in the range of -180 to 180
    double x = rand() / (double)(RAND_MAX / 360.0) - 179.0;
    lon_N.push_back(x);
  }

  // 2. reading in the grid files
  readGridFile(filename_grid_x, data_x);
  readGridFile(filename_grid_y, data_y);
  readGridFile(filename_grid_z, data_z);

  for (int i = 0; i < N; i++) {
    // get force by interpolation
    GVector force_grid =
        bilinear_interp(data_x, data_y, data_z, lon_N[i], lat_N[i]);
    // GVector force_grid = mybilinear_interp(data_x, data_y, data_z, lon_N[i],
    // lat_N[i]);

    // get the force by ray tracing
    double lon_rad = lon_N[i] * M_PI / 180.0;
    double lat_rad = lat_N[i] * M_PI / 180.0;

    myraytracer.rayIntersected = 0;
    myraytracer.area_hit = 0.0;

    myraytracer.pixelarray.buildPixelArray(lat_rad, lon_rad,
                                           myraytracer.objects.boundingBox);

    // myraytracer.pixelarray.buildPixelArray(-0.1*M_PI/180, 145*M_PI/180.0,
    // myraytracer.objects.boundingBox);

    GVector force_ray = myraytracer.run();

    diff.push_back((force_grid - force_ray) * 1.0E9 / mass);
  }

  // calculate the rms, std, and average
  GVector mean;
  for (int i = 0; i < N; i++) {
    mean += diff[i];
  }
  mean = mean / N;

  GVector rms;
  for (int i = 0; i < N; i++) {
    // GVector a = diff[i];
    GVector a = diff[i] - mean;
    rms += GVector(a.x * a.x, a.y * a.y, a.z * a.z);
  }
  rms = rms / N;
  rms.x = sqrt(rms.x);
  rms.y = sqrt(rms.y);
  rms.z = sqrt(rms.z);

  // output results into a file to plot
  FILE* pf = fopen("userAlgorithmValidation.txt", "w+");

  fprintf(pf, "lat,lon,diff_x,diff_y,diff_z\n");
  for (int i = 0; i < N; i++) {
    fprintf(pf, "%12.5f,%12.5f,%20.12E,%20.12E,%20.12E\n", lat_N[i], lon_N[i],
            diff[i].x, diff[i].y, diff[i].z);
  }
  fclose(pf);

  printf("mean: %.4E %.4E %.4E\n", mean.x, mean.y, mean.z);
  printf("std: %.4E %.4E %.4E\n", rms.x, rms.y, rms.z);

  return 0;
}
