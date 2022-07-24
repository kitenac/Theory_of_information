

/*

 Use "gcc Entrophia.c -lm" to compile 
    ( and don`t be confused of cast-warning - 
      -I had nothing to write in u_16 pointer before casting it to u_8 pointer )


*/




#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const u_int32_t KB = 1024;
const u_int32_t MB = 1024*KB;

unsigned char key;            
u_int8_t Extra_bl[256] = {0};       // number of extra blocks(0-255) per each symbol in range 0x00-0xFF


//======== Choose one(e.g. define) of Block`s sizes (Block holds total ammount of particular 0x_XX indentions) ========

//Optionaly)
//#define INFO                       //Show p[i] and n_i while counting Entrophia

//1) Choose it with small files ==> their graph will be more detalized 
#define Small                    //For <16 MB sized files workes good 

//2)
//#define Large                      //For bigger files

//========= Keep in mind that we have 256 Extrablocks (if it`s too few => choose Large or retype Extra_bl abowe ) ========


//========= Info-graphical functions - don`t mind, mb carry them in sep library next time
// graph draws as much '*' as some symbol has extra blocks => use 'Small' type of extrablock with small files
void graph(FILE* file)
{
    int i;
    printf("-------------\nFile`s stat:\n-------------\n");
    for (key = 0; key <= 254; key++)                      // key<=255 - could make inf loop ( 255 ---> 0 after key++) due key is char(1 byte)  
       {
         printf("blocks[%d] | %X | ",Extra_bl[key], key);
         for(i = 0; i < Extra_bl[key]; i++) printf("*"); 
         printf("\n");
       }

        printf("blocks[%d] | FF | ",Extra_bl[255]);       //Костыль(
        for(i = 0; i < Extra_bl[key]; i++) printf("*"); 
            printf("\n");
}

void inform(long double P, u_int32_t N, u_int32_t n)
{
  printf("[To Check] Total P = %Lf\n Total N = %u , n = %u\n", P, N, n);
  printf("\n[!!!] If P != 1  or  N != n - increase block type (#define Large) or add some extra blocks\n\n");
  printf("[View] To hide n[i] and p[i] - undefine(or comment) macro \"Info\" \n");
}
//=========


 int main(int argc, char* argv[])
{
 int i;
 char ans;
 if( argc < 2)
  {
    printf("Missing filename\n");
    return 0;
  } 

 FILE* file = fopen(argv[1], "rb");
 if (!file) 
  {
    printf("No such file or an error occured,\n -take a shure that filename is full\n");
    return 0;
  }


 fseek(file,0L ,SEEK_END);       //Setting cursor to END without moving from it( 0L - long int zero)
 u_int32_t n = ftell(file) + 1;  //Knowing length of file (each file has tailing "stoping" byte - so don`t be confused of n = 6 for file of only 5 printed  charecters)


/*      32bit-numbers of file length ~2GB is max for C standart
                for >2GB can be used POSIX standart                 */

 fseek(file, 0L, SEEK_SET);        //return cursor
 
 printf("File size in bytes = %u, in Mb = %u \nsame in HEX: %x\n", n, n/MB ,n);
 printf("\n[!!!]Size of file here is less then file system tells!!!, \nbecause not all allocated space of file is written now) ");


 #ifdef  Small              
     u_int8_t* Block = (u_int8_t*)malloc(256*sizeof(u_int8_t));    //Giving 256 1-byted blocks 
     
     u_int8_t LIMIT = 0xff;                                         //Using to as multipler by ammount of Extrablocks when we count n_i bellow
     #define Block_Type u_int8_t 
     printf("\n-------------\nSmall block\n-------------\n");
  
 #endif
 #ifdef Large 
      u_int16_t* Block = (u_int16_t*)malloc(256*sizeof(u_int16_t));  //Giving 256 2-byted blocks
            
      u_int16_t LIMIT = 0xffff;                                      // keep in mind these 2 bytes represent number of incedents of some 0x ASCII-code in whole document
      #define Block_Type u_int16_t
      printf("\n-------------\nLarge block\n-------------\n");         // 2 bytes` max value is 0xFFFF = 65535 indents of particular 0x 
      printf("If your file ~ under 16 MB set Small block( #define Small)");
 #endif


for (i=0;i<256;++i)             //"Clearing" malloc()`ed memory 
     Block[i] = 0x0000;  



// Openning file and filling Indention table at the same time!
while(!feof(file))
 {
    
    key=fgetc(file);                                     //taking next symbol of source file each loop
    Block[key]++;
    if (Block[key] == LIMIT)                             // Block has been filled
    {
     Block[key] = 0;                                     // "Clearing" current Block
     Extra_bl[key]++; 
     if (Extra_bl[key] == 255)                            // Checking overflow
     {  
        printf("\n\n     [ERROR]Limit of Extrablocks is reached - increase block size\n     (#define LARGE) or add some Extrablocks\n");
        return 0;
     }
    }
 }


 printf("\nWana see a graphic in %u byte-sized blocks?[Y/N]\n", LIMIT+1); //for ex: 255 is LIMIT value and 256 - total size
    scanf("%c", &ans);
    if ( ans == 'Y')  graph(file);


 

 long double p[256];
 long double Entrophia, P;
 Entrophia = 0;
 P = 0;

 u_int32_t n_i, N = 0;

 for(i = 0; i<256; ++i)
 {
   n_i = LIMIT * Extra_bl[i] + Block[i];    //total ammount of i-th numb in raw file
   p[i] = (long double)n_i/(long double)n;
   P += p[i];
   N += n_i;
   if (p[i] != 0)                           //log2(0) ---> -infinity which is -nan
       Entrophia -= p[i]*log2l(p[i]);

   #ifdef INFO 
   printf("\n %X |n[i] = %u| p[i] = %Lf ", i, n_i, p[i]); 
   #endif
 }




 printf("\n----------------------------\nEntrophia = %Lf\n----------------------------\n", Entrophia);

 inform(P, N, n);                          //Just info for user
 fclose(file);
 free(Block);
 return 0;
}


