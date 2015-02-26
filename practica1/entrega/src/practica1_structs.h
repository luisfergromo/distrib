#define DIF 16

// NOMBRE DEL ARCHIVO A PROCESAR

char filename[]="C:/Users/espnosa/iteso/distribuidos/practica1/img/rocks.bmp";

#pragma pack(2) // Empaquetado de 2 bytes
typedef struct {
  // firma
  unsigned char magic1; // 'B'
  unsigned char magic2; // 'M'

  unsigned int size; // Tamaño
  unsigned short int reserved1, reserved2; // campo reservado
  unsigned int pixelOffset; // offset a la imagen
} HEADER;

#pragma pack() // Empaquetamiento por default
typedef struct {
  unsigned int size;// Tamaño de este encabezado INFOHEADER
  int cols, rows;// Renglones y columnas de la imagen
  unsigned short int planes; // planos (siempre 1)
  unsigned short int bitsPerPixel; // profundidad (Bits por pixel)
  unsigned int compression; // tipo de compresión
  unsigned int cmpSize; // tamaño en bytes de la imágen
  int xScale, yScale; // resolución horizontal y vertical
  unsigned int numColors; // número de colores de la paleta
  unsigned int importantColors; // número de colores importantes de la paleta (0 es todos)
} INFOHEADER;

typedef struct {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
} PIXEL;

typedef struct {
  HEADER header; // encabezado de archivo
  INFOHEADER infoheader; // encabezado de información
  PIXEL *pixel; // cuerpo de datos
} IMAGE;
