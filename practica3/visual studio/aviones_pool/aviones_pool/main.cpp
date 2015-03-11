#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <windows.h>
#include <tchar.h>

#include "def.h"/*
	THREADS, PI, SIN(a), COS(a), MAX, TASKSIZE, NTASK, NITERACIONES, NOTIFY_EVERY, MINSPEED, MAXSPEED, MAXALT, MINDIST
	struct AVION
*/

#include "methods.h"/*
	void init(struct AVION *x)
	double dist(struct AVION a,struct AVION b)
	double distmin(struct AVION a,struct AVION b)
	void update(struct AVION *a)
*/

struct AVION		aviones[MAX];
int					totwarnings = 0;
CRITICAL_SECTION	sc;

PTP_POOL			pool			= NULL;
TP_CALLBACK_ENVIRON callbackEnv;
PTP_WORK			poolWork[NTASK];


VOID CALLBACK MyWorkCallback( PTP_CALLBACK_INSTANCE Instance, PVOID Parameter, PTP_WORK Work)
{
	int taskInit=*((int *) Parameter);

	int taskEnd=TASKSIZE;
	if(taskEnd>MAX)
		taskEnd=MAX;

	int i,j,warningsAcc=0;

	// printf("Hilo inicio=%d, fin=%d\n",taskInit,taskEnd);
	for(i=taskInit;i<taskEnd;i++)
		for(j=i;j<MAX;j++)
			if(i!=j)
				if(dist(aviones[i],aviones[j])<distmin(aviones[i],aviones[j]))
				{
					aviones[i].warnings++;
					aviones[j].warnings++;
					warningsAcc++;
				}

	//reduce
	EnterCriticalSection(&sc);
	totwarnings+= warningsAcc;
	LeaveCriticalSection(&sc);
}

int main()
{
	clock_t t_inicial,t_final;
	init(aviones);
	InitializeCriticalSection(&sc);
	t_inicial=clock();

	int nIteraciones;
	int i,j;

	int nTask;
	int taskInit[NTASK+1];
	
	//thread pool init
	pool= CreateThreadpool(NULL);
	InitializeThreadpoolEnvironment(&callbackEnv);

	SetThreadpoolThreadMaximum(pool, THREADS);
	SetThreadpoolCallbackPool(&callbackEnv, pool);
	
	//create tasks
	for(nTask=0;nTask<NTASK;nTask++)
	{
				taskInit[nTask]=nTask*TASKSIZE;
				poolWork[nTask] = CreateThreadpoolWork(MyWorkCallback,&taskInit[nTask],&callbackEnv);
	}

	
	//cycle
	for(nIteraciones=0;nIteraciones<NITERACIONES;nIteraciones++)
	{
		//work
		for(nTask=0;nTask<NTASK;nTask++)
			SubmitThreadpoolWork(poolWork[nTask]);

		//wait
		for(nTask=0;nTask<NTASK;nTask++)
			WaitForThreadpoolWorkCallbacks(poolWork[nTask],false);

		//update
		for(i=0;i<MAX;i++)
			update(&aviones[i]);

		//notify
		if(nIteraciones%NOTIFY_EVERY==0)
			printf("Total Warnings = %d\n",totwarnings);
	}
	
	t_final=clock();
	printf("En %3.6f segundos\n",((float) t_final- (float)t_inicial)/ CLOCKS_PER_SEC);
	DeleteCriticalSection(&sc);

	//Sleep(2000);
	return 0;
}