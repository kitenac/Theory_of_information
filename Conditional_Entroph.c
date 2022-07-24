 

/*
    +-----------------------------------------------+
    | Use "gcc Conditional_Probab.c -lm" to compile |
    +-----------------------------------------------+


 Условная вероятность 
-Если что-то не понятно в этой проге - смотри Entrophia.c - там аналогичные Extra_blocks[]
*/

#include <stdio.h>
#include <stdlib.h> // for u_int_t
#include <string.h> //for memset
#include <dirent.h> //for directories
#include <unistd.h> // for chdir()

#include "Staff.h"

char* dev = "\n===========================================================================\n";                 // deviding border




int main(int argc, char* argv[])
{
 int i, j;
 char cur_d[256];                                                        //Buffer for writing in current directory
 char dir_name[256];
 u_int8_t prev, cur;                                                     //previous and current bytes while reading file

 u_int64_t Total_size = 0;                                              // Ammount of bytes 
 u_int32_t Indents[256] = {0};                                          //How many times we see each of 256 bytes 


//Relates - 256x256 матрицa "идущих подряд" событий(байтов) a[i]a[j]. Block_Type - выбираемый(define`ом) тип элеиентов матрицы - описанно в Staff.c
 Block_Type** Relates;                                                   
 Relates = get_Relates();




//----------Try to open ./source dir, then changing workdirectory
 DIR *d;
 struct dirent *dir;
 d = opendir("./source"); 	                                              // Передаём dir-STREAM(поток папочный) в переменную структ. типа dirent                       
 if (d == NULL) 
	{printf("Can`t oppen/find directory \"/source\" \\_(0-o)_/\n"); return 0;}
 else
 {
    dir_info(d, dir);

    chdir("source");                                                      //changing working directory
    printf("Write exactly name of folder you want to analayze (all files in directory assumes as single byte-stream)\n: ");
    scanf("%s", dir_name);
 }


//---------Same for user-chosen dir (./source/PDF, ./source/IMG, ...) 
 DIR *D;
 struct dirent *Dir;
 D = opendir(dir_name);
 
 if (D == NULL) 
	{printf("Error with oppening directory %s\n", dir_name); return 0;}
 else
 {
    chdir(dir_name);
    printf("-Current dir: %s\n", getcwd(cur_d, 256));
 }


//---------Scroling down all files in user`s directory and fullfill 256x256 array

 while ((Dir = readdir(D)) != NULL) 
 {
  if  (Dir->d_type == DT_REG)                                           //check if it`s file now(it may be dir, block device, etc)
  {
    
    FILE *dir_file_i = fopen( Dir->d_name, "rb");
    if (!dir_file_i) printf("Failed to open file: %s \n", Dir->d_name);
    else printf("...oppened %s\n", Dir->d_name);

    fseek(dir_file_i, 0L ,SEEK_END);      
    u_int32_t size_i = ftell(dir_file_i) + 1;                               //Size of i-th file in dir
    Total_size += size_i;
    fseek(dir_file_i, 0L ,SEEK_SET);      
    
    

    //----------Получаем 256x256 матрицу "идущих подряд" событий a[i]a[j] для каждого файла и Масссив Indents[256] - далее с их пом-ю пол-им P(a[i]\a[j])
    prev = fgetc(dir_file_i);   
    Indents[prev]++;
   
   // Relate[i][j] - tells how many times byte-bigramm ij  accured. where i,j in 00-ff
    while (!feof(dir_file_i))
    {
        cur = fgetc(dir_file_i);
        Relates[prev][cur]++;                                           // it`s event a[i]a[j]  from  p( a[i]a[j] ) 
        Indents[cur]++;
        
        if (Relates[prev][cur] == LIMIT)
        {
           Relates[prev][cur] = 0;
           Extra_bl[prev][cur]++;
        }
        if (Extra_bl[prev][cur] == 0xffff)                              // Checking overflow of extrablock
        {  
          printf("\n\n     [ERROR]Limit of Extrablocks is reached - increase block size\n     (#define LARGE or Giant) or add some Extrablocks\n");
          return 0;
        }
       

        prev = cur;                                                     // transposition 
    }
    
    fclose(dir_file_i);    
  }
 }


 long double** Condit_probab = (long double **)malloc(256 * sizeof(long double*)); 
 for (int i = 0; i<256; ++i)
 {
     Condit_probab[i] = (long double *)malloc(256*sizeof(long double));            //256 arrays per each pointer
     memset(Condit_probab[i], 0, 256*sizeof(long double));                       //Nulling each byte from Relates[0][0]
 } 



 //double p_i = 0;
 double check_P = 0;
 long double* p = p_a(Indents, Total_size);    // probable vector - has probbability of each 256 byte 
 
 // Conditional_probab[][]:  P[i][j] <=> P(j\i) - т.е. вероятность перейти из E_i -> E_j
 for(i = 0; i < 256; ++i)
 {
     for(j = 0; j < 256; ++j)
     {
         if (p[i] != 0)
             Condit_probab[i][j] = ( (long double)(Extra_bl[i][j] * LIMIT + Relates[i][j]) / (long double)Total_size )  /  p[i];  //P[i][j] <=> P(j\i) = P(ij)/P(i) 
         else                                                                                                                     
             Condit_probab[i][j] = 0;    //p_i == 0 => dev by zero :( BUT also it means that event i  never happens => P(j\i) = 0 
 
         // [Check] -Just to prove that total P = 1 as in theory 
         check_P += (long double)(Extra_bl[i][j] * LIMIT + Relates[i][j]) / (long double)Total_size; //P(ab)        
     }
 }


//check_CM(Condit_probab);
//print_256x256(Relates);                   //print Relates table 256x256

//print_256x256_fl(Condit_probab);

//-----Info
 printf("\nBytes readen: %u , in MB~ %u\n", Total_size, Total_size/MB);
 printf("\n[!!!]'Size of file' here is less then file system tells!!!, \nbecause not all allocated space of file is written now)\n");

 
 //long double* p = p_a(Indents, Total_size);    // probable vector - has probbability of each 256 byte 
 long double Entroph = Entrophia(p);
 long double CM_Entrop = Entrophia_CM(p, Condit_probab);

 printf("%sEntrophia of dir`s '%s' containing is %Lf %s", dev,   dir_name, Entroph,    dev);
 printf("%sEntrophia of Markov`s chain trained on dir`s '%s' containing %Lf %s", dev,    dir_name, CM_Entrop,   dev);

 Clear_Relates(Relates);                      // Free all allocated memory for Relates table 
 return 0;
}


