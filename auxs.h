
/****************************************************/
/*     Definicao do tamanho e posicao da janela     */
/****************************************************/
#define DIMX		 900
#define DIMY		 675
#define INITIALPOS_X ((1024 - DIMX) / 2)
#define INITIALPOS_Y ((768 - DIMY) / 2)
/****************************************************/


/***************************************************/
/*     Definicao das listas a usar no programa     */
/***************************************************/
#define TAB_LIST	 1
#define MARCAD1_LIST 15
#define MARCAD2_LIST 16
#define LABEL1_LIST	 17
#define LABEL2_LIST	 18
#define CUBE_LIST    19
/***************************************************/


/****************************************************************/
/*     Definicao de variaveis auxiliares a usar no programa     */
/****************************************************************/
#define VLENGTH		 3
#define PI			 3.14159265
#define BUFSIZE		 512
#define TAB_SIZE	 20.0
#define PICKLENGTH	 4.0
/****************************************************************/


/*********************************************************/
/*     Definicao de variveis a usar para a animaçao      */
/*********************************************************/
#define ROTATE 0
#define UP 1
#define DOWN 2
#define BACK 3
#define FRONT 4
#define UP_LEFT 5
#define UP_RIGHT 6
#define DOWN_LEFT 7
#define DOWN_RIGHT 8
/*********************************************************/


/*************************************************/
/*     Estrutura que representa um movimento     */
/*************************************************/
typedef struct
{
	int line1; int line_type1; int column1;
	int line2; int line_type2; int column2;
} move;
/*************************************************/


/*********************************************/
/*     Devolve o modulo dum valor double     */
/*********************************************/
double abs(double value)
{
	if(value >= 0.0)
		return value;
	else
		return (0.0 - value);
}
/*********************************************/

