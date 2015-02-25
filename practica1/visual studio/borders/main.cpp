/*
 ============================================================================
 Name        : practica1.c
 Description : Detect contrasting borders in BMP and save it into new file.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include "practica1_structs.h"
#include "practica1_methods.h"

IMAGE imagenfte,imagendst;

#define DIF 16
#define NHILOS 4

typedef struct ThreadInfo {
   int nThread;
   int fromRow;
   int status;
   int toRow;
   int imageCols;
   IMAGE *imagefte;//Source
   IMAGE *imagedst;//Target
} TDATA, *PTDATA;

void processBMP(IMAGE *imagefte, IMAGE *imagedst);
DWORD WINAPI threadProcessBMP( LPVOID lpParam );

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int _tmain()
{
  // variables para manejo de tiempo
  clock_t t_inicial,t_final;

  int res; // variable de resultado de procesos

  char namedest[100]; // variable para el nombre del destino

  // obtener el tiempo inicial
  t_inicial=clock();

  // generar los nombres de los archivos fuente y destino
  strcpy(namedest, strtok(filename,"."));
  strcat(filename,".bmp");
  strcat(namedest,"_secuencial.bmp");

  printf("Archivo fuente %s\n",filename);
  printf("Archivo destino %s\n",namedest);

  // cargar el archivo en memoria
  res = loadBMP(filename, &imagenfte);
  // verificar el cargado
  if(res == -1)
  {
    fprintf(stderr, "Error al abrir imagen\n");
    exit(1);
  }

  printf("Procesando imagen de: Renglones = %d, Columnas =%d\n", imagenfte.infoheader.rows, imagenfte.infoheader.cols);
  // ejecutar el procesamiento
  processBMP(&imagenfte, &imagendst);

  //
  res = saveBMP(namedest, &imagendst);
  // verificar la escritura
  if(res == -1)
  {
    fprintf(stderr, "Error al escribir imagen\n");
    exit(1);
  }

  // obtener el tiempo final
  t_final=clock();

  // calcular e imprimir tiempo
	printf("Tiempo %3.6f segundos\n",((float) t_final- (float)t_inicial)/ CLOCKS_PER_SEC);
}

void processBMP(IMAGE *imagefte, IMAGE *imagedst)
{
  // copia los encabezados de la imágen fuente a la imágen destino
  memcpy(imagedst,imagefte,sizeof(IMAGE)-sizeof(PIXEL *));
  int imageRows = imagefte->infoheader.rows;
  int imageCols = imagefte->infoheader.cols;
  imagedst->pixel=(PIXEL *)malloc(sizeof(PIXEL)*imageRows*imageCols);

  PTDATA tinfo[NHILOS];
  DWORD   dwThreadIdArray[NHILOS];
  HANDLE  hThreadArray[NHILOS];

  int i;

  for(i=0; i<NHILOS; i++)
  {
	  tinfo[i] = (PTDATA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,sizeof(TDATA));
    /*
    * assign rows to thread
    */
    tinfo[i]->nThread = i;
    tinfo[i]->status = 1;
    if (i==0)
	tinfo[i]->fromRow = 1;
    else
	tinfo[i]->fromRow = i*(imageRows/NHILOS);
    if((i+1)>=NHILOS)
      tinfo[i]->toRow = imageRows-2;
    else
      tinfo[i]->toRow = ((i+1)*(imageRows/NHILOS))-1;
    tinfo[i]->imageCols = imageCols;
    tinfo[i]->imagefte = imagefte;
    tinfo[i]->imagedst = imagedst;

    /*
    * LAUNCH THREADS
    */
	hThreadArray[i] = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            threadProcessBMP,       // thread function name
            tinfo[i],          // argument to thread function 
            0,                      // use default creation flags 
            &dwThreadIdArray[i]);   // returns the thread identifier

    //threads[i] = CreateThread(NULL, 0, threadProcessBMP, &tinfo[i], 0, NULL);
  }

  //WAIT FOR THREADS
  WaitForMultipleObjects(NHILOS, hThreadArray,TRUE, INFINITE);
}

DWORD WINAPI threadProcessBMP( LPVOID lpParam ) 
{ 
    PTDATA tinfo= (PTDATA)lpParam;

	int imageCols= tinfo->imageCols;
    IMAGE *imagefte= tinfo->imagefte;
    IMAGE *imagedst= tinfo->imagedst;
    PIXEL *pfte,*pdst;
    PIXEL *v0,*v1,*v2,*v3,*v4,*v5,*v6,*v7;

    int i,j;
    for( i=tinfo->fromRow ; i<=tinfo->toRow ; i++)
        for(j=1;j< imageCols-1;j++)
        {
          pfte=imagefte->pixel+imageCols*i+j;
          v0=pfte-imageCols-1;
          v1=pfte-imageCols;
          v2=pfte-imageCols+1;
          v3=pfte-1;
          v4=pfte+1;
          v5=pfte+imageCols-1;
          v6=pfte+imageCols;
          v7=pfte+imageCols+1;

          pdst=imagedst->pixel+imageCols*i+j;

          if(abs(blackandwhite(*pfte)-blackandwhite(*v0))>DIF ||
          abs(blackandwhite(*pfte)-blackandwhite(*v1))>DIF ||
          abs(blackandwhite(*pfte)-blackandwhite(*v2))>DIF ||
          abs(blackandwhite(*pfte)-blackandwhite(*v3))>DIF ||
          abs(blackandwhite(*pfte)-blackandwhite(*v4))>DIF ||
          abs(blackandwhite(*pfte)-blackandwhite(*v5))>DIF ||
          abs(blackandwhite(*pfte)-blackandwhite(*v6))>DIF ||
          abs(blackandwhite(*pfte)-blackandwhite(*v7))>DIF)
          {
            pdst->red=0;
            pdst->green=0;
            pdst->blue=0;
          }
          else
          {
            pdst->red=255;
            pdst->green=255;
            pdst->blue=255;
          }


            if(i==2999)
            printf("i=%d, j=%d\n",i,j);
        }

  tinfo->status = 0;
        printf("Thread %d finished\n",tinfo->nThread);

    return 0; 
}
