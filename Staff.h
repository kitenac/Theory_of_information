/*
    Here`re some staff functions not related with initial point of main program(Conditional_Probab.c) 
*/

#include <stdio.h>
#include <stdlib.h> // for u_int_t
#include <dirent.h> //for directories
#include <unistd.h> // for chdir()
#include <string.h> //for memset
#include <math.h>   // for log

// Some specific structures for pseudo-dynamic type: Number_of_indents_byte_pair = Extrablock(bytepair[i,j]) * LIMIT + Relates(byte pair[i,j]) 
 
const u_int32_t KB = 1024;
const u_int32_t MB = 1024*KB;

// TODO - обернуть Extra_bl[][] в структуру для того, чтоб не одно значение Extra_bl на всю либу было
u_int16_t Extra_bl[256][256];                                     // number of extra blocks(0-255) per each byte-pair



//#define Small
#define Large
//#define Giant

#ifdef Small 
    #define Block_Type u_int8_t
    #define LIMIT 0xff
#elif defined(Large)  
    #define Block_Type u_int16_t
    #define LIMIT 0xffff
#elif defined(Giant)
    #define Block_Type u_int32_t
    #define LIMIT 0xffffffff
#endif




Block_Type** get_Relates()
{
//-------Making 2 Dimentional array <=> making (array of pointers) to (arrays)
//Relates - 256x256 матрицa "идущих подряд" событий(байтов) a[i]a[j]

 Block_Type** Relates;                                                   
 Relates = (Block_Type**)malloc(256*sizeof(Block_Type*));                //Making array of pointers 
 for (int i = 0; i<256; ++i)
 {
    Relates[i] = (Block_Type*)malloc(256*sizeof(Block_Type));            //256 arrays per each pointer
    memset(Relates[i], 0, 256*sizeof(Block_Type));                       //Nulling each byte from Relates[0][0]
 }   //----As result we have 256x256 array of zeroes 

 memset(Extra_bl, 0, 256*256*sizeof(Block_Type));
 return Relates;

}


void Clear_Relates(Block_Type** Relates)
{
 //Clear allocated memory
 for (int i = 0; i<256; ++i)
    free(Relates[i]);

 free(Relates); 
}



long double* p_a(u_int32_t* Indents, u_int64_t total_sz)       //Indents[i] - total ammount of i-th numb in raw file
{                   
  long double* p = malloc(256*sizeof(long double));         // Saving array in Heap not to lose local value of array(pointer can point on trash)

  for(int i = 0; i<256; ++i)
      p[i] = (long double)Indents[i]/(long double)total_sz;
  
  return p;
} 


long double Entrophia(long double* p)      // p = (p_1, ...., p_n) - probabilites for each of 256 bytes
{
 long double entrophia = 0;

 for(int i = 0; i<256; ++i)
 {
   if (p[i] != 0)                           //log2(0) ---> -infinity which is -nan, but we "told" that log(0) = 0 <=> ignore
       entrophia -= p[i]*log2l(p[i]);
 }

 return entrophia;
}



void check_CM(long double** Markov_matrix)
{
    int i,j;    long double P = 0;
    long double p_i[256];

    for (i = 0; i < 256; ++i)
        {
            for (j = 0; j < 256; ++j )
            {    p_i[j] = Markov_matrix[i][j];    // copping i-th line  
                 P += p_i[j];   }

//            if (P != 1 && P !=0)
                printf("%Lf ", P);         // proves that P = 1 in each line
               
            P = 0; 
        }
}




// Entrophia of Markov`s chain initialized by conditional probability table
long double Entrophia_CM(long double* p, long double** Markov_matrix)  // vector p and matrix П of Markov`s chain
{
    long double Entroph_CM = 0;
    long double p_i[256];           // probable vector of i-th line of Relate
    long double P = 0;              // для проверки стохастичности 
    int i, j;

    for (i = 0; i < 256; ++i)
        {
            for (j = 0; j < 256; ++j )
            {    p_i[j] = Markov_matrix[i][j];    // copping i-th line  
                 P += p_i[j];   }

            //printf("%Lf ", P);         // proves that P = 1 in each line
               
            Entroph_CM += Entrophia(p_i) * p[i];
            P = 0; 
        }
    
    //printf("Markov function says: %Lf\n", Entroph_CM);
    return Entroph_CM;
}




void print_256x256( Block_Type** Relates)
{
    int i,j;
    for(i = 0; i<256; ++i)
    {
        printf("\n--------------------i = %x------------------------\n", i);
        for(j = 0; j<256; ++j)
           {
            printf("%X ", Extra_bl[i][j]*LIMIT + Relates[i][j]);
      //    if (Relate[i][j] > 0) printf(" <Gotcha_bitch%x ", i); //for small files
            if (j % 16 == 0) printf("\n");
           } 
    }
}


 



 void print_256x256_fl(long double **matr )
 {
     int i,j;
     for(i = 0; i<256; ++i)
     {
         printf("\n--------------------i = %x------------------------\n", i);
         for(j = 0; j<256; ++j)
         {
            printf("%f[%x] ", matr[i][j], j);
            if (j % 6 == 0) printf("\n");
         }
     }
 }
 




void dir_info(DIR* d, struct dirent *dir)
 {
    printf ("Avalible directorirs and files in /source\n");
    while ((dir = readdir(d)) != NULL) 
  	 {
      printf("[name] %s\n", dir->d_name);	
      if(dir->d_type == 4)
      	 printf("...[type] Directory \n\n");
      if(dir->d_type == 8)
      	 printf("...[type] File \n\n");
    }

 }


// Simple Entrophia:
// PDF: 7.943583
// TXT: 4.087378


// Markov Chain Entrophia:
// PDF: 7.630883
// TXT: 2.556357 
