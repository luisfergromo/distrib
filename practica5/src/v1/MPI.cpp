/*
 * MPI.cpp
 *
 *  Created on: Mar 16, 2015
 *      Author: federico, romina
 */

#include <mpi.h>
#include <time.h>
#define MASTER 0

//nProcess, thisN, thisName
int world_size,world_rank,name_len;
char processor_name[MPI_MAX_PROCESSOR_NAME];

//scope
clock_t t_inicial,t_final;
int initValue, finalValue;

void init();
void run();
int sumaDeFactores(int num, int isPair);

/**
* @method main
**/
int main(int argc, char** argv) {

  MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Get_processor_name(processor_name, &name_len);

  /*printf("\nRepoting from processor %s, rank %d"
			" out of %d processors",
			processor_name, world_rank, world_size);*/

  init();

  MPI_Barrier(MPI_COMM_WORLD);
	if(world_rank == MASTER)
		t_inicial=clock();

  run();

  MPI_Barrier(MPI_COMM_WORLD);
	if(world_rank == MASTER)
  {
		t_final=clock();
		printf("\nTime: %3.6f seconds\n",((float) t_final- (float)t_inicial)/
				CLOCKS_PER_SEC);
	}

	MPI_Finalize();
}

/**
* @method init
**/
void init()
{
  //Master gets initValue and finalValue
  float buf[2];
  MPI_Barrier(MPI_COMM_WORLD);
  if(world_rank == MASTER)
  {
		printf("\nIngresar valor inicial: ");
		scanf("%d",&initValue);
		printf("\nIngresar valor final: ");
		scanf("%d",&finalValue);
		buf[0]= initValue;
		buf[1]= finalValue;
	}

  //Master broadcasts
  MPI_Bcast(buf, world_size, MPI_INT, MASTER, MPI_COMM_WORLD);
	fflush(stdout);

  //Slaves save copy
  MPI_Barrier(MPI_COMM_WORLD);
	if(world_rank != MASTER){
    initValue = buf[0] + world_rank;
    finalValue = buf[1];
	}

}

/**
* @method run
**/
void run()
{
  int sum1, sum2;
	int isPair;
  int i;

	if((initValue % 2) == 0)
		isPair = 1;
	else
		isPair = -1;
	for(i = initValue; i <= finalValue; i+= world_size){
		//printf("\nnum %d from proc %d",i,world_rank);
		if((sum1 = sumaDeFactores(i, isPair)) > i){
			if(((sum1 % 2) == 0 && isPair == 1) || ((sum1 % 2) != 0 && isPair == -1) ){
				sum2 = sumaDeFactores(sum1, isPair);
				if(i == sum2) printf("\nLos numeros: %d  y %d son amigos",sum2, sum1);
			}
		}

		isPair = isPair*-1;
	}
}

/**
* @method sumaDeFactores
* @isPairam num
* @isPairam isPair
**/
int sumaDeFactores(int num, int isPair){
	if(num == 2) return 1;
	if(num == 1) return 0;

	int suma = 1;
	int end = num; //el final de la comisPairación no se vuelve a tomar en cuenta
	int i;
	int add;
	if (isPair == 1){
		end = num/2;
		add = 1;
		if(end == 2) suma +=2;
		else suma += (2 + end);
	}else add = 2;

	for(i = 3; i < end; i+= add){ // si el numero es non entonces no se verifica su divisón con isPaires
		if(num % i == 0){
			end = num/i;
			if(end == i) suma += i;
			else suma += (i + end);
		}
	}
	return suma;
}
