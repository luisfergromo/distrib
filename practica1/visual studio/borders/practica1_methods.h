int loadBMP(char *filename, IMAGE *image)
{
  FILE* fin; // apuntador al flujo del archivo

  int i = 0; // contador de lectura
  int totpixs = 0; // límite para el contador de lectura

  // abrir el flujo al archivo como binary read/update
  fopen_s(&fin,filename,"rb+");

  // Si el archivo no existe
  if (fin == NULL)
    return(-1);

  // Leer encabezado de archivo
  fread(&image->header, sizeof(HEADER), 1, fin);

  // Probar si es un archivo BMP
  if (!((image->header.magic1 == 'B') && (image->header.magic2 == 'M')))
    return(-1);

  // leer encabezado de información
  fread(&image->infoheader, sizeof(INFOHEADER), 1, fin);

  // Probar si es un archivo BMP 24 bits no compactado
  if (!((image->infoheader.bitsPerPixel == 24) && !image->infoheader.compression))
    return(-1);

  // reservar memoria para el cuerpo de la imágen
  // (tamaño de pixel) x columnas x filas
  image->pixel = (PIXEL *) malloc(sizeof(PIXEL)*image->infoheader.cols*image->infoheader.rows);

  // definir el número de pixeles a leer: filas x columnas
  totpixs = image->infoheader.rows*image->infoheader.cols;

  // mientras no se haya alcanzado el total de pixeles
  while(i < totpixs)
  {
    // leer un bloque de 512 pixeles
    fread(image->pixel+i, sizeof(PIXEL), 512, fin);
    // aumentar el contador de lecturas
    i += 512;
  }

  return fclose(fin);
}

int saveBMP(char *filename, IMAGE *image)
{
  // apuntador al flujo de salida
  FILE *fout;

  int i = 0; // contador de lectura
  int totpixs = 0; // límite para el contador de lectura

  // abrir el flujo al archivo como binary write
  fopen_s(&fout,filename,"wb");

  // verificar creación del flujo
  if (fout == NULL)
    return(-1);// Error

  // Escribe encabezado de archivo
  fwrite(&image->header, sizeof(HEADER), 1, fout);

  // Escribe encabezado de información
  fwrite(&image->infoheader, sizeof(INFOHEADER), 1, fout);

  // definir el número de pixeles a escribir: filas x columnas
  totpixs = image->infoheader.rows*image->infoheader.cols;

  // mientras no se haya alcanzado el total de pixeles
  while(i < totpixs)
  {
    // escribir un bloque de 512 pixeles
    fwrite(image->pixel+i, sizeof(PIXEL), 512, fout);
    // aumentar el contador de lecturas
    i += 512;
  }

  return fclose(fout);
}

unsigned char blackandwhite(PIXEL p)
{
  // reporta el producto de una porción de 1 para cada tono (0.3 + 0.59 + 0.11 = 1.0)
  return ((unsigned char) (0.3 * ((float)p.red) + 0.59 * ((float)p.green) + 0.11 * ((float)p.blue)));
}