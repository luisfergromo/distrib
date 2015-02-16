/*
 ============================================================================
 Name        : practica1.c
 Description : Detect contrasting borders in BMP and save it into new file.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "practica1_structs.h"
#include "practica1_methods.h"

IMAGE imagenfte,imagendst;

void processBMP(IMAGE *imagefte, IMAGE *imagedst)
{
  int i,j; // indicadores de posición actual
  PIXEL *pfte,*pdst; // apuntadores para el pixel actual
  PIXEL *v0,*v1,*v2,*v3,*v4,*v5,*v6,*v7; // apuntadores para los pixeles vecinos
  int imageRows, imageCols; // límites para los contadores

  // copia los encabezados de la imágen fuente a la imágen destino
  memcpy(imagedst, imagefte, sizeof(IMAGE)-sizeof(PIXEL *));
  // obten los límites para los contadores de filas y columnas
  imageRows = imagefte->infoheader.rows;
  imageCols = imagefte->infoheader.cols;

  // reserva la memoria en la imágen destino para los pixeles
  imagedst->pixel = (PIXEL *) malloc(sizeof(PIXEL)*imageRows*imageCols);

  // para cada fila
  for(i = 1; i < imageRows-1; i++)
    // para cada columna
    for(j = 1; j < imageCols-1; j++)
      {
        // encuentra al pixel actual
        pfte = imagefte->pixel+imageCols*i+j;

        // encuentra el pixel destino
        pdst=imagedst->pixel+imageCols*i+j;

        // encuentra a los vecinos
        v0=pfte-imageCols-1;  // renglon arriba a la izquierda
        v1=pfte-imageCols;    // renglon arriba
        v2=pfte-imageCols+1;  // renglon arriba a la derecha
        v3=pfte-1;            // mismo renglon a la izquierda
        v4=pfte+1;            // mismo renglon a la derecha
        v5=pfte+imageCols-1;  // renglon abajo a la izquierda
        v6=pfte+imageCols;    // renglon abajo
        v7=pfte+imageCols+1;  // renglon abajo a la derecha

        // si hay un cambio de tono mayor al DIF entre el pixel fuente y algun vecino
        if(abs(blackandwhite(*pfte)-blackandwhite(*v0))>DIF ||
        abs(blackandwhite(*pfte)-blackandwhite(*v1))>DIF ||
        abs(blackandwhite(*pfte)-blackandwhite(*v2))>DIF ||
        abs(blackandwhite(*pfte)-blackandwhite(*v3))>DIF ||
        abs(blackandwhite(*pfte)-blackandwhite(*v4))>DIF ||
        abs(blackandwhite(*pfte)-blackandwhite(*v5))>DIF ||
        abs(blackandwhite(*pfte)-blackandwhite(*v6))>DIF ||
        abs(blackandwhite(*pfte)-blackandwhite(*v7))>DIF)
        {
          // pinta el pixel de negro
          pdst->red=0;
          pdst->green=0;
          pdst->blue=0;
        }
        else // si no hay cambio
        {
          // pinta el pixel de blanco
          pdst->red=255;
          pdst->green=255;
          pdst->blue=255;
        }
      }
}

int main()
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
