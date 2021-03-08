/////////////////////////////////////////////////////////////////////////////////////////
// my_matr.cpp
// w tym pliku jest podana obsluga macierzy jako tablicy jednowymiarowej
#include "stdafx.h"
#include "my_matr.h"
#include "my_mess.h"
#include "omp.h"

//
//Dane globalne, udostepnione tylko w tym pliku
static MY_MATRIX A_Matr;  //Macierz A
static MY_MATRIX B_Matr;  //Macierz B  
static MY_MATRIX C_Matr;  //Macierz C 
static MY_MATRIX CC_Matr;  //Macierz CC

static void matr_SetZero(MY_MATRIX *Matr);

int MATR_Init(MY_MATRIX *Matr, int m, int n)
/*=========================================================================================
n, m - rozmiary macierzy
RetVal: 1 - OK
        0 - !OK
===========================================================================================*/
{
	Matr->mdim = m;
	Matr->ndim = n;
	
	//alokujemy tablice wskaznikow do wierszy macierzy
	Matr->buff = (double *)malloc(Matr->ndim*Matr->mdim*sizeof(double));
	if(!Matr->buff)
		return 0;

	return 1;
}//InitMatr


void MATR_Free(MY_MATRIX *Matr)
/*===========================================================================================
zwalnia pamiec  
============================================================================================*/
{
	if(Matr->buff)
	{
		free(Matr->buff);
		Matr->buff = NULL;
	}
}

void MATR_Prepare(MY_MATRIX *Matr)
/*=====================================================================================
Przygotowanie macierzy jako tablicy jednowymiarowe. 
Wypelnienie wiersz po wiersze.
======================================================================================*/
{
	int i, j, ij = 0;

	for(i=0, ij=0; i<Matr->mdim; i++)
	{
		for(j=0; j<Matr->ndim; j++, ij++)
		{
			Matr->buff[ij] = (double)(j+1);
		}
	}
}

void MATR_Mult(MY_MATRIX *C, MY_MATRIX *A, MY_MATRIX *B)
/*=====================================================================================
C = C + A*B
umieszczenie danych - wiersz po wiersze

algorytm
do i = 1, m
	do j = 1, n
		do k = 1, k
			Cij += Aik*Bkj
		end do k
	end do j
end do i

Uwaga: To jest kod nie optymalny, poniewaz  powoduje skoki danych przy odczycie
		elementow macierzy B z pamieci glownej do cache
======================================================================================*/
{
	int i, j, k, 
		lda = A->ndim,
		ldb = B->ndim,
		ldc = C->ndim,
		ij = 0, 
		ik = 0, 
		jk = 0;
	double register rrr;

	for(i=0; i<C->mdim; i++)
	{
		for(j=0; j<C->ndim; j++)
		{
			ij = ldc*i+j;   //wskazuje na C[i][j]

			rrr = 0;
			for(k=0; k<A->ndim; k++)
			{
				ik = lda*i+k;  //wskazuje na A[i][k]
				jk = ldb*k+j;  //wskazuje na B[k][j]
				rrr += A->buff[ik]*B->buff[jk];
			}

			C->buff[ij] = rrr;
		}
	}
}

void MATR_MultFast(MY_MATRIX *C, MY_MATRIX *A, MY_MATRIX *B)
/*=====================================================================================
C = C + A*B
umieszczenie danych - wiersz po wiersze

algorytm
do i = 1, m
	do k = 1, k
		do j = 1, n
			Cij += Aik*Bkj
		end do k
	end do j
end do i

Uwaga: Ten kod nie powoduje skokow przy pobieraniu danych
======================================================================================*/
{
	int i, j, k,
		lda = A->ndim,
		ldb = B->ndim,
		ldc = C->ndim,
		ij = 0, 
		ik = 0, 
		kj = 0;
	double register rrr;
	memset(C->buff, 0, C->mdim*C->ndim*sizeof(double));

	for(i=0; i<C->mdim; i++)
	{
		kj = 0;
		ij = ldc*i; 
		ik = lda*i; 

		for(k=0; k<A->ndim; k++)
		{
			rrr = A->buff[ik];
			for(j=0; j<C->ndim; j++)
			{
				C->buff[ij] += rrr*B->buff[kj];
				kj++; 
				ij++;
			}
			ij -= C->ndim;
			ik++;             
		}
	}
}

void MATR_MultFast1(MY_MATRIX *C, MY_MATRIX *A, MY_MATRIX *B)
/*=====================================================================================
C = C + A*B
umieszczenie danych - wiersz po wiersze
Kazda z macierzy pozostaje podzielona na bloki lb x lb tak, zeby w
pamici cache byli umieszczone 3 bloki: jden - dla macierzy A, drugi - dla B,
trzeci - dla C. Nadaje to mozliwosc zmniejszyc ilosc odczytow danych z pamieci RAM do pamieci cache,
poniewaz dane, jeden raz umieszczone w cache, beda wielokrotnie pobierane z szybkiej pamieci cache,
a nie z wolnej RAM. Nazywa sie to cache reuse.

		lb		lb
	.................
	.		.		.
lb	.		.		.
	.................
	.		.		.
lb	.		.		.
	.................
	
algorytm
do k = 1, K, lb
	do j = 1, N, lb
		przepakujemy block lbxlb of matrix B to BB tak,
		zeby w macierzy BB elementy byli umieszczone kolumna po kolumnie.
		Pozwoli to na najszybszy dostep do tych elementow w petle wewnetrznej.
		do i = 1, M, lb
			//mnozenie blokow macierzy, umieszczonych w cache
			Cib,jb += Aib,kb*Bkb,jb
		end do i
	end do j
end do k

Na poziomie mnozenia blokow jest stosowany algorytm blokowania rejestrow 2x2.
Blok macierzy A jest podzielony na pasma poziome o szerokosci 2,
blok macierzy B - na pasma pionowe o szerokosci 2, a
blok macierzy C - na klatki o rozmiarze 2 x 2.
Doprowadzi to do zmniejszenia ilosci ladowan rejestrow w dwa razy w porownaniu z algorytmem naiwnym.
Dla jednej iteracji petli kk trzeba wykonac 4 ladowania danych w rejestry A[ii,kk], BB[ind_B], A[ii,kk+K], BB[ind_B+lb]
oraz 8 operacji arytmetycznych (4 mnozenia i 4 dodawania). Dane A[ii,kk], BB[ind_B], A[ii,kk+K], BB[ind_B+lb], 
umieszczone w rejestrach, sa wykorzystane po 2 razy (register's reuse).
Algorytm wymaga 8 rejestrow double: rc0 - rc3, oraz dla danych, wymienionych wyzej.

do ii=i, i+lb, 2
	do jj=j, j+lb, 2
		rc0 = ... = rc3 = 0; ind_B = 0;
		do kk=k, k+lb, 1
			rc0 += A[ii,kk]*BB[ind_B];
			rc1 += A[ii,kk]*BB[ind_B+lb];
			rc2 += A[ii,kk+K]*BB[ind_B];
			rc3 += A[ii,kk+K]*BB[ind_B+lb];
			ind_B++;
		end do kk
	C[ii,jj]     = rc0;
	C[ii,jj+1]   = rc1;
	C[ii+1,jj]   = rc2;
	C[ii+1,jj+1] = rc3;
	end do jj
end do ii

======================================================================================*/
{
	int i, j, k, ik, kj, ij;
	int ii, jj, kk, ii_max, jj_max, kk_max, ind_B;
	int ldb;
	const int lb = 64;
	double register rc0, rc1, rc2, rc3;
	MY_MATRIX BB;
	if(!MATR_Init(&BB, lb, lb))
		MESSAGE_Put(MESS_ERR_ALLOC);
	
	matr_SetZero(C);

	int K = A->ndim;
	int N = B->ndim;
	int M = A->mdim;

	for(k=0; k<K; k+=lb)
	{
		kk_max = k+lb;
		ldb = lb;
		if(kk_max > K)
		{
			kk_max = K;
			ldb = K-k;
		}

		for(j=0; j<N; j+=lb)
		{
			jj_max = j+lb;
			if(jj_max > N)
				jj_max = N;

			//repack block lbxlb of matrix B to BB
			for(jj=j,ind_B=0; jj<jj_max; jj++)
			{
				for(kk=k; kk<kk_max; kk++, ++ind_B)
				{
					kj = N*kk+jj;
					BB.buff[ind_B] = B->buff[kj];
				}
			}

			#pragma omp parallel for private(i, ii_max, ii, ind_B, jj, rc0, rc1, rc2, rc3, kk, ik, ij) schedule(guided)
			//mnozenie blokow macierzy przy blokowaniu rejestrow 2 x 2.
			for(i=0; i<M; i+=lb)
			{
				ii_max = i+lb;
				if(ii_max > M)
					ii_max = M;

				for(ii=i; ii<ii_max; ii+=2)
				{
					ind_B=0;
					for(jj=j; jj<jj_max; jj+=2)
					{
						rc0 = rc1 = rc2 = rc3 = 0;
						for(kk=k; kk<kk_max; kk++)
						{
							ik = K*ii+kk;
							rc0 += A->buff[ik]*BB.buff[ind_B];
							rc1 += A->buff[ik]*BB.buff[ind_B+ldb];
							rc2 += A->buff[ik+K]*BB.buff[ind_B];
							rc3 += A->buff[ik+K]*BB.buff[ind_B+ldb];
							++ind_B;
						}
						ind_B += ldb;

						ij = N*ii+jj;
						C->buff[ij]     += rc0;
						C->buff[ij+1]   += rc1;
						C->buff[ij+N]   += rc2;
						C->buff[ij+N+1] += rc3;
					}
				}
			}
		}
	}

	MATR_Free(&BB);
}

bool MATR_Compare(MY_MATRIX *C, MY_MATRIX *CC)
{
	int i, j, ij = 0;
	double val;
	const double eps = 1e-9;
	for(i=0; i<C->mdim; ++i)
	{
		for(j=0; j<C->ndim; ++j, ++ij)
		{
			val = C->buff[ij]-CC->buff[ij];
			if(val*val > eps*eps)
				return 0;
		}
	}
	return 1;
}


MY_MATRIX *GetA_MatrPtr()
{
	return &A_Matr;
}

MY_MATRIX *GetB_MatrPtr()
{
	return &B_Matr;
}
MY_MATRIX *GetC_MatrPtr()
{
	return &C_Matr;
}

MY_MATRIX *GetCC_MatrPtr()
{
	return &CC_Matr;
}


void matr_SetZero(MY_MATRIX *Matr)
{
	memset((void *)Matr->buff, 0, Matr->ndim*Matr->mdim*sizeof(double));
}