#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <windows.h>

#define THREADS 4
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

struct AVION avion[MAX];
int totwarnings=0;
CRITICAL_SECTION sc;

double dist(struct AVION a,struct AVION b)
{
	double dist_x,dist_y,dist_z,dist;
	dist_x=a.pos_x-b.pos_x;
	dist_y=a.pos_y-b.pos_y;
	dist_z=a.pos_z-b.pos_z;
	dist=sqrt(dist_x*dist_x+dist_y*dist_y+dist_z*dist_z);
	return(dist);
}

double distmin(struct AVION a,struct AVION b)
{
	double speed;
	double distmin;
	if(a.currspeed>b.currspeed)
		speed=a.currspeed/3.6;
	else
		speed=b.currspeed/3.6;
	distmin=MINDIST+speed*speed*1.6;
	return(distmin);
}

void inicializa_aviones(struct AVION *x)
{
	int i;
	for(i=0;i<MAX;i++)
	{
		x[i].pos_x=1000*(5000.0 - (double) (rand()%10000));
		x[i].pos_y=1000*(5000.0 - (double) (rand()%10000));
		x[i].pos_z=(double) (rand()%13000);
		x[i].currspeed=MINSPEED+x[i].pos_z/21;
		x[i].dir=(double) (rand()%360);
	}
	return;
}

// Se actualiza cada 1/1000 segundos
void actualiza_avion(struct AVION *a)
{
	if(a->pos_z<13000.0)
	a->pos_z+=0.001;
	a->currspeed=MINSPEED+a->pos_z/21;
	a->pos_x+=0.001*(a->currspeed/3.6)*SIN(a->dir);
	a->pos_y+=0.001*(a->currspeed/3.6)*COS(a->dir);
	return;
}

DWORD WINAPI hilo(LPVOID arg)
{
	int hnilo=*((int *) arg);
	int rinic=(MAX/THREADS)*hnilo;
	int rfin=rinic+(MAX/THREADS);
	int i,j;
	if(hnilo==THREADS-1)
		rfin--;
	// printf("Hilo=%d, inicio=%d, fin=%d\n",hnilo,rinic,rfin);
	for(i=rinic;i<rfin;i++)
	{
		for(j=i;j<MAX;j++)
			if(i!=j)
				if(dist(avion[i],avion[j])<distmin(avion[i],avion[j]))
				{
					EnterCriticalSection(&sc);
					avion[i].warnings++;
					avion[j].warnings++;
					totwarnings++;
					LeaveCriticalSection(&sc);
				}
	}
	return 0;
}

int main()
{
	int n;
	int i,j;
	int t;
	int par[THREADS];
	HANDLE tids[THREADS];
	clock_t t_inicial,t_final;
	inicializa_aviones(avion);
	InitializeCriticalSection(&sc);
	t_inicial=clock();
	for(n=0;n<2000;n++)
	{
		for(t=0;t<THREADS;t++)
		{
			par[t]=t;
			tids[t]=CreateThread(NULL,0,hilo,&par[t],0,NULL);
		}
		WaitForMultipleObjects(THREADS,tids,TRUE,INFINITE);
		for(i=0;i<MAX;i++)
			actualiza_avion(&avion[i]);
		if(n%100==0)
			printf("Total Warnings = %d\n",totwarnings);
	}
	t_final=clock();
	printf("En %3.6f segundos\n",((float) t_final- (float)t_inicial)/ CLOCKS_PER_SEC);
	DeleteCriticalSection(&sc);

	return 0;
}