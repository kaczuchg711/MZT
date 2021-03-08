/////////////////////////////////////////////////////////////////////////////////////////
//my_matr.h

//to sa strazy pliku: nie daja mozliwosci dolaczac jego wielokrotnie
#ifndef MY_MATR_PLIK_H
#define MY_MATR_PLIK_H

#include <malloc.h>
#include <stdlib.h>
#include <memory.h>
#include "my_matr.h"

//tu miescimy informacje, ktora mysi byc widoczna w wielu plikach
//deklaracja struktury macierzy wprowadzi nowy typ danych MY_MATRIX
struct MY_MATRIX
{
	double *buff;
	int ndim, mdim;
};

//prototypy funkcji, udostepnionych w innych plikach
int MATR_Init(MY_MATRIX *Matr, int n, int m);
void MATR_Free(MY_MATRIX *Matr);
void MATR_Prepare(MY_MATRIX *Matr);
void MATR_Mult(MY_MATRIX *C, MY_MATRIX *A, MY_MATRIX *B);
void MATR_MultFast(MY_MATRIX *C, MY_MATRIX *A, MY_MATRIX *B);
void MATR_MultFast1(MY_MATRIX *C, MY_MATRIX *A, MY_MATRIX *B);
bool MATR_Compare(MY_MATRIX *C, MY_MATRIX *CC);

MY_MATRIX *GetA_MatrPtr();
MY_MATRIX *GetB_MatrPtr();
MY_MATRIX *GetC_MatrPtr();
MY_MATRIX *GetCC_MatrPtr();


#endif
