// W7_6.cpp : Defines the entry point for the console application.
// Mnozenie macierzy przez macierz: C = C + A*B
// A - n x k
// B - k x m
// C - n x m

#include "stdafx.h"
#include <time.h>
#include "my_matr.h"
#include "my_mess.h"
#include "omp.h"


//prototyp funkcji All_Free() jest umieszczony w pliku stdafx.h

int _tmain(int argc, _TCHAR* argv[])
{
	int n = 0, m = 0, k = 0, np = 1;
	clock_t t_start, t_end;
	double elapsed_time;

	//inicjializacja systemu komunikatow
	if(!MESSAGE_Init("output.txt"))
		MESSAGE_Put(MESS_ERR_OPENFILE);

	printf("podaj rozmiar m, n, k oraz liczbe watkow np\n");
	scanf_s("%d%d%d%d", &m, &n, &k, &np);

	//Zeby uniknac komplikowania kodu, w celu nauczania
	//ograniczymy sie parzystymi wartosciami m, n, k.
	//Jest to związane z blokowaniem rejestrow 2x2
	m = m/2;
	n = n/2;
	k = k/2;
	m = 2*m;
	n = 2*n;
	k = 2*k;

	if(m > 1000 || n > 1000 || k > 1000)
		MESSAGE_Put(MESS_WARN_DIM_TOO_BIG);

	omp_set_num_threads(np);

	//inicjalizacja macierzy A, B, C
	if(!MATR_Init(GetA_MatrPtr(), m, k))
		MESSAGE_Put(MESS_ERR_ALLOC);
	if(!MATR_Init(GetB_MatrPtr(), k, n))
		MESSAGE_Put(MESS_ERR_ALLOC);
	if(!MATR_Init(GetC_MatrPtr(), m, n))
		MESSAGE_Put(MESS_ERR_ALLOC);
	if(!MATR_Init(GetCC_MatrPtr(), m, n))
		MESSAGE_Put(MESS_ERR_ALLOC);

	//Przygotowanie macierzy A, B
	MATR_Prepare(GetA_MatrPtr());
	MATR_Prepare(GetB_MatrPtr());

	//Drukujemy macierzy A, B, jesli wersie DEBUG
	//Nie drukujemy, esli RELEASE
#ifdef _DEBUG
	MESSAGE_PrintMatr(GetA_MatrPtr(), "Macierz A");

	//Drukujemy macierz B (k x m)
	MESSAGE_PrintMatr(GetB_MatrPtr(), "Macierz B");
#endif

	//C = C + A*B  - metoda zwykla
	t_start = clock();
	MATR_Mult(GetCC_MatrPtr(), GetA_MatrPtr(), GetB_MatrPtr());
	t_end = clock();
	elapsed_time = (double)(t_end-t_start)/CLOCKS_PER_SEC;
	printf("Metoda klasyczna i, j, k          :  %lf s   %lf MFLOPS\n", elapsed_time, 2.0*m*n*k / elapsed_time / 1000/1000 );

	//Drukujemy wynik: macierz CC (DEBUG)
#ifdef _DEBUG
	MESSAGE_PrintMatr(GetCC_MatrPtr(), "Wynik: C = C + A*B");
#endif

	//C = C + A*B - metoda przyspieszona, sa usuniete skoki przy odczycie danych z macierzy B
	t_start = clock();
	MATR_MultFast(GetC_MatrPtr(), GetA_MatrPtr(), GetB_MatrPtr());
	t_end = clock();
	elapsed_time = (double)(t_end-t_start)/CLOCKS_PER_SEC;
	printf("metoda przysp.   i, k, j          :  %lf s   %lf MFLOPS\n", elapsed_time, 2.0 * m*n*k / elapsed_time / 1000 / 1000);
	
	//Drukujemy wynik: macierz C (DEBUG)
#ifdef _DEBUG
	MESSAGE_PrintMatr(GetC_MatrPtr(), "Wynik 1: C = C + A*B");
#endif

	//Porownujemy wyniki mnozenia dwoch metod
	if(!MATR_Compare(GetCC_MatrPtr(), GetC_MatrPtr()))
		MESSAGE_Put(MESS_RES_INCORR);

	//C = C + A*B - metoda przyspieszona, sa blokowanie cache oraz rejestrow 2 x 2
	t_start = clock();
	MATR_MultFast1(GetC_MatrPtr(), GetA_MatrPtr(), GetB_MatrPtr());
	t_end = clock();
	elapsed_time = (double)(t_end-t_start)/CLOCKS_PER_SEC;
	printf("Block cache, block registers 2 x 2:  %lf s   %lf MFLOPS\n", elapsed_time, 2.0 * m*n*k / elapsed_time / 1000 / 1000);

	//Porownujemy wyniki mnozenia dwoch metod
	if(!MATR_Compare(GetCC_MatrPtr(), GetC_MatrPtr()))
		MESSAGE_Put(MESS_RES_INCORR);

	//Drukujemy wynik: macierz C (n x m)
#ifdef _DEBUG
	MESSAGE_PrintMatr(GetC_MatrPtr(), "Wynik 2: C = C + A*B");
#endif

	MESSAGE_Put(MESS_OK);

	//zwalniamy wszystko
	All_Free();
	system("pause");
	return 0;
}


void All_Free()
{
	MATR_Free(GetA_MatrPtr());
	MATR_Free(GetB_MatrPtr());
	MATR_Free(GetC_MatrPtr());
	MESSAGE_Free();
}

