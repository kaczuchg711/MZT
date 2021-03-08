/////////////////////////////////////////////////////////////////////////////////////////////
//my_mess.h

//to sa strazy pliku: nie daja mozliwosci dolaczac jego wielokrotnie
#ifndef MY_MESS_PLIK_H
#define MY_MESS_PLIK_H

#include <stdlib.h>
#include "my_matr.h"

#define MAX_PATH 1024

//tu miescimy informacje, ktora mysi byc widoczna w wielu plikach

enum MESSAGE_ERR_WARN
{
	MESS_ERR_ALLOC,
	MESS_ERR_OPENFILE,
	MESS_WARN_DIM_TOO_BIG,
	MESS_RES_INCORR,
	MESS_OK,
	MESS_TOT_NUMB
};

//prototypy funkcji interfejsnych
int MESSAGE_Init(char *FileName);
void MESSAGE_Free();
void MESSAGE_Put(int num);
void MESSAGE_PrintMatr(MY_MATRIX *Matr, char *Title);

#endif