
/***********************************************/
/*     Bibliotecas necessarias ao programa     */
/***********************************************/
#include "stdafx.h"
#include "glui.h"
#include "RGBpixmap.h"
#include "pick.h"
#include "auxs.h"
#include "objects.h"
#include "sockets.h"
#include <math.h>
#include <windows.h>
/***********************************************/


/***************************************************************************/
/*     Variaveis necessarias para as janelas e para os menus de opcoes     */
/***************************************************************************/
int main_window;
GLUI *menu_right, *menu_left, *menu_bottom;
GLUI_Button *button_start_end;
GLUI_Listbox *listbox_theme;
GLUI_Spinner *spinner_level;
GLUI_RadioGroup *radiogroup_game_type;
GLUI_RadioGroup *radiogroup_start_player;
GLUI_StaticText *type_player1;
GLUI_StaticText *type_player2;
GLUI_Button *button_undo;
int themes[4] = {0,1,2,3};
int levels[5] = {0,1,2,3,4};
/***************************************************************************/


/*****************************************************************/
/*     Variaveis necessarias para posiçao, aspecto e textura     */
/*****************************************************************/
int texture = 0;
int texture_marcador1 = 6;
int texture_marcador2 = 6;
float xy_aspect;
RGBpixmap textures[20];
float obj_pos[] = {0.0,0.0,0.0};
float view_rotate[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
/*****************************************************************/


/************************************************/
/*     Variaveis necessarias para o picking     */
/************************************************/
picking *pk;
int obj = -1;
int selected_piece = -1;
float dxAc = 0.0; float dzAc = 0.0; 
float Xini = 0.0; float Yini = 0.0;
float dx = 0.0; float dy = 0.0; float dz = 0.0;
/************************************************/


/***********************************************************/
/*     Variveis necessarias para a definicao das luzes     */
/***********************************************************/
float ambient[] = {0.0,0.0,0.0,1.0};
float lmodel_ambient[] = {0.0,0.0,0.0,1.0};

int light_selected = -1;
float lights_diffuse[8][4] =   {{1.0,1.0,1.0,1.0},
								{1.0,1.0,1.0,1.0},
								{1.0,1.0,1.0,1.0},
								{1.0,1.0,1.0,1.0},
								{1.0,1.0,1.0,1.0},
								{1.0,1.0,1.0,1.0},
								{1.0,1.0,1.0,1.0},
								{1.0,1.0,1.0,1.0}};

float lights_position[8][4] =  {{0.0,0.0,1.0,0.0},
								{0.0,0.0,1.0,0.0},
								{0.0,1.0,0.0,0.0},
								{0.0,1.0,0.0,0.0},
								{1.0,0.0,0.0,0.0},
								{1.0,0.0,0.0,0.0},
								{10.0,10.0,10.0,1.0},
								{10.0,10.0,10.0,1.0}};
/***********************************************************/


/********************************************/
/*     Variaveis auxiliares necessarias     */
/********************************************/
GLUquadric* glQ;
bool flag = false;
bool pick = false;
bool started = false;
bool rotated = false;
float pos_aux[3] = {0.0,0.0,0.0};
double rotateX = 0.0; double rotateZ = 0.0;
double initial_rotationX = 0.0; double initial_rotationZ = 30.0;
bool animation_flag = false; move pc_move;
float animation_rotations[4] = {0.0,0.0,0.0,0.0};
float animation_translation[3] = {0.0,0.0,0.0};
int animation_type = -1;
int animation_piece = -1;
bool move_composed = false;
move pc_move1;
move pc_move2;

bool pc_moved1 = false;
bool pc_moved2 = false;

bool move_selected = false;

int line_selected = -1; int col_selected = -1;
int line_destin = -1; int col_destin = -1;

bool play_again = false;
bool end_game = false;
bool undo_flag = false;
int undo_count = 0;

SYSTEMTIME start_game_time;
SYSTEMTIME start_move_time;
SYSTEMTIME actual_game_time;

float cube_rotation = 0.0;
int timer = 1;
float rotX = 0.0, rotY = 0.0, rotZ = 0.0;

/********************************************/


/***************************************************************/
/*     Variaveis necessarias para a comunicaçao com prolog     */
/***************************************************************/
int level = 1;
int player = 1;
int game_type = 1;
int player_type = 0;
/***************************************************************/


/*****************************************************/
/*     Prototipos das funcoes a usar no programa     */
/*****************************************************/
void define_jogada(void);
void move_piece_up(int,int);
void move_piece_up_left(int,int);
void move_piece_up_right(int,int);
void move_piece_down(int,int);
void move_piece_down_left(int,int);
void move_piece_down_right(int,int);
void move_piece(int,int);
void game_over(void);
void make_move(move);
void draw_scene(GLenum);
void display(void);
void process_hits(GLint,GLuint []);
void start_end(void);
void game_over(void);
void process_mouse(int,int,int,int);
void process_mouse_moved(int,int);
void process_passive_mouse_moved(int,int);
void reshape(int,int);
void keyboard(unsigned char,int,int);
void keyboardSpecials(int,int,int);
void myGlutIdle(void);
void inicializacao(void);
void inicializacao_listas(void);
void inicializacao_prolog(void);
void level_changed(void);
void start_player_changed(void);
void theme_changed(void);
void game_type_changed(void);
void undo(void);
void start_end(void);
void menus_construction(void);
void print_text(char *,int,float *);
void light_update(int,bool);
void reset_animation(void);
void animation(void);
void animation_back(int,float,float);
bool animation_down(int);
void animation_down_left(int,float,float,float,float);
void animation_down_right(int,float,float,float,float);
void animation_front(int,float,float);
void animation_up(int);
void animation_up_left(int,float,float,float,float);
void animation_up_right(int,float,float,float,float);
void update_pieces_position(void);
/*****************************************************/


/*********************************************************************/
/*     Define uma jogada de [line1,column1] para [line2,column2]     */
/*********************************************************************/
void define_jogada(void)
{
	if(game_type != 3)
		button_undo->enable();

	if(play_again)
	{
		move last_move = get_last_move();
		if(line_destin > 10)
			execute_move(last_move.line1,last_move.column1,(line_destin - 10),col_destin);
		else
			execute_move(last_move.line1,last_move.column1,line_destin,col_destin);
		if(player == 1)
			set_player(2);
		else
			set_player(1);
	}
	else
	{
		if(line_destin > 10)
			execute_move(line_selected,col_selected,(line_destin - 10),col_destin);
		else
			execute_move(line_selected,col_selected,line_destin,col_destin);
	}
	
	if(end_test())
	{
		game_over();
		return;
	}
	
	if(black_house(line_destin,col_destin))
	{
		char str[256];
		sprintf(str,"Player %d - You Moved To A Black House!",player);
		if(MessageBox(NULL,"Do You Want To Play Again?",str,MB_YESNO) == IDYES)
		{
			play_again = true;
			set_player(player);
			selected_piece = obj;
			GetLocalTime(&start_move_time);
		}
		else
		{
			play_again = false;
			selected_piece = -1;
			player = get_player();
			player_type = get_player_type();
		}
	}
	else
	{
		play_again = false;
		selected_piece = -1;
		player = get_player();
		player_type = get_player_type();
	}
}
/*********************************************************************/


//*********************************************//
// Funcao que movimenta uma piramide para cima //
//*********************************************//
void move_piece_up(int line, int column)
{
	if(dz < -0.5)
	{
		line_destin = line - 1;
		col_destin = column;

		printf("UP : [%d,%d] - [%d,%d]\n",line,column,line_destin,col_destin);

		glTranslated(0.0,0.1,-2.0);
		glRotated(60.0,0.0,1.0,0.0);
	}
	else
	{
		line_destin = -1;
		col_destin = -1;
	}
}
//*********************************************//


//***************************************************************//
// Funcao que movimenta uma piramide para cima e para a esquerda //
//***************************************************************//
void move_piece_up_left(int line, int column)
{
	if((dz < -0.5) && (dx < -0.5))
	{
		line_destin = line + 10;
		col_destin = column - 1;

		printf("UP_LEFT : [%d,%d] - [%d,%d]\n",line,column,line_destin,col_destin);

		glTranslated(colunas[column - 1] - colunas[column],0.1,linhas_brancas[line] - linhas_pretas[line]);
		glRotated(60.0,0.0,1.0,0.0);
	}
	else
	{
		line_destin = -1;
		col_destin = -1;
	}
}
//***************************************************************//


//**************************************************************//
// Funcao que movimenta uma piramide para cima e para a direita //
//**************************************************************//
void move_piece_up_right(int line, int column)
{
	if((dz < -0.5) && (dx > 0.5))
	{
		line_destin = line + 10;
		col_destin = column + 1;

		printf("UP_RIGHT : [%d,%d] - [%d,%d]\n",line,column,line_destin,col_destin);

		glTranslated(colunas[column + 1] - colunas[column],0.1,linhas_brancas[line] - linhas_pretas[line]);
		glRotated(60.0,0.0,1.0,0.0);
	}
	else
	{
		line_destin = -1;
		col_destin = -1;
	}
}
//**************************************************************//


//**********************************************//
// Funcao que movimenta uma piramide para baixo //
//**********************************************//
void move_piece_down(int line, int column)
{
	if(dz > 0.5)
	{
		line_destin = line + 1;
		col_destin = column;

		printf("DOWN : [%d,%d] - [%d,%d]\n",line,column,line_destin,col_destin);

		glTranslated(0.0,0.1,2.0);
		glRotated(60.0,0.0,1.0,0.0);
	}
	else
	{
		line_destin = -1;
		col_destin = -1;
	}
}
//**********************************************//


//****************************************************************//
// Funcao que movimenta uma piramide para baixo e para a esquerda //
//****************************************************************//
void move_piece_down_left(int line, int column)
{
	if((dz > 0.5) && (dx < -0.5))
	{
		line_destin = line;
		col_destin = column - 1;

		printf("DOWN_LEFT : [%d,%d] - [%d,%d]\n",line,column,line_destin,col_destin);

		glTranslated(colunas[column - 1] - colunas[column],0.1,linhas_pretas[line] - linhas_brancas[line]);
		glRotated(60.0,0.0,1.0,0.0);
	}
	else
	{
		line_destin = -1;
		col_destin = -1;
	}
}
//****************************************************************//


//***************************************************************//
// Funcao que movimenta uma piramide para baixo e para a direita //
//***************************************************************//
void move_piece_down_right(int line, int column)
{
	if((dz > 0.5) && (dx > 0.5))
	{
		line_destin = line;
		col_destin = column + 1;

		printf("DOWN_RIGHT : [%d,%d] - [%d,%d]\n",line,column,line_destin,col_destin);

		glTranslated(colunas[column + 1] - colunas[column],0.1,linhas_pretas[line] - linhas_brancas[line]);
		glRotated(60.0,0.0,1.0,0.0);
	}
	else
	{
		line_destin = -1;
		col_destin = -1;
	}
}
//***************************************************************//


//***********************************//
// Funcao que movimenta uma piramide //
//***********************************//
void move_piece(int line, int column)
{
	col_selected = column;
	
	if(line < 10)
	{
		line_selected = line;
		if((dz < 0.0) && (line > 0) && (valid_move(line,column,(line - 1),column)))
			move_piece_up(line,column);
		else if((dx < 0.0) && (line >= 0) && (valid_move(line,column,line,(column - 1))))
			move_piece_down_left(line,column);
		else if((dx > 0.0) && (line >= 0) && (valid_move(line,column,line,(column + 1))))
			move_piece_down_right(line,column);
	}
	else
	{
		line_selected = line - 10;
		if((dz > 0.0) && (line < 15) && (valid_move((line - 10),column,(line - 9),column)))
			move_piece_down(line,column);
		else if((dx < 0.0) && (line <= 15) && (valid_move((line - 10),column,(line - 10),(column - 1))))
			move_piece_up_left((line - 10),column);
		else if((dx > 0.0) && (line <= 15) && (valid_move((line - 10),column,(line - 10),(column + 1))))
			move_piece_up_right((line - 10),column);
	}
}
//***********************************//


/******************************************************/
/*     Actualiza a posiçao das peças no tabuleiro     */
/******************************************************/
void update_pieces_position(void)
{
	if((line_destin != -1) && (col_destin != -1))
	{
		pos_piramides[obj - 1][0] = colunas[col_destin];

		if(line_destin < 10)
			pos_piramides[obj - 1][2] = linhas_pretas[line_destin];
		else
			pos_piramides[obj - 1][2] = linhas_brancas[line_destin - 10];

		define_jogada();

		if(game_type == 2)
			GetLocalTime(&start_move_time);

		line_destin = -1;
		col_destin = -1;
	}
}
/******************************************************/


/********************************************************/
/*     Indica que o jogo terminou e qual o vencedor     */
/********************************************************/
void game_over(void)
{
	end_game = true;

	if(player == 1)
		texture_marcador1++;
	else
		texture_marcador2++;

	inicializacao_listas();
	start_end();

	char msg[256];
	sprintf(msg,"GAME OVER!!! Player %d WIN!!!",player);

	if(MessageBox(NULL,"Do You Like To Play A New Game?",msg,MB_YESNO) == IDYES)
		start_end();
}
/********************************************************/


/**********************************************/
/*     Executa um movimento do computador     */
/**********************************************/
void make_move(move the_move)
{
	if(black_house(the_move.line1,the_move.column1))
		the_move.line_type1 = 0;
	else
		the_move.line_type1 = 1;

	if(black_house(the_move.line2,the_move.column2))
		the_move.line_type2 = 0;
	else
		the_move.line_type2 = 1;

	int line_int;
	int col_int;

	if(((abs(the_move.line1 - the_move.line2) == 1) && (abs(the_move.column1 - the_move.column2) == 1))
		|| (abs(the_move.line1 - the_move.line2) == 2) || (abs(the_move.column1 - the_move.column2) == 2))
	{
		move_composed = true;

		if((the_move.line1 - the_move.line2) == 0)
		{
			line_int = the_move.line1;
			col_int = ((the_move.column2 - the_move.column1) / 2) + the_move.column1;
		}
		else
		{
			if((the_move.line1 - the_move.line2) == 1)
			{
				line_int = the_move.line2;
				col_int = the_move.column1;
			}
			if(((the_move.line2 - the_move.line1) == 1) && ((the_move.column2 - the_move.column1) == 1))
			{
				line_int = the_move.line1;
				col_int = the_move.column1 + 1;
			}
			if(((the_move.line2 - the_move.line1) == 1) && ((the_move.column1 - the_move.column2) == 1))
			{
				line_int = the_move.line1;
				col_int = the_move.column1 - 1;
			}
		}

		pc_move1.line1 = the_move.line1;
		pc_move1.line_type1 = the_move.line_type1;
		pc_move1.column1 = the_move.column1;
		pc_move1.line2 = line_int;
		pc_move1.line_type2 = 0;
		pc_move1.column2 = col_int;
		pc_move2.line1 = line_int;
		pc_move2.line_type1 = 0;
		pc_move2.column1 = col_int;
		pc_move2.line2 = the_move.line2;
		pc_move2.line_type2 = the_move.line_type2;
		pc_move2.column2 = the_move.column2;
	}
	else
	{
		pc_move = the_move;
		move_composed = false;
	}
}
/**********************************************/


/******************************************/
/*     Limpa as variaveis da animaçao     */
/******************************************/
void reset_animation(void)
{
	animation_rotations[0] = 0.0;
	animation_rotations[1] = 0.0;
	animation_rotations[2] = 0.0;
	animation_rotations[3] = 0.0;

	animation_translation[0] = 0.0;
	animation_translation[1] = 0.0;
	animation_translation[2] = 0.0;
}
/******************************************/


/*********************************************/
/*     Move a peça para cima na animaçao     */
/*********************************************/
void animation_up(int next)
{
	if(animation_translation[1] < 3.0)
		animation_translation[1] += 0.5;
	else
		animation_type = next;
}
/*********************************************/


/**********************************************/
/*     Move a peça para baixo na animaçao     */
/**********************************************/
bool animation_down(int next)
{
	if(animation_translation[1] > 0.0)
	{
		animation_translation[1] -= 0.5;
		return true;
	}
	else
	{
		animation_type = next;
		return false;
	}
}
/**********************************************/


/***************************************************************/
/*     Move a peça para cima e para a esquerda na animaçao     */
/***************************************************************/
void animation_up_left(int next, float x1, float x2, float z1, float z2)
{
	float dist = sqrt(pow(abs(x1 - x2),2.0) + pow(abs(z1 - z2),2.0));

	if(sqrt(pow(abs(animation_translation[0]),2.0) + pow(abs(animation_translation[2]),2.0)) < (dist - 0.1))
	{
		animation_translation[0] -= (dist * sin(alpha)) / 5.0;
		animation_translation[2] -= (dist * cos(alpha)) / 5.0;
	}
	else
		animation_type = next;
}
/***************************************************************/


/**************************************************************/
/*     Move a peça para cima e para a direita na animaçao     */
/**************************************************************/
void animation_up_right(int next, float x1, float x2, float z1, float z2)
{
	float dist = sqrt(pow(abs(x1 - x2),2.0) + pow(abs(z1 - z2),2.0));

	if(sqrt(pow(abs(animation_translation[0]),2.0) + pow(abs(animation_translation[2]),2.0)) < (dist - 0.1))
	{
		printf("Animacao UP RIGHT - %2.2f : %2.2f - %2.2f : %2.2f\n",animation_translation[0],animation_translation[2],(x1 - x2),(z2 - z1));
		animation_translation[0] += (dist * sin(alpha)) / 5.0;
		animation_translation[2] -= (dist * cos(alpha)) / 5.0;
	}
	else
		animation_type = next;
}
/**************************************************************/


/****************************************************************/
/*     Move a peça para baixo e para a esquerda na animaçao     */
/****************************************************************/
void animation_down_left(int next, float x1, float x2, float z1, float z2)
{
	float dist = sqrt(pow(abs(x1 - x2),2.0) + pow(abs(z1 - z2),2.0));

	if(sqrt(pow(abs(animation_translation[0]),2.0) + pow(abs(animation_translation[2]),2.0)) < (dist - 0.1))
	{
		animation_translation[0] -= (dist * sin(alpha)) / 5.0;
		animation_translation[2] += (dist * cos(alpha)) / 5.0;
	}
	else
		animation_type = next;
}
/****************************************************************/


/***************************************************************/
/*     Move a peça para baixo e para a direita na animaçao     */
/***************************************************************/
void animation_down_right(int next, float x1, float x2, float z1, float z2)
{
	float dist = sqrt(pow(abs(x1 - x2),2.0) + pow(abs(z1 - z2),2.0));

	if(sqrt(pow(abs(animation_translation[0]),2.0) + pow(abs(animation_translation[2]),2.0)) < (dist - 0.1))
	{
		animation_translation[0] += (dist * sin(alpha)) / 5.0;
		animation_translation[2] += (dist * cos(alpha)) / 5.0;
	}
	else
		animation_type = next;
}
/***************************************************************/


/*********************************************/
/*     Move a peça para tras na animaçao     */
/*********************************************/
void animation_back(int next, float z1, float z2)
{
	if(abs(animation_translation[2]) < (abs(z1 - z2) - 0.1))
		animation_translation[2] -= abs(z1 - z2) / 4.0;
	else
		animation_type = next;
}
/*********************************************/


/*************************************************/
/*     Move a peça para a frente na animaçao     */
/*************************************************/
void animation_front(int next, float z1, float z2)
{
	if(abs(animation_translation[2]) < (abs(z1 - z2) - 0.1))
		animation_translation[2] += abs(z1 - z2) / 4.0;
	else
		animation_type = next;
}
/*************************************************/


/****************************************/
/*     Executa a animaçao duma peça     */
/****************************************/
void animation(void)
{
	Sleep(100);

	int piramide;

	if(pc_move.line_type1 == 0)
	{
		float x1 = colunas[pc_move.column1];
		float x2 = colunas[pc_move.column2];
		float z1 = linhas_pretas[pc_move.line1];
		float z2 = linhas_brancas[pc_move.line2];

		piramide = get_piramide(pc_move.line1,pc_move.line_type1,pc_move.column1);
		
		if(pc_move.line1 == (pc_move.line2 - 1))
		{
			if(animation_type == -1)
			{
				reset_animation();
				animation_type = UP;
			}
			else
			{
				if(animation_type == UP)
					animation_up(ROTATE);
				else if(animation_type == ROTATE)
				{
					if(animation_rotations[0] < 60.0)
					{
						animation_rotations[0] += 10.0;
						animation_rotations[2] = 1.0;
					}
					else
						animation_type = FRONT;
				}
				else if(animation_type == FRONT)
					animation_front(DOWN,z1,z2);
				else
				{
					if(!animation_down(-1))
					{
						pos_piramides[piramide][2] = linhas_brancas[pc_move.line1 + 1];
						animation_flag = false;
					}
				}
			}
		}
		else if(pc_move.column1 == (pc_move.column2 - 1))
		{
			if(animation_type == -1)
			{
				reset_animation();
				animation_type = UP;
			}
			else
			{
				if(animation_type == UP)
					animation_up(ROTATE);
				else if(animation_type == ROTATE)
				{
					if(animation_rotations[0] < 60.0)
					{
						animation_rotations[0] += 10.0;
						animation_rotations[2] = 1.0;
					}
					else
						animation_type = UP_RIGHT;
				}
				else if(animation_type == UP_RIGHT)
					animation_up_right(DOWN,x1,x2,z1,z2);
				else
				{
					if(!animation_down(-1))
					{
						pos_piramides[piramide][0] = colunas[pc_move.column2];
						pos_piramides[piramide][2] = linhas_brancas[pc_move.line1];
						animation_flag = false;
					}
				}
			}
		}
		else
		{
			if(animation_type == -1)
			{
				reset_animation();
				animation_type = UP;
			}
			else
			{
				if(animation_type == UP)
					animation_up(ROTATE);
				else if(animation_type == ROTATE)
				{
					if(animation_rotations[0] < 60.0)
					{
						animation_rotations[0] += 10.0;
						animation_rotations[2] = 1.0;
					}
					else
						animation_type = UP_LEFT;
				}
				else if(animation_type == UP_LEFT)
					animation_up_left(DOWN,x1,x2,z1,z2);
				else
				{
					if(!animation_down(-1))
					{
						pos_piramides[piramide][0] = colunas[pc_move.column2];
						pos_piramides[piramide][2] = linhas_brancas[pc_move.line1];
						animation_flag = false;
					}
				}
			}
		}
	}
	else
	{
		float x1 = colunas[pc_move.column1];
		float x2 = colunas[pc_move.column2];
		float z1 = linhas_brancas[pc_move.line1];
		float z2 = linhas_pretas[pc_move.line2];

		piramide = get_piramide(pc_move.line1,pc_move.line_type1,pc_move.column1);

		if((pc_move.line1 - 1) == pc_move.line2)
		{
			if(animation_type == -1)
			{
				reset_animation();
				animation_type = UP;
			}
			else
			{
				if(animation_type == UP)
					animation_up(ROTATE);
				else if(animation_type == ROTATE)
				{
					if(animation_rotations[0] < 60.0)
					{
						animation_rotations[0] += 10.0;
						animation_rotations[2] = 1.0;
					}
					else
						animation_type = BACK;
				}
				else if(animation_type == BACK)
					animation_back(DOWN,z1,z2);
				else
				{
					if(!animation_down(-1))
					{
						pos_piramides[piramide][2] = linhas_pretas[pc_move.line1 - 1];
						animation_flag = false;
					}
				}
			}
		}
		else if(pc_move.column1 == (pc_move.column2 - 1))
		{
			if(animation_type == -1)
			{
				reset_animation();
				animation_type = UP;
			}
			else
			{
				if(animation_type == UP)
					animation_up(ROTATE);
				else if(animation_type == ROTATE)
				{
					if(animation_rotations[0] < 60.0)
					{
						animation_rotations[0] += 10.0;
						animation_rotations[2] = 1.0;
					}
					else
						animation_type = DOWN_RIGHT;
				}
				else if(animation_type == DOWN_RIGHT)
					animation_down_right(DOWN,x1,x2,z1,z2);
				else
				{
					if(!animation_down(-1))
					{
						pos_piramides[piramide][0] = colunas[pc_move.column2];
						pos_piramides[piramide][2] = linhas_pretas[pc_move.line1];
						animation_flag = false;
					}
				}
			}
		}
		else
		{
			if(animation_type == -1)
			{
				reset_animation();
				animation_type = UP;
			}
			else
			{
				if(animation_type == UP)
					animation_up(ROTATE);
				else if(animation_type == ROTATE)
				{
					if(animation_rotations[0] < 60.0)
					{
						animation_rotations[0] += 10.0;
						animation_rotations[2] = 1.0;
					}
					else
						animation_type = DOWN_LEFT;
				}
				else if(animation_type == DOWN_LEFT)
					animation_down_left(DOWN,x1,x2,z1,z2);
				else
				{
					if(!animation_down(-1))
					{
						pos_piramides[piramide][0] = colunas[pc_move.column2];
						pos_piramides[piramide][2] = linhas_pretas[pc_move.line1];
						animation_flag = false;
					}
				}
			}
		}
	}
}
/****************************************/


/*****************************************/
/*     Apresenta os objectos no ecra     */
/*****************************************/
void draw_scene(GLenum mode)
{
	int i;
	float pos_piramide[3] = {0.0,0.0,0.0};

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslated(-3.5,-2.0,-30.0);

	if(rotated)
	{
		initial_rotationX += (dx * 5) - rotateX;
		initial_rotationZ += (dz * 5) - rotateZ;

		if(initial_rotationZ > 30.0)
			initial_rotationZ = 30.0;

		rotateX = dx * 5;
		rotateZ = dz * 5;
	}

	glRotated(initial_rotationX,0.0,1.0,0.0);
	glRotated(initial_rotationZ,1.0,0.0,0.0);

	glTranslatef(obj_pos[0],obj_pos[1],-obj_pos[2]);
	glMultMatrixf(view_rotate);

	// Tabuleiro
	glPushMatrix();
	if(mode == GL_SELECT)
		glLoadName(0);
	glCallList(TAB_LIST);
	glPopMatrix();

	if(obj == 0)
		rotated = true;

	// Piramides
	for(i = 0; i < 12; i++)
	{
		int line = get_line(pos_piramides[i][2]);
		int column = get_column(pos_piramides[i][0]);

		pos_piramide[0] = pos_piramides[i][0];
		pos_piramide[1] = pos_piramides[i][1];
		pos_piramide[2] = pos_piramides[i][2];

		if(mode == GL_SELECT)
			glLoadName(i + 1);

		glPushMatrix();

		if(started)
		{
			if(undo_flag)
			{
				if(!move_selected)
				{
					undo();
					glTranslated(pos_piramide[0],pos_piramide[1],pos_piramide[2]);
				}
				else
				{
					if(move_composed)
					{
						if(!pc_moved1 && !pc_moved2)
							pc_moved1 = true;

						if(pc_moved1)
						{
							pc_move = pc_move1;
							animation_piece = get_piramide(pc_move.line1,pc_move.line_type1,pc_move.column1);
							if(animation_flag && (animation_piece == i))
							{
								animation();
								glTranslated(pos_piramide[0],pos_piramide[1],pos_piramide[2]);
								glTranslated(animation_translation[0],animation_translation[1],animation_translation[2]);
								glRotated(animation_rotations[0],animation_rotations[1],animation_rotations[2],animation_rotations[3]);
							}
							else
								glTranslated(pos_piramide[0],pos_piramide[1],pos_piramide[2]);

							if(!animation_flag)
							{
								pc_moved1 = false;
								pc_moved2 = true;
								animation_flag = true;
							}
						}
						else
						{
							pc_move = pc_move2;
							animation_piece = get_piramide(pc_move.line1,pc_move.line_type1,pc_move.column1);
							if(animation_flag && (animation_piece == i))
							{
								animation();
								glTranslated(pos_piramide[0],pos_piramide[1],pos_piramide[2]);
								glTranslated(animation_translation[0],animation_translation[1],animation_translation[2]);
								glRotated(animation_rotations[0],animation_rotations[1],animation_rotations[2],animation_rotations[3]);
							}
							else
								glTranslated(pos_piramide[0],pos_piramide[1],pos_piramide[2]);

							if(!animation_flag)
							{
								pc_moved2 = false;
								if((game_type != 1) || (undo_count == 2))
								{
									undo_count = 0;
									undo_flag = false;
									move_selected = false;
									player_type = get_player_type();
									player = get_player();
									if(player_type == 0)
										GetLocalTime(&start_move_time);
								}
								else
									move_selected = false;
							}
						}
					}
					else
					{
						if(!animation_flag)
						{
							if((game_type != 1) || (undo_count == 2))
							{
								undo_count = 0;
								undo_flag = false;
								pc_moved2 = false;
								move_selected = false;
								player_type = get_player_type();
								player = get_player();
								if(player_type == 0)
									GetLocalTime(&start_move_time);
							}
							else
								move_selected = false;
						}

						animation_piece = get_piramide(pc_move.line1,pc_move.line_type1,pc_move.column1);
						if(animation_flag && (animation_piece == i))
						{
							animation();
							glTranslated(pos_piramide[0],pos_piramide[1],pos_piramide[2]);
							glTranslated(animation_translation[0],animation_translation[1],animation_translation[2]);
							glRotated(animation_rotations[0],animation_rotations[1],animation_rotations[2],animation_rotations[3]);
						}
						else
							glTranslated(pos_piramide[0],pos_piramide[1],pos_piramide[2]);
					}
				}
			}
			else
			{
				if(player_type == 0)
				{
					player = get_player();
					glTranslated(pos_piramide[0],pos_piramide[1],pos_piramide[2]);

					if(!play_again)
					{
						if((obj - 1) == i)
							move_piece(line,column);
					}
					else
					{
						if((selected_piece == obj) && ((obj - 1) == i))
							move_piece(line,column);
					}
				}
				else
				{
					if(!move_selected)
					{
						make_move(select_computer_move());
						move_selected = true;
						animation_flag = true;
						glTranslated(pos_piramide[0],pos_piramide[1],pos_piramide[2]);
					}
					else
					{
						if(move_composed)
						{
							if(!pc_moved1 && !pc_moved2)
								pc_moved1 = true;

							if(pc_moved1)
							{
								pc_move = pc_move1;
								animation_piece = get_piramide(pc_move.line1,pc_move.line_type1,pc_move.column1);
								if(animation_flag && (animation_piece == i))
								{
									animation();
									glTranslated(pos_piramide[0],pos_piramide[1],pos_piramide[2]);
									glTranslated(animation_translation[0],animation_translation[1],animation_translation[2]);
									glRotated(animation_rotations[0],animation_rotations[1],animation_rotations[2],animation_rotations[3]);
								}
								else
									glTranslated(pos_piramide[0],pos_piramide[1],pos_piramide[2]);

								if(!animation_flag)
								{
									pc_moved1 = false;
									pc_moved2 = true;
									animation_flag = true;
								}
							}
							else
							{
								pc_move = pc_move2;
								animation_piece = get_piramide(pc_move.line1,pc_move.line_type1,pc_move.column1);
								if(animation_flag && (animation_piece == i))
								{
									animation();
									glTranslated(pos_piramide[0],pos_piramide[1],pos_piramide[2]);
									glTranslated(animation_translation[0],animation_translation[1],animation_translation[2]);
									glRotated(animation_rotations[0],animation_rotations[1],animation_rotations[2],animation_rotations[3]);
								}
								else
									glTranslated(pos_piramide[0],pos_piramide[1],pos_piramide[2]);

								if(!animation_flag)
								{
									if(end_test())
										game_over();
									pc_moved2 = false;
									move_selected = false;
									player_type = get_player_type();
									//player = get_player();
									if(player_type == 0)
										GetLocalTime(&start_move_time);
								}
							}
						}
						else
						{
							if(!animation_flag)
							{
								if(end_test())
										game_over();
								move_selected = false;
								player_type = get_player_type();
								//player = get_player();
								if(player_type == 0)
									GetLocalTime(&start_move_time);
							}

							animation_piece = get_piramide(pc_move.line1,pc_move.line_type1,pc_move.column1);
							if(animation_flag && (animation_piece == i))
							{
								animation();
								glTranslated(pos_piramide[0],pos_piramide[1],pos_piramide[2]);
								glTranslated(animation_translation[0],animation_translation[1],animation_translation[2]);
								glRotated(animation_rotations[0],animation_rotations[1],animation_rotations[2],animation_rotations[3]);
							}
							else
								glTranslated(pos_piramide[0],pos_piramide[1],pos_piramide[2]);
						}
					}
				}
			}
		}
		else
			glTranslated(pos_piramide[0],pos_piramide[1],pos_piramide[2]);

		if(line < 10)
			glRotated(-90.0,0.0,1.0,0.0);
		else
			glRotated(90.0,0.0,1.0,0.0);

		glCallList(i + 2);
		glPopMatrix();
	}

	// Marcador 1
	glPushMatrix();
	glTranslated(-8.0,4.0,-4.0);
	glRotated(60.0,0.0,1.0,0.0);
	glCallList(MARCAD1_LIST);
	glPopMatrix();

	// Marcador 2
	glPushMatrix();
	glTranslated(8.0,4.0,-4.0);
	glRotated(-60.0,0.0,1.0,0.0);
	glCallList(MARCAD2_LIST);
	glPopMatrix();

	// Label do Marcador 1
	glPushMatrix();
	glTranslated(-8.0,7.0,-4.0);
	glRotated(60.0,0.0,1.0,0.0);
	glCallList(LABEL1_LIST);
	glPopMatrix();

	// Label do Marcador 2
	glPushMatrix();
	glTranslated(8.0,7.0,-4.0);
	glRotated(-60.0,0.0,1.0,0.0);
	glCallList(LABEL2_LIST);
	glPopMatrix();

	// Cubo
	glPushMatrix();
	if(!started)
	{
		if(timer == 0)
		{
			timer = 1;
			int random = rand();
			if((random % 10) < 5)
				rotX = -1.0;
			else
				rotX = 1.0;
			random = rand();
			if((random % 10) < 5)
				rotY = -1.0;
			else
				rotY = 1.0;
			random = rand();
			if((random % 10) < 5)
				rotZ = -1.0;
			else
				rotZ = 1.0;
			random = rand();
			cube_rotation += (float) (random % 10);
		}
		else
			timer--;

		glTranslated(0.0,5.0,0.0);
		glRotated(cube_rotation,rotX,rotY,rotZ);
	}
	else
	{
		glTranslated(0.0,4.0,-12.0);
		glRotated(45.0,0.0,1.0,0.0);		
	}
	glCallList(CUBE_LIST);
	glPopMatrix();
}
/*****************************************/


/*****************************************************/
/*     Visualiza a cena e o texto correspondente     */
/*****************************************************/
void display(void)
{
	GetLocalTime(&actual_game_time);
		
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-xy_aspect * 0.04,xy_aspect * 0.04,-0.04,0.04,0.1,50.0);
	glPolygonMode(GL_FRONT,GL_FILL);
	draw_scene(GL_RENDER);

	char str[256];
	float color[] = {1.0,0.0,0.0};
	
	if(!started)
	{
		if(end_game)
		{
			print_text("Game Over",95,color);
			color[0] = 0.0; color[1] = 0.0; color[2] = 1.0;
			sprintf(str,"Player %d WIN!!!",player);
			print_text(str,91,color);
		}
		else
			print_text("Welcome to Sahara Game!!!",95,color);
	}
	else
	{
		if(undo_flag)
		{
			sprintf(str,"Making undo of Player %d move...",player);
			print_text(str,95,color);
		}
		else
		{
			if(player_type == 0)
			{
				char move_time[256];
				int move_minutes = actual_game_time.wMinute - start_move_time.wMinute;
				int move_seconds = actual_game_time.wSecond - start_move_time.wSecond;

				if(move_seconds < 0)
				{
					move_minutes--;
					move_seconds = 60 + move_seconds;
				}

				int move_minutes_dif = 1 - move_minutes;
				int move_seconds_dif = 0 - move_seconds;

				if(move_seconds_dif < 0)
				{
					move_minutes_dif--;
					move_seconds_dif = 60 + move_seconds_dif;
				}

				if((move_minutes_dif == 0) && (move_seconds_dif == 0))
				{
					if(player == 1)
						player = 2;
					else
						player = 1;

					game_over();
					glutSwapBuffers();
					glFlush();
					return;
				}
				else
				{
					sprintf(str,"Player %d",player);
					print_text(str,95,color);
					color[0] = 0.0; color[1] = 0.0; color[2] = 1.0;
					sprintf(move_time,"Time Left To Play - %.2d:%.2d",move_minutes_dif,move_seconds_dif);
					print_text(move_time,91,color);
				}
			}
			else
			{
				sprintf(str,"Player %d playing...",player);
				print_text(str,95,color);
			}
		}
	}

	color[0] = 0.0; color[1] = 1.0; color[2] = 0.0;

	if(started)
	{
		char time[256];

		int minutes = actual_game_time.wMinute - start_game_time.wMinute;
		int seconds = actual_game_time.wSecond - start_game_time.wSecond;

		if(seconds < 0)
		{
			minutes--;
			seconds = 60 + seconds;
		}

		sprintf(time,"Game Time - %.2d:%.2d",minutes,seconds);
		print_text(time,87,color);
	}
	else
		print_text("Game Time - 00:00",87,color);

	glutSwapBuffers();
	glFlush();
}
/*****************************************************/


/************************************/
/*     Imprime texto no cenario     */
/************************************/
void print_text(char *str, int y, float *color)
{
	int i;

	glDisable(GL_LIGHTING);
	glPushMatrix();
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0,100.0,0.0,100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3fv(color);
	glRasterPos2i((80 - (int) strlen(str)) / 2,y);

	for(i = 0; i < (int) strlen(str); i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,str[i]);

	glPopMatrix();
	glEnable(GL_LIGHTING);
}
/************************************/


/************************************/
/*     Procassa os hits do rato     */
/************************************/
void process_hits(GLint hits,GLuint buffer[])
{
	int i;
	unsigned int j;
    GLuint names;
	GLuint *ptr;
	unsigned int z1 = 0;
	unsigned int z2 = 0;

	pk->resetEstados();

	if(hits == 0)
		return;

    ptr = (GLuint *) buffer;

    for(i = 0; i < hits; i++) 
	{
		names = *ptr;
		ptr++;

		if(i == 0)
			z1 = (unsigned int) *ptr;

		if ((unsigned int) *ptr <= z1)
		{	
			z1 = (unsigned int)*ptr; ptr++;
			z2 = (unsigned int)*ptr; ptr++;
			obj = *ptr;
		}
		else 
		{
			ptr++;
			ptr++;
		}
		
		for(j = 0; j < names; j++)
			ptr++;
    }

	pk->setEstado(obj);
	dx = dxAc = pk->getDx(obj);
	dz = dzAc = pk->getDz(obj);
}
/************************************/


/***************************************/
/*     Processa os eventos do rato     */
/***************************************/
void process_mouse(int button,int state,int x,int y)
{
	if(button == GLUT_RIGHT_BUTTON)
	{
		initial_rotationX = 0.0;
		initial_rotationZ = 30.0;
	}

	GLuint selectBuf[BUFSIZE];
    GLint hits;
    GLint viewport[4];

	Xini = x * view_rotate[0] - y * view_rotate[8];
	Yini = -x * view_rotate[2] + y * view_rotate[10];

	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		update_pieces_position();
		flag = false;
		rotated = false;
		dx = 0.0; dz = 0.0;
		rotateX = 0.0; rotateZ = 0.0;
		pk->setDxyz(dx,0.0,dz);
		pk->resetEstados();
		obj = -1;
		return;
	}

	flag = true;

    glGetIntegerv(GL_VIEWPORT,viewport);
    glSelectBuffer(BUFSIZE,selectBuf);
    (void) glRenderMode(GL_SELECT);

	glInitNames();
    glPushName(-1);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix((GLdouble) x,(GLdouble) (viewport[3] - y),5.0,5.0,viewport);
    glFrustum(-xy_aspect * 0.04,xy_aspect * 0.04,-0.04,0.04,0.1,50.0);
	draw_scene(GL_SELECT);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glFlush();

    hits = glRenderMode(GL_RENDER);
    process_hits(hits,selectBuf);
}
/***************************************/


/****************************************/
/*     Processa o movimento do rato     */
/****************************************/
void process_mouse_moved(int x,int y)
{
	if(flag)
	{
		int x1, y1;
		
		x1 = x * view_rotate[0] - y * view_rotate[8];
		y1 = -x * view_rotate[2] + y * view_rotate[10];

		dx = dxAc + (float) (x1 - Xini) / 20.0;
		dz = dzAc + (float) (y1 - Yini) / 20.0;	
	}

	glutPostRedisplay();
}
/****************************************/


/************************************************/
/*     Processa o movimento passivo do rato     */
/************************************************/
void process_passive_mouse_moved(int x,int y)
{
	glutPostRedisplay();				
}
/************************************************/


/********************************/
/*     Actualiza o Viewport     */
/********************************/
void reshape(int w,int h)
{
	xy_aspect = (float) w / (float) h;
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glutPostRedisplay();
}
/********************************/


/************************************************/
/*     Aumenta/Diminui a intensidade da luz     */
/************************************************/
void light_update(int light, bool more)
{
	if((light_selected >= GL_LIGHT0) && (light_selected <= GL_LIGHT7))
	{
		if(more && (lights_diffuse[light - GL_LIGHT0][0] < 1.0))
		{
			lights_diffuse[light - GL_LIGHT0][0] += 0.1;
			lights_diffuse[light - GL_LIGHT0][1] += 0.1;
			lights_diffuse[light - GL_LIGHT0][2] += 0.1;
			lights_diffuse[light - GL_LIGHT0][3] += 0.1;
		}
		else
		{
			if(!more && (lights_diffuse[light - GL_LIGHT0][0] > 0.0))
			{
				lights_diffuse[light - GL_LIGHT0][0] -= 0.1;
				lights_diffuse[light - GL_LIGHT0][1] -= 0.1;
				lights_diffuse[light - GL_LIGHT0][2] -= 0.1;
				lights_diffuse[light - GL_LIGHT0][3] -= 0.1;
			}
		}
		glLightfv(light,GL_DIFFUSE,lights_diffuse[light - GL_LIGHT0]);
	}
}
/************************************************/


/***************************************/
/*     Trata os eventos do teclado     */
/***************************************/
void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 27:
			exit(0);
			break;
		case '+':
			light_update(light_selected,true);
			break;
		case '-':
			light_update(light_selected,false);
			break;
	}
}
/***************************************/


/*************************************************/
/*     Trata os eventos especiais do teclado     */
/*************************************************/
void keyboardSpecials(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_UP:
			initial_rotationZ -= 2.0;
			break;
		case GLUT_KEY_DOWN:
			initial_rotationZ += 2.0;
			break;
		case GLUT_KEY_LEFT:
			initial_rotationX -= 2.0;
			break;
		case GLUT_KEY_RIGHT:
			initial_rotationX += 2.0;
			break;
		case GLUT_KEY_HOME:
			initial_rotationX = 0.0;
			initial_rotationZ = 45.0;
			break;
		case GLUT_KEY_F1:
			if(glIsEnabled(GL_LIGHT0))
			{
				glDisable(GL_LIGHT0);
				light_selected = -1;
			}
			else
			{
				glEnable(GL_LIGHT0);
				light_selected = GL_LIGHT0;
			}
			break;
		case GLUT_KEY_F2:
			if(glIsEnabled(GL_LIGHT1))
			{
				glDisable(GL_LIGHT1);
				light_selected = -1;
			}
			else
			{
				glEnable(GL_LIGHT1);
				light_selected = GL_LIGHT1;
			}
			break;
		case GLUT_KEY_F3:
			if(glIsEnabled(GL_LIGHT2))
			{
				glDisable(GL_LIGHT2);
				light_selected = -1;
			}
			else
			{
				glEnable(GL_LIGHT2);
				light_selected = GL_LIGHT2;
			}
			break;
		case GLUT_KEY_F4:
			if(glIsEnabled(GL_LIGHT3))
			{
				glDisable(GL_LIGHT3);
				light_selected = -1;
			}
			else
			{
				glEnable(GL_LIGHT3);
				light_selected = GL_LIGHT3;
			}
			break;
		case GLUT_KEY_F5:
			if(glIsEnabled(GL_LIGHT4))
			{
				glDisable(GL_LIGHT4);
				light_selected = -1;
			}
			else
			{
				glEnable(GL_LIGHT4);
				light_selected = GL_LIGHT4;
			}
			break;
		case GLUT_KEY_F6:
			if(glIsEnabled(GL_LIGHT5))
			{
				glDisable(GL_LIGHT5);
				light_selected = -1;
			}
			else
			{
				glEnable(GL_LIGHT5);
				light_selected = GL_LIGHT5;
			}
			break;
		case GLUT_KEY_F7:
			if(glIsEnabled(GL_LIGHT6))
			{
				glDisable(GL_LIGHT6);
				light_selected = -1;
			}
			else
			{
				glEnable(GL_LIGHT6);
				light_selected = GL_LIGHT6;
			}
			break;
		case GLUT_KEY_F8:
			if(glIsEnabled(GL_LIGHT7))
			{
				glDisable(GL_LIGHT7);
				light_selected = -1;
			}
			else
			{
				glEnable(GL_LIGHT7);
				light_selected = GL_LIGHT7;
			}
			break;
	}
}
/*************************************************/


/**/
/**/
void myGlutIdle(void)
{
  if(glutGetWindow() != main_window)
    glutSetWindow(main_window);

  glutPostRedisplay();

  /****************************************************************/
  /*            This demonstrates GLUI::sync_live()               */
  /*   We change the value of a variable that is 'live' to some   */
  /*   control.  We then call sync_live, and the control          */
  /*   associated with that variable is automatically updated     */
  /*   with the new value.  This frees the programmer from having */
  /*   to always remember which variables are used by controls -  */
  /*   simply change whatever variables are necessary, then sync  */
  /*   the live ones all at once with a single call to sync_live  */
  /****************************************************************/

  //menu_right->sync_live();
}
/**/


//**************************************************
// Funcao que inicializa os componentes
//**************************************************
void inicializacao(void)
{
	//**************************************************
	// Inicializacao de variaveis e definicoes
	//**************************************************

		// Definicoes necessarias ao programa
		glFrontFace(GL_CCW);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT,lmodel_ambient);
		glClearColor(1.0,1.0,1.0,1.0);

		// Definicoes das luzes e activacao das mesmas
		glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
		glLightfv(GL_LIGHT0,GL_DIFFUSE,lights_diffuse[0]);
		glLightfv(GL_LIGHT0,GL_POSITION,lights_position[0]);

		glLightfv(GL_LIGHT1,GL_AMBIENT,ambient);
		glLightfv(GL_LIGHT1,GL_DIFFUSE,lights_diffuse[1]);
		glLightfv(GL_LIGHT1,GL_POSITION,lights_position[1]);

		glLightfv(GL_LIGHT2,GL_AMBIENT,ambient);
		glLightfv(GL_LIGHT2,GL_DIFFUSE,lights_diffuse[2]);
		glLightfv(GL_LIGHT2,GL_POSITION,lights_position[2]);

		glLightfv(GL_LIGHT3,GL_AMBIENT,ambient);
		glLightfv(GL_LIGHT3,GL_DIFFUSE,lights_diffuse[3]);
		glLightfv(GL_LIGHT3,GL_POSITION,lights_position[3]);

		glLightfv(GL_LIGHT4,GL_AMBIENT,ambient);
		glLightfv(GL_LIGHT4,GL_DIFFUSE,lights_diffuse[4]);
		glLightfv(GL_LIGHT4,GL_POSITION,lights_position[4]);

		glLightfv(GL_LIGHT5,GL_AMBIENT,ambient);
		glLightfv(GL_LIGHT5,GL_DIFFUSE,lights_diffuse[5]);
		glLightfv(GL_LIGHT5,GL_POSITION,lights_position[5]);

		glLightfv(GL_LIGHT6,GL_AMBIENT,ambient);
		glLightfv(GL_LIGHT6,GL_DIFFUSE,lights_diffuse[6]);
		glLightfv(GL_LIGHT6,GL_POSITION,lights_position[6]);

		glLightfv(GL_LIGHT7,GL_AMBIENT,ambient);
		glLightfv(GL_LIGHT7,GL_DIFFUSE,lights_diffuse[7]);
		glLightfv(GL_LIGHT7,GL_POSITION,lights_position[7]);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);

		// Outras definicoes
		glShadeModel(GL_SMOOTH);
		glPolygonMode(GL_FRONT,GL_FILL);
		glQ = gluNewQuadric();
		gluQuadricOrientation(glQ,GLU_OUTSIDE);

	//**************************************************

	//**************************************************
	// Inicializacao das texturas
	//**************************************************
	
		int i;

		// Textura da marmore
		textures[0].readBMPFile("marmore.bmp");
		textures[0].setTexture(0);

		// Textura da neve
		textures[1].readBMPFile("neve.bmp"); 
		textures[1].setTexture(1);

		// Textura da madeira
		textures[2].readBMPFile("madeira.bmp");
		textures[2].setTexture(2);

		// Textura do metal
		textures[3].readBMPFile("metal.bmp");
		textures[3].setTexture(3);

		// Textura da areia
		textures[4].readBMPFile("yellowsand1.bmp");
		textures[4].setTexture(4);

		textures[5].readBMPFile("coarsesand.bmp");
		textures[5].setTexture(5);

		// Texturas dos numeros
		for(i = 0; i < 10; i++)
		{
			char str[5];
			sprintf(str,"%d.bmp",i);
			textures[i + 6].readBMPFile(str);
			textures[i + 6].setTexture(i + 6);
		}

		// Textura da label do jogador 1
		textures[16].readBMPFile("player1.bmp");
		textures[16].setTexture(16);

		// Textura da label do jogador 2
		textures[17].readBMPFile("player2.bmp");
		textures[17].setTexture(17);

		// Textura do filme
		textures[18].readBMPFile("movie.bmp");
		textures[18].setTexture(18);

	//**************************************************

}
//**************************************************


//**************************************************
// Funcao que inicializa as listas a usar
//**************************************************
void inicializacao_listas(void)
{
	int i;

	//**************************************************
	// Contrucao das listas a usar no desenho
	//**************************************************

		if(!end_game)
		{
			// Lista do tabuleiro
			glNewList(TAB_LIST,GL_COMPILE);
				glBindTexture(GL_TEXTURE_2D,texture);
				glEnable(GL_TEXTURE_2D);
				tabuleiro(TAB_SIZE,1.0,6 * (raio_triangulo + (raio_triangulo * cos(alpha))));
				glDisable(GL_TEXTURE_2D);
			glEndList();

			// Listas das piramides
			for(i = 0; i < 6; i++)
			{
				glNewList(((i * 2) + 2),GL_COMPILE);
					glBindTexture(GL_TEXTURE_2D,4);
					glEnable(GL_TEXTURE_2D);
					piramide(raio_triangulo,1);
					glDisable(GL_TEXTURE_2D);
				glEndList();

				glNewList(((i * 2) + 3),GL_COMPILE);
					glBindTexture(GL_TEXTURE_2D,5);
					glEnable(GL_TEXTURE_2D);
					piramide(raio_triangulo,0);
					glDisable(GL_TEXTURE_2D);
				glEndList();
			}
		}

		// Lista do marcador 1
		glNewList(MARCAD1_LIST,GL_COMPILE);
			glBindTexture(GL_TEXTURE_2D,texture_marcador1);
			glEnable(GL_TEXTURE_2D);
			marcador(6.0,4.0,0.3);
			glDisable(GL_TEXTURE_2D);
		glEndList();

		// Lista do marcador 2
		glNewList(MARCAD2_LIST,GL_COMPILE);
			glBindTexture(GL_TEXTURE_2D,texture_marcador2);
			glEnable(GL_TEXTURE_2D);
			marcador(6.0,4.0,0.3);
			glDisable(GL_TEXTURE_2D);
		glEndList();

		// Lista da label do marcador 1
		glNewList(LABEL1_LIST,GL_COMPILE);
			glBindTexture(GL_TEXTURE_2D,LABEL1_LIST - 1);
			glEnable(GL_TEXTURE_2D);
			marcador(6.0,1.5,0.3);
			glDisable(GL_TEXTURE_2D);
		glEndList();

		// Lista da label do marcador 1
		glNewList(LABEL2_LIST,GL_COMPILE);
			glBindTexture(GL_TEXTURE_2D,LABEL2_LIST - 1);
			glEnable(GL_TEXTURE_2D);
			marcador(6.0,1.5,0.3);
			glDisable(GL_TEXTURE_2D);
		glEndList();

		// Lista do cubo
		glNewList(CUBE_LIST,GL_COMPILE);
			glBindTexture(GL_TEXTURE_2D,CUBE_LIST - 1);
			glEnable(GL_TEXTURE_2D);
			cubo(4.0);
			glDisable(GL_TEXTURE_2D);
		glEndList();

	//**************************************************

}
//**************************************************


/*********************************************************/
/*     Inicializa a comunicaçao com o SICStus Prolog     */
/*********************************************************/
void inicializacao_prolog(void)
{
	char goal[14];
	sprintf(goal,"server(%d).",PORT);
	create_bat("\"C:\\Program Files\\SICStus Prolog 3.9.1\\bin\\sicstus.exe\"","d:\\Trab2\\Release\\sockets.pl",goal);
	run_sicstus();
	socket_create();
}
/*********************************************************/


/************************************************/
/*     Muda o tema das peças e do tabuleiro     */
/************************************************/
void theme_changed(void)
{
	texture = listbox_theme->get_int_val();
	inicializacao_listas();
}
/************************************************/


/********************************/
/*     Muda o nivel do jogo     */
/********************************/
void level_changed(void)
{
	level = spinner_level->get_int_val();
}
/********************************/


/******************************/
/*     Muda o tipo de jogo    */
/******************************/
void game_type_changed(void)
{
	game_type = (radiogroup_game_type->get_int_val()) + 1;

	if(game_type == 1)
	{
		type_player1->set_name("Humano");
		type_player2->set_name("Maquina");
		spinner_level->enable();
	}

	if(game_type == 2)
	{
		type_player1->set_name("Humano");
		type_player2->set_name("Humano");
		spinner_level->disable();
	}

	if(game_type == 3)
	{
		type_player1->set_name("Maquina");
		type_player2->set_name("Maquina");
		spinner_level->enable();
	}
}
/******************************/


/********************************************/
/*     Muda o jogador que inicia o jogo     */
/********************************************/
void start_player_changed(void)
{
	player = (radiogroup_start_player->get_int_val()) + 1;
}
/********************************************/


/***************************************************/
/*     Anula a ultima jogada do jogador humano     */
/***************************************************/
void undo(void)
{
	undo_flag = true;

	move last_move = get_last_move();
	move last_move_inverse;

	if(get_player() == 1)
		player = 2;
	else
		player = 1;

	last_move_inverse.line1 = last_move.line2;
	last_move_inverse.line_type1 = last_move.line_type2;
	last_move_inverse.column1 = last_move.column2;
	last_move_inverse.line2 = last_move.line1;
	last_move_inverse.line_type2 = last_move.line_type1;
	last_move_inverse.column2 = last_move.column1;

	make_move(last_move_inverse);
	move_selected = true;
	animation_flag = true;

	undo_count++;

	if(player == 1)
		player = 2;
	else
		player = 1;

	if(get_num_moves() == 0)
			button_undo->disable();
}
/***************************************************/


/*********************************/
/*     Inicia/Termina o jogo     */
/*********************************/
void start_end(void)
{
	if(strcmp(button_start_end->name,"Start") == 0)
	{
		game_type_changed();
		start_player_changed();
		level_changed();
		inicializacao_prolog();
		spinner_level->disable();
		radiogroup_game_type->disable();
		radiogroup_start_player->disable();
		button_start_end->set_name("End Game");
		set_level(level);
		set_player(player);
		set_game_type(game_type);
		player_type = get_player_type();
		initialize_pos_piramides();
		GetLocalTime(&start_game_time);
		if(player_type == 0)
			GetLocalTime(&start_move_time);
		started = true;
		end_game = false;
	}
	else
	{
		socket_send("client_shutdown.");
		printf("A receber: %s",socket_receive());
		socket_close();
		spinner_level->enable();
		radiogroup_game_type->enable();
		radiogroup_start_player->enable();
		button_undo->disable();
		button_start_end->set_name("Start");
		started = false;
	}
}
/*********************************/


/*************************************/
/*     Constroi o menu de opcoes     */
/*************************************/
void menus_construction(void)
{
	// Menu com as opcoes do jogo
	menu_right = GLUI_Master.create_glui_subwindow(main_window,GLUI_SUBWINDOW_RIGHT);
	menu_right->set_main_gfx_window(main_window);

	GLUI_Panel *panel_separator1 = menu_right->add_panel("",GLUI_PANEL_NONE);

	button_start_end = menu_right->add_button("Start",0,(GLUI_Update_CB) start_end);
	button_start_end->enable();

	GLUI_Panel *panel_separator2 = menu_right->add_panel("",GLUI_PANEL_NONE);
	menu_right->add_separator();
	
	GLUI_Panel *panel_separator3 = menu_right->add_panel("",GLUI_PANEL_NONE);

	listbox_theme = menu_right->add_listbox("Tema",NULL,0,(GLUI_Update_CB) theme_changed);
		listbox_theme->add_item(0,"Marmore");
		listbox_theme->add_item(1,"Neve");
		listbox_theme->add_item(2,"Madeira");
		listbox_theme->add_item(3,"Metal");

	spinner_level = menu_right->add_spinner("Nivel",GLUI_SPINNER_INT,NULL,1,(GLUI_Update_CB) level_changed);
	spinner_level->set_int_limits(1,5,GLUI_LIMIT_CLAMP);

	GLUI_Panel *panel_separator4 = menu_right->add_panel("",GLUI_PANEL_NONE);
	menu_right->add_separator();
	GLUI_Panel *panel_separator5 = menu_right->add_panel("",GLUI_PANEL_NONE);

	GLUI_Panel *panel_game_type = menu_right->add_panel("Tipo de Jogo",GLUI_PANEL_EMBOSSED);
	radiogroup_game_type = menu_right->add_radiogroup_to_panel(panel_game_type,NULL,0,(GLUI_Update_CB) game_type_changed);
		GLUI_RadioButton *radiobutton1 = menu_right->add_radiobutton_to_group(radiogroup_game_type,"Humano vs Maquina");
		GLUI_RadioButton *radiobutton2 = menu_right->add_radiobutton_to_group(radiogroup_game_type,"Humano vs Humano");
		GLUI_RadioButton *radiobutton3 = menu_right->add_radiobutton_to_group(radiogroup_game_type,"Maquina vs Maquina");

	GLUI_Panel *panel_separator6 = menu_right->add_panel("",GLUI_PANEL_NONE);

	GLUI_Panel *panel_start_player = menu_right->add_panel("Quem inicia o jogo? ",GLUI_PANEL_EMBOSSED);
	
		radiogroup_start_player = menu_right->add_radiogroup_to_panel(panel_start_player,NULL,0,(GLUI_Update_CB) start_player_changed);
		GLUI_RadioButton *radiobutton_1 = menu_right->add_radiobutton_to_group(radiogroup_start_player,"Jogador 1");
		GLUI_RadioButton *radiobutton_2 = menu_right->add_radiobutton_to_group(radiogroup_start_player,"Jogador 2");

	GLUI_Panel *panel_separator7 = menu_right->add_panel("",GLUI_PANEL_NONE);
	menu_right->add_separator();
	GLUI_Panel *panel_separator8 = menu_right->add_panel("",GLUI_PANEL_NONE);

	GLUI_Panel *panel_player1 = menu_right->add_panel("Jogador 1",GLUI_PANEL_EMBOSSED);
			type_player1 = menu_right->add_statictext_to_panel(panel_player1,"Humano");
			GLUI_StaticText *pieces_player1 = menu_right->add_statictext_to_panel(panel_player1,"Pecas Escuras");

	GLUI_Panel *panel_player2 = menu_right->add_panel("Jogador 2",GLUI_PANEL_EMBOSSED);
			type_player2 = menu_right->add_statictext_to_panel(panel_player2,"Maquina");
			GLUI_StaticText *pieces_player2 = menu_right->add_statictext_to_panel(panel_player2,"Pecas Claras");

	GLUI_Panel *panel_separator9 = menu_right->add_panel("",GLUI_PANEL_NONE);
	menu_right->add_separator();
	GLUI_Panel *panel_separator10 = menu_right->add_panel("",GLUI_PANEL_NONE);

	button_undo = menu_right->add_button("Undo",0,(GLUI_Update_CB) undo);
	button_undo->disable();
}
/*************************************/


/****************/
/*     MAIN     */
/****************/
int main(int argc, char* argv[])
{
	//**************************************************
	// Inicializacoes do glut para a janela
	//**************************************************
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(DIMX,DIMY);
	glutInitWindowPosition(INITIALPOS_X,INITIALPOS_Y);
	main_window = glutCreateWindow(argv[0]);
	//**************************************************

	//**************************************************
	// Inicializacoes do glut para o tratamento de eventos
	//**************************************************
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboardSpecials);
	glutMouseFunc(process_mouse);
	glutMotionFunc(process_mouse_moved);
	glutPassiveMotionFunc(process_passive_mouse_moved);
	//**************************************************

	menus_construction();

	GLUI_Master.set_glutIdleFunc(myGlutIdle);
	inicializacao();
	inicializacao_listas();

	pk = new picking(13);
	glutMainLoop();
	delete pk;

	return 0;
}
/****************/
