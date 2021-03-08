////////////////////////////////////////////////////////////////////////////////////////////////////
//   mvec.cpp

#include "stdafx.h"
#include <iostream>
#include "mvec.h"
#include <emmintrin.h>

#include <immintrin.h>

using namespace std;

void mult_naive(double *a, double *x, double *y, int n)
{
	int i, j, ij;
	double register reg;

	for(i=0, ij=0; i<n; ++i)
	{
		reg = 0;

		for(j=0; j<n; ++j, ++ij)
		{
			reg += a[ij]*x[j];
		}

		y[i] = reg;
	}
}
//4 zewnetrznej // 16 wewnetrzenej
void matvec_XMM(double* a, double* x, double* y, int n, int lb)
{
	//napisz swoj kod

}

#ifdef YES_AVX
void matvec_YMM(double* a, double* x, double* y, int n, int lb)
{
	//napisz swoj kod
}
#endif

#ifdef YES_FMA
void matvec_FMA(double* a, double* x, double* y, int n, int lb)
{
	int i, j;
	__m256d  ra0, ra1, ra3, rx0, sum0, sum1, sum2, sum3;
	double* ptr_a, * ptr_x;
	__declspec(align(32)) double res[4];

	const int nr = 8;
	if (nr != lb)
	{
		cout << "lb !=ne\n";
		system("pause");
		exit(1);
	}

	ptr_a = a;

	for (i = 0; i < n; i += 4)
	{
		ptr_x = x;
		sum0 = _mm256_setzero_pd();
		sum1 = _mm256_setzero_pd();
		sum2 = _mm256_setzero_pd();
		sum3 = _mm256_setzero_pd();

		for (j = 0; j < n; j += nr)
		{
			_mm_prefetch((const char*)(ptr_a + nr), _MM_HINT_NTA);
			_mm_prefetch((const char*)(ptr_a +n+nr), _MM_HINT_NTA);
			_mm_prefetch((const char*)(ptr_a + nr), _MM_HINT_NTA);
			_mm_prefetch((const char*)(ptr_a + nr), _MM_HINT_NTA);
			_mm_prefetch((const char*)(ptr_a + nr), _MM_HINT_NTA);
			ra0 = _mm256_load_pd(ptr_a);
			ra0 = _mm256_load_pd(ptr_a);
			ra0 = _mm256_load_pd(ptr_a);
			ra0 = _mm256_load_pd(ptr_a);
			rx0 = _mm256_load_pd(ptr_x);
		}
	}
}
#endif
