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

void initValues();
void run();
int sumaDeFactores(int num, bool isPair);

/**
* @method main
**/
int main(int argc, char** argv)
{
  bool sync=false;

  MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Get_processor_name(processor_name, &name_len);

  MPI_Barrier(MPI_COMM_WORLD);
    initValues();

	if(world_rank == MASTER)
		t_inicial=clock();

  run();

  MPI_Barrier(MPI_COMM_WORLD);
    sync=true;

	if(world_rank == MASTER)
  {
		t_final=clock();
		printf("\nTime: %3.6f seconds\n",((float) t_final- (float)t_inicial)/
				CLOCKS_PER_SEC);
	}

	MPI_Finalize();
}

/**
* @method initValues
**/
void initValues()
{
  int buf[2];

  if (world_rank == MASTER)
  {
    printf("\nIngresar valor inicial: ");
		scanf("%d",&initValue);
		printf("\nIngresar valor final: ");
		scanf("%d",&finalValue);
		buf[0]= initValue;
		buf[1]= finalValue;

    //printf("\n[%d] broadcasting data", world_rank);
    int i;
    for (i = 0; i < world_size; i++) {
      if (i != world_rank)
        MPI_Send(&buf, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
  }
  else
  {
    MPI_Recv(&buf, 2, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    initValue = buf[0] + world_rank;
    finalValue = buf[1];
    //printf("\n[%d] got message, starting with %d", world_rank,initValue);
  }
}

/**
* @method run
**/
void run()
{
  printf("\n[%d] is running", world_rank);

  //printf("\n[%d] is in, starting with %d", world_rank,initValue);
  bool isPair= ((initValue % 2) == 0);
  int A, sumA, B;

  for(A = initValue; A <= finalValue; A+= world_size)
  {
    sumA= sumaDeFactores(A, isPair);
    //printf("\n[%d] %d /%d ",world_rank,A,sumA);
    B= sumaDeFactores(sumA, isPair);
    if(A==B && A!=sumA)
      printf("\n[%d] Los numeros: %d  y %d son amigos",world_rank,A,sumA);
  }
  /*
  int sum1, sum2;
  int i;

	for(i = initValue; i <= finalValue; i+= world_size)
  {
    //printf("\n[%d] working with %d", world_rank,i);
		if((sum1 = sumaDeFactores(i, isPair)) > i)
			if(((sum1 % 2) == 0 && isPair ) || ((sum1 % 2) != 0 && !isPair) )
      {
				sum2 = sumaDeFactores(sum1, isPair);
				if(i == sum2) printf("\n[%d] Los numeros: %d  y %d son amigos",world_rank,sum2, sum1);
			}

		isPair = !isPair;
	}
  */

  printf("\n[%d] finished", world_rank);
}

/**
* @method sumaDeFactores
* @isPairam num
* @isPairam isPair
**/
int sumaDeFactores(int num, bool isPair)
{
	if(num == 2) return 1;
	if(num < 2) return 0;

  int sum=1;
  int factor=2;
  for(;factor<num;factor++)
    if(num%factor == 0)
      sum+=factor;

  return sum;
/*
	int suma = 1;
	int end = num; //el final de la comisPairación no se vuelve a tomar en cuenta
	int i;
	int add;
	if (isPair)
  {
		end = num/2;
		add = 1;
		if(end == 2) suma +=2;
		else suma += (2 + end);
	}else add = 2;

	for(i = 3; i < end; i+= add){ // si el numero es non entonces no se verifica su divisón con pares
		if(num % i == 0){
			end = num/i;
			if(end == i) suma += i;
			else suma += (i + end);
		}
	}
	return suma;*/
}
