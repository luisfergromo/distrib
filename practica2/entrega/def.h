#define PI 3.14159
#define SIN(a) sin(a*PI/180.0)
#define COS(a) cos(a*PI/180.0)
#define MAX 1000
#define MINSPEED 300.0 // Km/h
#define MAXSPEED 900.0 // Km/h
#define MAXALT 13000.0 // Metros
#define MINDIST 4000.0 // Distancia mínima en metros entre aviones si estos están

struct AVION
{
	double pos_x;
	double pos_y;
	double pos_z;
	double currspeed;
	double dir;
	double incl;
	int warnings;
};
