#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "def.h"/*
	PI, SIN(a), COS(a), MAX, MINSPEED, MAXSPEED, MAXALT, MINDIST
	struct AVION
*/

#include "methods.h"/*
	double dist(struct AVION a,struct AVION b)
	double distmin(struct AVION a,struct AVION b)
	void inicializa_aviones(struct AVION *x)
	void actualiza_avion(struct AVION *a)
*/

struct	AVION avion[MAX];

int main()
{
	int totalWarnings=0;
	int nIteraciones;
	int i,j;

	clock_t t_inicial,t_final;
	inicializa_aviones(avion);
	t_inicial=clock();

	//cycle
	for(nIteraciones=0;nIteraciones<2000;nIteraciones++)
	{
		//trabaja
		for(i=0 ;i<MAX-1 ;i++ )
		{
			for(j=i;j<MAX;j++)
				if(i!=j)
					if(dist(avion[i],avion[j])<distmin(avion[i],avion[j]))
					{
						avion[i].warnings++;
						avion[j].warnings++;
						totalWarnings++;
					}
		}

		//sucede unidad de tiempo
		//Sleep(10);
		for(i=0;i<MAX;i++)
			actualiza_avion(&avion[i]);

		//resultado parcial
		if(nIteraciones%100==0)
			printf("Total Warnings = %d\n",totalWarnings);
	}

	t_final=clock();
	printf("En %3.6f segundos\n",((float) t_final- (float)t_inicial)/ CLOCKS_PER_SEC);

	//Sleep(3000);
	return 0;
}
