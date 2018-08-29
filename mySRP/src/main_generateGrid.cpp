#include <stdio.h>
#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

void output(FILE* pf, double data[181][361]) {
  fprintf(pf, "grid generator: myRayTracer:grid, value unit: Newton\n");
  fprintf(pf, "361 181\n");
  fprintf(pf, "-180 180\n");
  fprintf(pf, "-90 90\n");
  fprintf(pf, "0.0 0.0\n");
  int kindex = 0;
  int lat = 0, lon = 0;

  for (int i = 0; i < 181; i++)  // from -90 to 90
  {
    for (int j = 0; j <= 360; j++)  // from 0 to 360
    {
      // kindex = j + 360;
      // if(kindex > 360 )  kindex = kindex -360;
      fprintf(pf, "%16.15E ", data[i][j]);
    }
    fprintf(pf, "\n");
  }
}

// void getFiles( string path, vector<string>& files )
//{
////文件句柄
// long   hFile   =   0;
////文件信息
// struct _finddata_t fileinfo;
// string p;
//    if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=
//    -1)
//    {
//        do
//        {
//         //如果是目录,迭代之
//          //如果不是,加入列表
//         if((fileinfo.attrib &  _A_SUBDIR))
//          {
//           if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..")
//           != 0)
//                    getFiles(
//                    p.assign(path).append("\\").append(fileinfo.name), files
//                    );
//          }
//        else
//         {
//           files.push_back(p.assign(path).append("\\").append(fileinfo.name)
//           );
//         }
//      }while(_findnext(hFile, &fileinfo)  == 0);
//           _findclose(hFile);
//    }
//}

// the function that reads data files named with Longitude only
void readData_v2(std::string dataPath, double data_x[181][361],
                 double data_y[181][361], double data_z[181][361]) {
  double lat = 0.0, lon = 0.0;
  long ray_intersected, totalray;
  double ratio, area_hit, pixelarray_x, pixelarray_y;
  double x, y, z, magnitude;
  bool copy_set = false;  // 180 存在, -180不存在
  std::vector<int> latitudes;
  for (int j = 0; j <= 360; j++)  // from -180 to 180
  {
    std::string filename;
    lon = double(j) - 180.0;
    char testFN[1024] = {0};
    sprintf(testFN, "LON%06.2f.txt", lon);
    // printf("%s\n", testFN);
    filename = testFN;
    filename = dataPath + filename;

    FILE* pf = fopen(filename.c_str(), "r");
    if (pf == NULL) {
      if (lon == 180) {
        copy_set = true;
      }
      printf("WARNING data file %s open failed\n", filename.c_str());
      continue;
    }
    latitudes.clear();
    // ignore the first line
    fgets(testFN, 1024, pf);

    while (fgets(testFN, sizeof(char) * 1024, pf)) {
      sscanf(testFN, "%lf %lf %ld %ld %lf %lf %lf %lf %lf %lf %lf %lf\n", &lat,
             &lon, &ray_intersected, &totalray, &ratio, &area_hit,
             &pixelarray_x, &pixelarray_y, &x, &y, &z, &magnitude);

      latitudes.push_back(int(lat));

      // printf("%lf %lf\n", lat, lon);
      int lat_index = lat + 90;
      data_x[lat_index][j] = x;
      data_y[lat_index][j] = y;
      data_z[lat_index][j] = z;
    }

    fclose(pf);
  }

  // for longitude 180, should be the same as -180
  for (int k = 0; k < latitudes.size(); k++) {
    int lat_index = latitudes[k] + 90;
    if (copy_set == true)  // -180 存在, 180不存在
    {
      data_x[lat_index][360] = data_x[lat_index][0];
      data_y[lat_index][360] = data_y[lat_index][0];
      data_z[lat_index][360] = data_z[lat_index][0];
    } else if (copy_set == false)  // 180 存在, -180不存在
    {
      data_x[lat_index][0] = data_x[lat_index][360];
      data_y[lat_index][0] = data_y[lat_index][360];
      data_z[lat_index][0] = data_z[lat_index][360];
    }
  }
}

// read in the output files named with both Longitude and Latitude
// There is only one record inside these output files
void readData_v1(std::string dataPath, double data_x[181][361],
                 double data_y[181][361], double data_z[181][361]) {
  double lat = 0.0, lon = 0.0;
  long ray_intersected, totalray;
  double ratio, area_hit, pixelarray_x, pixelarray_y;
  double x, y, z, magnitude;
  for (int i = 0; i <= 180; i++)  // latitude -90 to 90
  {
    lat = i - 90;
    for (int j = 0; j <= 360; j++)  // form -180 to 180
    {
      lon = j - 180;

      char testFN[1024] = {0};
      sprintf(testFN, "LAT%06.2fLON%06.2f.txt", lat, lon);
      // printf("%s\n", testFN);
      std::string filename = testFN;
      filename = dataPath + filename;
      FILE* pf = fopen(filename.c_str(), "r");
      if (pf == NULL) {
        printf("WARNING data file %s open failed\n", filename.c_str());
        continue;
      }

      memset(testFN, sizeof(char) * 1024, 0);
      // ignore the first line
      fgets(testFN, 1024, pf);
      // printf("%s\n", testFN);
      fscanf(pf, "%lf %lf %ld %ld %lf %lf %lf %lf %lf %lf %lf %lf\n", &lat,
             &lon, &ray_intersected, &totalray, &ratio, &area_hit,
             &pixelarray_x, &pixelarray_y, &x, &y, &z, &magnitude);

      // printf("%lf %lf\n", lat, lon);
      data_x[i][j] = x;
      data_y[i][j] = y;
      data_z[i][j] = z;
    }
  }
}

int main(int argc, char* argv[]) {
  // argv[1]="./";
  // argv[2]="test";
  double data_x[181][361] = {
      {0.0}};  // latitude from -90-90, longitude from 0-360
  double data_y[181][361] = {
      {0.0}};  // latitude from -90-90, longitude from 0-360
  double data_z[181][361] = {
      {0.0}};  // latitude from -90-90, longitude from 0-360

  std::string dataPath(argv[1]);  //"galileoFOC/";
  readData_v2(dataPath, data_x, data_y, data_z);

  // printf("read good\n");

  std::string spacecraftName = argv[2];

  std::string xpath = dataPath + "xgrid_";
  xpath += spacecraftName;
  xpath += ".grd";

  std::string ypath = dataPath + "ygrid_";
  ypath += spacecraftName;
  ypath += ".grd";

  std::string zpath = dataPath + "zgrid_";
  zpath += spacecraftName;
  zpath += ".grd";

  // output parts
  FILE* xgrid = fopen(xpath.c_str(), "w+");
  output(xgrid, data_x);
  fclose(xgrid);
  FILE* ygrid = fopen(ypath.c_str(), "w+");
  output(ygrid, data_y);
  fclose(ygrid);
  FILE* zgrid = fopen(zpath.c_str(), "w+");
  output(zgrid, data_z);
  fclose(zgrid);

  printf("GRID file generated successfully!\n");
  printf("%s\n", xpath.c_str());
  printf("%s\n", ypath.c_str());
  printf("%s\n", zpath.c_str());
}

//
// int main(int argc, char* argv[])
//{
//
//
//	double data_x[181][361]={{0.0}}; //latitude from -90-90, longitude from
// 0-360 	double data_y[181][361]={{0.0}}; //latitude from -90-90,
// longitude from 0-360 	double data_z[181][361]={{0.0}}; //latitude from
// -90-90, longitude from 0-360
//
//	std::string dataPath(argv[1]);   //"galileoFOC/";
//	//printf("%s\n", dataPath.c_str());
//	double lat = 0.0, lon = 0.0;
//	long ray_intersected, totalray;
//	double ratio,area_hit, pixelarray_x, pixelarray_y;
//	double x,y,z,magnitude;
//	bool copy_set = false; // 180 存在, -180不存在
//	for( int j = 0 ; j<=360; j++ ) // form -180 to 180
//	{
//		std::string filename;
//		lon = double(j)-180.0;
//		char testFN[1024]={0};
//    	sprintf(testFN, "LON%06.2f.txt",lon);
//    	//printf("%s\n", testFN);
//    	filename = testFN;
//    	filename = dataPath + filename;
//
//    	FILE* pf = fopen(filename.c_str(),"r");
//    	if(pf == NULL )
//    	{
//    		if(lon==180)
//    			{
//    				copy_set = true;
//    			}
//    		printf("WARNING data file %s open failed\n", filename.c_str());
//    		continue;
//    	}
//
//    	//ignore the first line
//    	fgets(testFN,1024,pf);
//    	//printf("%s\n", testFN);
//    	for( int i = 0 ; i< 181; i++ ) // from -90 to 90
//    	{
//    		fscanf(pf,"%lf %lf %ld %ld %lf %lf %lf %lf %lf %lf %lf %lf\n",
//    			   &lat,&lon,
//    &ray_intersected,&totalray,&ratio,&area_hit,&pixelarray_x,&pixelarray_y,
//    			   &x,&y,&z,&magnitude
//    			  );
//
//    		//printf("%lf %lf\n", lat, lon);
//    		data_x[i][j] = x;
//    		data_y[i][j] = y;
//    		data_z[i][j] = z;
//    	}
//
//    	fclose(pf);
//	}
//
//  printf("copy_set: %d\n", copy_set);
//	// for longitude 180, should be the same as -180
//	for(int k = 0 ; k< 181; k++ )
//	{
//		if(copy_set == true) // -180 存在, 180不存在
//			{
//				data_x[k][360] = data_x[k][0];
//   			data_y[k][360] = data_y[k][0];
//   			data_z[k][360] = data_z[k][0];
//
//			}
//			else if(copy_set==false) // 180 存在, -180不存在
//			{
//				data_x[k][0] = data_x[k][360];
//   			data_y[k][0] = data_y[k][360];
//   			data_z[k][0] = data_z[k][360];
//			}
//
//	}
//
//
//
//	std::string xpath=dataPath+"xgrid_new.grd";
//	std::string ypath=dataPath+"ygrid_new.grd";
//	std::string zpath=dataPath+"zgrid_new.grd";
//
//	//output parts
//	FILE* xgrid = fopen(xpath.c_str(),"w+");
//	output(xgrid, data_x);
//	fclose(xgrid);
//	FILE* ygrid = fopen(ypath.c_str(),"w+");
//	output(ygrid, data_y);
//	fclose(ygrid);
//	FILE* zgrid = fopen(zpath.c_str(),"w+");
//	output(zgrid, data_z);
//	fclose(zgrid);
//
//
//
//
//
//
//
//	cout<<"grid file generated!"<<endl;
//}
