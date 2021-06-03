//////////////////////////////////////////////////////////////////////////////////
//   funthrd.cpp
//   thread function implementations

#include "stdafx.h"
#include <iostream>
#include "timethrd.h"
#include<cmath>
#include <emmintrin.h>
#include <immintrin.h>
using namespace std;


DWORD WINAPI ThreadFuncWithLocalVar(LPVOID lpParam) 
{
	//Wpisz swoj kod tu
	THREAD_DATA* ptrDat = (THREAD_DATA*)(lpParam);
	register double rrr;
	double* X_i = ptrDat->X;
	double* Y_i = ptrDat->Y;

	for (int j = 0; j < ptrDat->ntimes; j++)
	{	
		rrr = 0;
		int i = 0;
		for (i;i < ptrDat->loc_N; i++)
		{
			rrr += X_i[i + ptrDat->threadNo * ptrDat->loc_N] * Y_i[i + ptrDat->threadNo * ptrDat->loc_N];
		}
		if (ptrDat->NoThreads == ptrDat->threadNo+1)
		{
			int withRest = i + ptrDat->N % ptrDat->NoThreads;
				for (i;i < withRest; i++)
				{
					rrr += X_i[i + ptrDat->threadNo * ptrDat->loc_N] * Y_i[i + ptrDat->threadNo * ptrDat->loc_N];
				}
		}
		ptrDat->res = rrr;
		ptrDat->ret = 0;
	}

	return 0;
}

DWORD WINAPI ThreadFuncWithPointers(LPVOID lpParam)
{
	THREAD_DATA* ptrDat = (THREAD_DATA*)(lpParam);
	double* X_i = ptrDat->X;
	double* Y_i = ptrDat->Y;
	int i;
	for (int j = 0; j < ptrDat->ntimes; j++)
	{
		ptrDat->res = 0;
		i = 0;
		for ( i; i < ptrDat->loc_N; i++)
		{
			ptrDat->res += X_i[i + ptrDat->threadNo * ptrDat->loc_N] * Y_i[i + ptrDat->threadNo * ptrDat->loc_N];
		}
		if (ptrDat->NoThreads == ptrDat->threadNo + 1)
		{
			int withRest = i + ptrDat->N % ptrDat->NoThreads;
			for (i; i < withRest; i++)
			{
				ptrDat->res += X_i[i + ptrDat->threadNo * ptrDat->loc_N] * Y_i[i + ptrDat->threadNo * ptrDat->loc_N];
			}
		}
				
	}
	ptrDat->ret = 0;
	return 0;
}

DWORD WINAPI ThreadFuncSSE2(LPVOID lpParam)
{

	
	THREAD_DATA* ptrDat = (THREAD_DATA*)(lpParam);
	double* X_i = ptrDat->X;
	double* Y_i = ptrDat->Y;
	int i;

	__m128d c1x, c2x, c3x, c4x;
	__m128d c1y, c2y, c3y, c4y;
	__m128d sum, sum1, sum2, sum3;
	_declspec(align(16)) double res[2];

	for (int j = 0; j < ptrDat->ntimes; j++)
	{
		sum = _mm_setzero_pd();
		sum1 = _mm_setzero_pd();
		sum2 = _mm_setzero_pd();
		sum3 = _mm_setzero_pd();
		ptrDat->res = 0;
		i = 0;
		for (;i <= ptrDat->loc_N-8; i+=8)
		{
			_mm_prefetch((const char *)(&X_i[i + ptrDat->threadNo * ptrDat->loc_N + 8]), _MM_HINT_T0);
			_mm_prefetch((const char *)(&Y_i[i + ptrDat->threadNo * ptrDat->loc_N + 8]), _MM_HINT_T0);

			c1x = _mm_load_pd(&X_i[i + ptrDat->threadNo * ptrDat->loc_N ]);
			c2x = _mm_load_pd(&X_i[i + ptrDat->threadNo * ptrDat->loc_N  +2]);
			c3x = _mm_load_pd(&X_i[i + ptrDat->threadNo * ptrDat->loc_N  +4]);
			c4x = _mm_load_pd(&X_i[i + ptrDat->threadNo * ptrDat->loc_N  +6]);
			c1y = _mm_load_pd(&Y_i[i + ptrDat->threadNo * ptrDat->loc_N ]);
			c2y = _mm_load_pd(&Y_i[i + ptrDat->threadNo * ptrDat->loc_N  + 2]);
			c3y = _mm_load_pd(&Y_i[i + ptrDat->threadNo * ptrDat->loc_N  + 4]);
			c4y = _mm_load_pd(&Y_i[i + ptrDat->threadNo * ptrDat->loc_N  + 6]);

			sum = _mm_fmadd_pd(c1x, c1y, sum);
			sum1 = _mm_fmadd_pd(c2x, c2y, sum1);
			sum2 = _mm_fmadd_pd(c3x, c3y, sum2);
			sum3 = _mm_fmadd_pd(c4x, c4y, sum3);
		}
		sum = _mm_add_pd(sum, sum1);
		sum2 = _mm_add_pd(sum2, sum3);
		sum = _mm_add_pd(sum, sum2);

			_mm_store_pd(res, sum);
			ptrDat->res = res[0] + res[1];
			int withRest = i + ptrDat->loc_N % 8;
			for (i; i < withRest; i++)
			{
				ptrDat->res += X_i[i + ptrDat->threadNo * ptrDat->loc_N] * Y_i[i + ptrDat->threadNo * ptrDat->loc_N];
			}
			if (ptrDat->NoThreads == ptrDat->threadNo + 1)
			{
				withRest = i + ptrDat->N % ptrDat->NoThreads;
				for (i; i < withRest; i++)
				{
					ptrDat->res += X_i[i + ptrDat->threadNo * ptrDat->loc_N] *Y_i[i + ptrDat->threadNo * ptrDat->loc_N];
				}
			}
	}
	ptrDat->ret = 0;
	return 0;
}