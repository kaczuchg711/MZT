//////////////////////////////////////////////////////////////////////////////////////
//my_mess.cpp
//Obsluga komunikatow

#include "stdafx.h"
#include "my_mess.h"

//dane globalne, udostepnione tylko w tym pliku
//pierwsa litera dyceduje, czy to jest blad, warning lub komunikat
static char My_File_Name[MAX_PATH];
static FILE *my_outfile_ptr = NULL;

static char *MY_STRING_TABLE[] =
{
	"E  blad alokacji pamieci",                //MESS_ERR_ALLOC
	"E  blad otwarcia pliku",                  //MESS_ERR_OPENFILE
	"W  rozmiar jest wiekszy od 1000",         //MESS_WARN_DIM_TOO_BIG
	"W  wynik niepoprawny!!!!!!!!!!!!!!!!!!!", //MESS_RES_INCORR
	"K  OK"                                    //MESS_OK
};

int MESSAGE_Init(char *FileName)
/*===========================================================================================
Otworza plik o nazwie FileName
RetVal:  1 - OK
         0 - !OK
=============================================================================================*/
{
	sprintf_s(My_File_Name, "%s", FileName);

	errno_t ret = fopen_s(&my_outfile_ptr, My_File_Name, "w");
	if(ret != 0)
		return 0;

	return 1;

}//int InitOutputFile(char *FileName)



void MESSAGE_Free()
{
	if(my_outfile_ptr)
	{
		fclose(my_outfile_ptr);
		my_outfile_ptr = NULL;
	}
}


void MESSAGE_Put(int num)
/*===============================================================================================
wyprowadza komunikat o numerze num w plik
================================================================================================*/
{
	if(!my_outfile_ptr)
	{
		printf("plik nie pozostal otwarty\n");
		printf("%s\n", MY_STRING_TABLE[num]);
		All_Free();
		system("pause");
		exit(1);
	}

	if(num < 0 || num >= MESS_TOT_NUMB)
	{
		printf("niepoprawny numer komunikatu\n");
		return;
	}

	char symb = MY_STRING_TABLE[num][0];  //to jest symbol, steryjacy wyprowadzeniem

	switch(symb)
	{
	case 'E': //blad
		fprintf(my_outfile_ptr, "Blad: %s\n", MY_STRING_TABLE[num]);
		printf("Blad: %s\n", MY_STRING_TABLE[num]);
		All_Free();
		system("pause");
		exit(1);
		break;
	case 'W': //warning
		fprintf(my_outfile_ptr, "Warning: %s\n", MY_STRING_TABLE[num]);
		printf("Warning: %s\n", MY_STRING_TABLE[num]);
		break;
	case 'K': //komunikat
		fprintf(my_outfile_ptr, "%s\n", MY_STRING_TABLE[num]);
		printf("%s\n", MY_STRING_TABLE[num]);		
		break;
	default:
		printf("Symbol sterujacy jest bledny.  %s\n", MY_STRING_TABLE[num]);
	};
}

void MESSAGE_PrintMatr(MY_MATRIX *Matr, char *Title)
/*==============================================================================================
Drukuje macierz Matr o rozmiarze ndim x mdim w plik. Title - naglowek przed wydrukiem
===============================================================================================*/
{
	int mdim = Matr->mdim, 
		ndim = Matr->ndim;
	
	if(!my_outfile_ptr)
	{
		printf("plik nie pozostal otwarty\n");
		return;
	}

	int i, j, ij = 0, k, kb;
	int jstart, jend;

	fprintf(my_outfile_ptr, "\n");
	fprintf(my_outfile_ptr, "%s\n\n", Title);

	//ilosc blokow dla wydruku. Drukujemy po 6 kolumn
	kb = (ndim-1)/6+1;

	for(k=0; k<kb; k++)
	{
		//pentle po ilosci blokow wydruku k
		jstart = 6*k;
		jend = (jstart+6 <= ndim) ? (jstart+6) : (ndim);

		fprintf(my_outfile_ptr, "\n");
		fprintf(my_outfile_ptr, "j = %d\t\t  j = %d\t\t  j = %d\t\t  j = %d\t\t  j = %d\t\t  j = %d\n",
			jstart, jstart+1, jstart+2, jstart+3, jstart+4, jstart+5);		

		//drukujemy blok o numerze k
		for(i=0; i<mdim; i++)
		{
			fprintf(my_outfile_ptr, "\n");
			for(j=jstart; j<jend; j++)
			{
				ij = ndim*i+j;
				fprintf(my_outfile_ptr, "%.4le\t", Matr->buff[ij]);
			}
		}

		fprintf(my_outfile_ptr, "\n");
	}
}