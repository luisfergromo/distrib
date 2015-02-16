/*
 ============================================================================
 Name        : practica1.c
 Author      : Romina Espinosa
 Description : Window Thread version.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <windows.h>
#include "practica1_structs.h"
#include "practica1_methods.h"

#define DIF 16
#define NHILOS 1

struct thread_info {
   int nThread;
   int fromRow;
   int status;
   int toRow;
   int imageCols;
   IMAGE *imagefte;//Source
   IMAGE *imagedst;//Target
};

//METHOD DEFINITION
void processBMP(IMAGE *imagefte, IMAGE *imagedst);
DWORD WINAPI threadProcessBMP(LPVOID arg );

int main()
{
  clock_t t_inicial,t_final;
  IMAGE imagefte;//Imagen fuente
  IMAGE imagedst;//Imagen destino

  //file name
  char namedest[80];
  strcpy(namedest,strtok(filename,"."));
  strcat(filename,".bmp");
  strcat(namedest,"_P.bmp");
  printf("Source file: %s\n",filename);
  printf("Target file: %s\n",namedest);

  t_inicial=clock();

  //load
  if(loadBMP(filename,&imagefte)==-1)
  {
    fprintf(stderr,"ERROR: Couldn't open image\n");
    exit(1);
  }

  printf("\nProcessing image: ROWS= %d, COLUMNS= %d\n\n",imagefte.infoheader.rows,imagefte.infoheader.cols);

  //process
  processBMP(&imagefte,&imagedst);

  //save
  puts("Saving...");
  if(saveBMP(namedest,&imagedst)==-1)
  {
    fprintf(stderr,"Error al escribir imagen\n");
    exit(1);
  }


  t_final=clock();
  printf("------------------------------\n");
  printf("Tiempo %3.6f segundos\n",((float) t_final- (float)t_inicial)/ CLOCKS_PER_SEC);

}

/**
*main thread
*@method processBMP
*/
void processBMP(IMAGE *imagefte, IMAGE *imagedst)
{
  // copia los encabezados de la imágen fuente a la imágen destino
  memcpy(imagedst,imagefte,sizeof(IMAGE)-sizeof(PIXEL *));
  int imageRows = imagefte->infoheader.rows;
  int imageCols = imagefte->infoheader.cols;
  imagedst->pixel=(PIXEL *)malloc(sizeof(PIXEL)*imageRows*imageCols);

  HANDLE threads[NHILOS];
  struct thread_info tinfo[NHILOS];
  int i;

  for(i=0; i<NHILOS; i++)
  {
    /*
    * assign rows to thread
    */
    tinfo[i].nThread = i;
    tinfo[i].status = 1;
    if (i==0)
	tinfo[i].fromRow = 1;
    else
	tinfo[i].fromRow = i*(imageRows/NHILOS);
    if((i+1)>=NHILOS)
      tinfo[i].toRow = imageRows-2;
    else
      tinfo[i].toRow = ((i+1)*(imageRows/NHILOS))-1;
    tinfo[i].imageCols = imageCols;
    tinfo[i].imagefte = imagefte;
    tinfo[i].imagedst = imagedst;

    /*
    * LAUNCH THREADS
    */
    threads[i] = CreateThread(NULL, 0, threadProcessBMP, &tinfo[i], 0, NULL);
  }

  //WAIT FOR THREADS
  WaitForMultipleObjects(NHILOS, threads,TRUE, INFINITE);
}

/**
*individual thread
*@method threadProcessBMP
*/
DWORD WINAPI threadProcessBMP(LPVOID arg )
{
    struct thread_info *tinfo = arg;
    printf("Thread %d working from %d to %d Row\n",tinfo->nThread,tinfo->fromRow,tinfo->toRow);

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
}
