#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

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
#define MAX_THREADS 3
HANDLE	semaphore;//semáforo para autorizar a los threads ejecutar un nuevo ciclo
CRITICAL_SECTION cs; //seccion critica para modificar variable global totalWarnings
int totalWarnings=0;

DWORD WINAPI ThreadFunction( LPVOID lpParam )
{
	int* p = (int*)lpParam;
	int threadId = *p;

	int i,j,warningsAcc;

	while(true)
	{
		//espera inicio
		WaitForSingleObject(semaphore,INFINITE);

		//reset warnings accumulator
		warningsAcc=0;

		//trabaja
		for(i=threadId ;i<MAX-1 ;i+=MAX_THREADS )
		{
			for(j=i;j<MAX;j++)
				if(i!=j)
					if(dist(avion[i],avion[j])<distmin(avion[i],avion[j]))
					{
						avion[i].warnings++;
						avion[j].warnings++;
						warningsAcc++;
					}
		}

		//reduce
		EnterCriticalSection(&cs);
		totalWarnings+= warningsAcc;
		LeaveCriticalSection(&cs);
	}

	return 0;
}

int main()
{
	int nIteraciones;
	int i,j;

	clock_t t_inicial,t_final;
	inicializa_aviones(avion);
	t_inicial=clock();

	//init
	InitializeCriticalSection(&cs);
	semaphore = CreateSemaphore( NULL,0/*initial count*/,MAX_THREADS/*maximum count*/,NULL);

	int		threadId[MAX_THREADS];
	HANDLE  thread[MAX_THREADS];
	DWORD   dwThreadIdArray[MAX_THREADS];
	for(i=0;i<MAX_THREADS;i++)
	{
		threadId[i]=i;
		thread[i] = CreateThread(NULL,0,ThreadFunction,&threadId[i],0,&dwThreadIdArray[i]);
	}


	//cycle
	for(nIteraciones=0;nIteraciones<2000;nIteraciones++)
	{
		//permite trabajar
		ReleaseSemaphore( semaphore, MAX_THREADS, NULL);

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

	Sleep(3000);
	DeleteCriticalSection(&cs);
	return 0;
}
