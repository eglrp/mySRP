

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void output(FILE* pf, double data[181][361])
{
	fprintf(pf, "grid generator: myRayTracer:grid, value unit: Newton\n" );
	fprintf(pf, "361 181\n" );
	fprintf(pf, "-180 180\n" );
	fprintf(pf, "-90 90\n" );
	fprintf(pf, "0.0 0.0\n" );
	int kindex = 0;
	int lat =0, lon = 0;
	
	for( int i = 0 ; i< 181; i++ ) // from -90 to 90
	{

		for( int j = 0 ; j< 361; j++ ) // from 0 to 360
		{
			//kindex = j + 360;
			//if(kindex > 360 )  kindex = kindex -360;
			fprintf(pf, "%16.15E ", data[i][j]);
		}
		fprintf(pf, "\n");
	}
}

int main(int argc, char* argv[])
{

std::string file_name = argv[1]; 
double data[181][361]={0.0};

std::ifstream grid_file(file_name);
        
if (grid_file.fail() || !grid_file.is_open())
{
    std::cerr << "Error opening SRP grid file: " << file_name << std::endl;
    return false;
}


printf("convert file name: %s\n", file_name.c_str());

uint_fast16_t n_cols, n_rows; // no. of horizontal and vertical grid nodes.
double min_lon, max_lon, min_lat, max_lat; // grid file ranges.
double min_acc, max_acc;
        
        // First header line:
std::string scrap;
getline(grid_file, scrap); // DSAA designation from Surfer6 format.
        
        // Check grid resolution:
grid_file >> n_cols;
grid_file >> n_rows;
grid_file >> min_lon;
grid_file >> max_lon;
grid_file >> min_lat;
grid_file >> max_lat;
grid_file >> min_acc;
grid_file >> max_acc;

for ( uint_fast16_t i = 0; i < n_rows; ++i )
{
    for ( uint_fast16_t j = 0; j < n_cols; ++j )
        {
                grid_file >> data[i][j];
        }
}

grid_file.close();
std::string output_file = "output.grid";
FILE* grid = fopen(output_file.c_str(),"w+");
output(grid, data);
fclose(grid);

return 0;

}