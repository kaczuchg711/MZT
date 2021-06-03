// Lab_dot_prod.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <cmath>
#include <ctime>
#include "timethrd.h"

using namespace std;

#define NUMB_THREADS 10

int _tmain(int argc, _TCHAR* argv[]) {
    HANDLE hThread[NUMB_THREADS];
    DWORD ThreadID[NUMB_THREADS];
    THREAD_DATA tDat[NUMB_THREADS];
    double* X = NULL, * Y = NULL;
    double dotProd;
    int N, np, loc_N;
    int ntimes;

#ifdef _DEBUG
   cout << "START dotProd: DEBUG VERSION\n";
#else
   cout << "START dotProd: RELEASE VERSION\n";
#endif



    
        cout << "Get N\n";
        cin >> N;
        cout << "Get number of processors\n";
        cin >> np;
        cout << "Get number of repetitions\n";
        cin  >> ntimes;

        loc_N = N / np;
        N = loc_N * np;

        cout << "actual N = " << N << endl;
        cout << "np = " << np << endl;

        try {
            X = new double[N];
            Y = new double[N];
        }
        catch (bad_alloc aa) {
            cout << "memory allocation error\n";
            system("pause");
            exit(1);
        }


        //preparation of X, Y

        for (int i = 0; i < N; i++) {
            X[i] = (double)(i + 1);
            Y[i] = 1.0;
        }
        LPTHREAD_START_ROUTINE threadFunctions[3] = { ThreadFuncWithLocalVar,ThreadFuncWithPointers,ThreadFuncSSE2 };
        DWORD time_st;
        bool IsOK;
        double sum;


        for (int ifun = 0; ifun < 3; ifun++)
        {
            switch (ifun)
            {
            case 0:
                printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ ThreadFuncWithLocalVar \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n");
                break;
            case 1:
                printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ ThreadFuncWithPointers \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n");
                break;
            case 2:
                printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ ThreadFuncSSE2 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n");
                break;
            }
            time_st = GetTickCount();

            for (int ip = 0; ip < np; ip++) {
                //wypelnic dane dla potoku ip
                tDat[ip].threadNo = ip;
                tDat[ip].NoThreads = np;
                tDat[ip].N = N;
                tDat[ip].loc_N = loc_N;
                tDat[ip].ntimes = ntimes;
                tDat[ip].ret = 1;
                tDat[ip].X = X;
                tDat[ip].Y = Y;
                tDat[ip].res = 0;
                //stworzyc potok ip
               // cout << "\nCreateThread" << endl;
                hThread[ip] = CreateThread(NULL, 0, threadFunctions[ifun], &tDat[ip], 0, &ThreadID[ip]);
            }

            //zawiesic pierwotny potok dokad potoki liczace nie skonca prace
            WaitForMultipleObjects(np, hThread, TRUE, INFINITE);

            //Check
            IsOK = true;
            for (int ip = 0; ip < np; ip++) {
                if (tDat[ip].ret != 0) {
                    cout << " Thread # " << ip + 1 << " failed" << endl;
                    IsOK = false;
                }
                CloseHandle(hThread[ip]);
            }

            if (!IsOK) {
                system("pause");
                exit(1);
            }

            //reduction: sumowanie
            dotProd = 0.0;

            for (int ip = 0; ip < np; ip++) {
                dotProd += tDat[ip].res;
            }

            cout << "\n\n\nWynik po sumowaniu:\t" << dotProd << endl;
            cout << "WTime = " << GetTickCount() - time_st << " ms\n\n\n\n";

            //check
            sum = 0.0;
            for (int i = 0; i < N; i++) {
                sum += X[i] * Y[i];
            }

            if ((fabs(sum - dotProd)) > 1.0e-9) {
                cout << "error\n";
                cout << "!!!!!!!!!!!\n";
                cout << N;
                cout << dotProd;
                system("pause");
            }


        }

        cout << "END MV\n";
        system("pause");


        delete[] X;
        delete[] Y;

    return 0;
}



