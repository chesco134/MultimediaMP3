#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"milista.h"

unsigned char* loadImage(char* fileName, int* bufferSize)
{
	FILE* f = fopen(fileName,"r");
	unsigned char* bufferedImage = NULL;
	int length = 0;
	int value;
	if( f )
	{
		while(fgetc(f) != EOF)
		{
			length++;
		}
		bufferedImage = malloc(length);
		rewind(f);
		value = fread(bufferedImage,sizeof(unsigned char),length,f);
		if( value != length )
		{
			perror("Lo sentimos, otra vez será...");
		}
		fclose(f);
	}
	*bufferSize = length;
	return bufferedImage;
}

int main(int argc, char** argv)
{
	if( argc < 3 )
	{
		printf("Error, debe ingresar al menos la ubicacion de un fichero mp3 y una imagen a cargar en caso de que el primero no contenga una.\n");
		return 0;
	}
	char filename[250];
	char c;
	char hadImage = 0;
	unsigned char sc;
	unsigned char APICHeader[31];
	unsigned char jpgHeader[2];
	unsigned char id3v2TagHeader[10];
	unsigned char* newBufferedFile;
	unsigned char* bufferedFile;
	unsigned char* bufferedImage;
	int currentImage = 0;
	int bytes = 0;
	int newBufferFileLength;
	int bufferedFileLength;
	int bufferedImageLength;
	int value;
	int i;
	int chainLinkHeaderTag = 0;
	int chainLinkAPIC = 0;
	int chainLinkBegining = 0;
	int haveHeaderTag = 0;
	int haveFF = 0;
	int chainLinkEnd = 0;
	FILE* outputFile;
	FILE* mp3File = fopen(argv[1],"r+");
	ListaArchivos* lista = crearListaArchivos();
	jpgHeader[0] = 0xff;
	jpgHeader[1] = 0xd8;
	id3v2TagHeader[0] = 0x49;
	id3v2TagHeader[1] = 0x44;
	id3v2TagHeader[2] = 0x33;
	id3v2TagHeader[3] = 0x04;
	id3v2TagHeader[4] = 0x00;
	id3v2TagHeader[5] = 0x00;
	id3v2TagHeader[6] = 0x00;
	id3v2TagHeader[7] = 0x01;
	id3v2TagHeader[8] = 0x14;
	id3v2TagHeader[9] = 0x54;
	APICHeader[0] = 0x6f;
	APICHeader[1] = 0x6c;
	APICHeader[2] = 0x61;
	APICHeader[3] = 0x4b;
	APICHeader[4] = 0x61;
	APICHeader[5] = 0x63;
	APICHeader[6] = 0x65;
	APICHeader[7] = 0x41;
	APICHeader[8] = 0x50;
	APICHeader[9] = 0x49;
	APICHeader[10] = 0x43;
	APICHeader[11] = 0x00;
	APICHeader[12] = 0x02;
	APICHeader[13] = 0x35;
	APICHeader[14] = 0x4b;
	APICHeader[15] = 0x00;
	APICHeader[16] = 0x00;
	APICHeader[17] = 0x00;
	APICHeader[18] = 0x69;
	APICHeader[19] = 0x6d;
	APICHeader[20] = 0x61;
	APICHeader[21] = 0x67;
	APICHeader[22] = 0x65;
	APICHeader[23] = 0x2f;
	APICHeader[24] = 0x6a;
	APICHeader[25] = 0x70;
	APICHeader[26] = 0x65;
	APICHeader[27] = 0x67;
	APICHeader[28] = 0x00;
	APICHeader[29] = 0x00;
	APICHeader[30] = 0x00;
	if( mp3File )
	{
		printf("Begining of mp3 file processing...\n");
		while( (value = fread(&sc,sizeof(unsigned char),1,mp3File)) == 1 )
		{
			bytes++;
			if( haveHeaderTag == 0 )
				if( sc == 0x49 )
				{
					chainLinkHeaderTag = 1;
				}
				else
				{
					if( sc == 0x44 && chainLinkHeaderTag == 1 )
					{
						chainLinkHeaderTag = 2;
					}
					else
					{
						if( sc == 0x33 && chainLinkHeaderTag == 2 )
						{
							printf("We've found an id3v2 header tag in the mp3 file!! =)\n");
							haveHeaderTag = 1;
						}
						else
						{
							chainLinkHeaderTag = 0;
						}
					}
				}
			if( sc == 0x41 )
			{
				chainLinkAPIC = 1;
			}
			else
			{
				if( sc == 0x50 && chainLinkAPIC == 1 )
				{
					chainLinkAPIC = 2;
				}
				else
				{
					if( sc == 0x49 && chainLinkAPIC == 2 )
					{
						chainLinkAPIC = 3;
					}
					else
					{
						if( sc == 0x43 && chainLinkAPIC == 3 )
						{
							printf("We've found a picture in the mp3 file!! =)\n");
							hadImage = 1;
						}
						else
						{
							chainLinkAPIC = 0;
						}
					}
				}
			}
			
			if( sc == 0xff  && haveFF == 0)
			{
				chainLinkBegining = 1;
				haveFF = 1;
			}
			else
			{
				if( sc == 0xd8 && chainLinkBegining == 1 )
				{
					chainLinkBegining = 2;
				}
				else
				{
					if( sc == 0xff && chainLinkBegining == 2 && haveFF == 1)
					{
						chainLinkBegining = 3;
					}
					else
					{
						if( (sc == 0xe1 || sc == 0xe0) && chainLinkBegining == 3 )
						{
							printf("We begin to keep an image file!! =) =)\n");
							if( currentImage == 0 )
							{
								sprintf(filename,"mp3FileFirstImageRead.jpg");
							}
							else
							{
								sprintf(filename,"mp3FileFirstImageRead (%d).jpg",currentImage);
							}
							agregarNodo(lista,crearNodo(filename,fopen(filename,"w")));
							currentImage++;
							fwrite(&jpgHeader[0],sizeof(unsigned char),1,obtenerUltimoDescriptorArchivo(lista));
							fwrite(&jpgHeader[1],sizeof(unsigned char),1,obtenerUltimoDescriptorArchivo(lista));
						}
						else
						{
							chainLinkBegining = 0;
							haveFF = 0;
						}
					}
				}
			}
			
			if( lista->root != NULL )
			{
				fwrite(&sc,sizeof(unsigned char),1,obtenerUltimoDescriptorArchivo(lista));
			}
			
			if( sc == 0xff )
			{
				chainLinkEnd = 1;
			}
			else
			{
				if( sc == 0xd9 && chainLinkEnd == 1 )
				{
					if( lista->root != NULL )
					{
						printf("We are finishing saving one file...\n");
						eliminarUltimoNodo(lista);
					}
				}
				else
				{
					chainLinkEnd = 0;
				}
			}
		}
		printf("Terminamos de leer el fichero... Se leyeron: %d bytes...\n",bytes);
		rewind(mp3File);
		if( hadImage == 1 )
		{
			printf("Se han obtenido las imágenes del fichero mp3.\n");
		}
		else
		{
			bufferedImage = loadImage(argv[2],&bufferedImageLength);printf("-------> %d\n",bufferedImageLength);
			if( bufferedImage == NULL )
			{
				printf("Hubo un error al cargar la imagen, terminando programa...\n");
				return 0;
			}
			else
			{	
				printf("Begining of mp3 file processing... %d bytes\n",strlen(bufferedFile) + bufferedImageLength + 31);
				bufferedFile = loadImage(argv[0],&bufferedFileLength);printf("-------> %d\n",bufferedFileLength);
				newBufferedFile = malloc(sizeof(unsigned char)*(bufferedFileLength + bufferedImageLength + 41));
				for(i=0;i<10;i++)
				{
					newBufferedFile[i] = id3v2TagHeader[i];
				}
				for(i=0;i<31;i++)
				{
					newBufferedFile[i+10] = APICHeader[i];
				}
				for( i=0; i<bufferedImageLength; i++)
				{
					newBufferedFile[i+41] = bufferedImage[i];
				}
				outputFile = fopen("nuevo.mp3","w");
				for(i=0;i<bufferedFileLength + bufferedImageLength + 41;i++)
					fwrite(&newBufferedFile[i],sizeof(unsigned char),1,outputFile);
				fclose(outputFile);
			}
		}
		fclose(mp3File);
	}
	else
	{
		printf("Error en el nombre del archivo proporcionado...\n");
	}
	return 0;
}
