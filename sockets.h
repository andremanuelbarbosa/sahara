
/***********************************************/
/*     Bibliotecas necessarias ao programa     */
/***********************************************/
#include <winsock.h>
/***********************************************/


/**********************************************/
/*     Ligaçao com a libraria wsock32.lib     */
/**********************************************/
#ifdef WIN32
#pragma comment(lib,"wsock32.lib")
#endif
/**********************************************/


/************************************************************/
/*     Definiçao de variaveis finais a usar no programa     */
/************************************************************/
#define HOST "localhost"
#define PORT 1111
/************************************************************/


/*************************************************************/
/*     Definiçao de variaveis globais a usar no programa     */
/*************************************************************/
SOCKET clientSocket;
char msg[256];
/*************************************************************/


/***********************************************************************/
/*     Cria o ficheiro bat necessario para correr o SICStus Prolog     */
/***********************************************************************/
bool create_bat(char *sicstus_path, char *file_path, char *start_goal)
{
	char aux[256] = {'\0'};
	char *str;

	str = strcat(aux,sicstus_path);
	str = strcat(aux," -l ");
	str = strcat(aux,file_path);
	str = strcat(aux," --goal ");
	str = strcat(aux,start_goal);

	FILE *fp;

	if((fp = fopen("sicstus.bat","w")) == NULL)
		return false;

	fputs(str,fp);
	fclose(fp);

	return true;
}
/***********************************************************************/


/**********************************************************/
/*     Corre o SICStus Prolog numa nova janela de DOS     */
/**********************************************************/
void run_sicstus(void)
{
	SHELLEXECUTEINFO ShellInfo;

	memset(&ShellInfo, 0, sizeof(ShellInfo));
	ShellInfo.cbSize = sizeof(ShellInfo);
	ShellInfo.hwnd = NULL;
	ShellInfo.lpVerb = "open";
	ShellInfo.lpFile = "sicstus.bat";
	ShellInfo.nShow = SW_HIDE;
	//ShellInfo.nShow = SW_NORMAL;
	ShellInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShellExecuteEx(&ShellInfo);
}
/**********************************************************/


/******************************************************/
/*     Cria um socket para comunicacao com prolog     */
/******************************************************/
void socket_create(void)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	SOCKADDR_IN saServer;
	LPHOSTENT lpHostEnt;

	wVersionRequested = MAKEWORD(1,1);
	WSAStartup(wVersionRequested, &wsaData);
	lpHostEnt = gethostbyname(HOST);

	clientSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(clientSocket == INVALID_SOCKET)
	{
		printf("O programa nao consegue iniciar o SOCKET para ligacao!\n");
		printf("A sair...");
		exit(2);
	}

	saServer.sin_family = AF_INET;
	saServer.sin_port = htons(PORT);
	saServer.sin_addr = *((LPIN_ADDR)*lpHostEnt->h_addr_list);

	if((connect(clientSocket,(PSOCKADDR) &saServer,sizeof(saServer))) != 0)
	{
		printf("O programa nao consegue fazer a ligacao com o servidor!\n");
		printf("A sair...");
		exit(2);
	}
}
/******************************************************/


/*********************************************************/
/*     Envia uma mensagem pelo socket para o SICStus     */
/*********************************************************/
bool socket_send(char *msg)
{
	int ret_value = send(clientSocket,msg,strlen(msg) + 1,0);

	return true;
}
/*********************************************************/


/******************************************************/
/*     Recebe uma mensagem do SICStus pelo socket     */
/******************************************************/
char *socket_receive(void)
{
	memset(msg,0,sizeof(msg));
	int ret_value = recv(clientSocket,msg,sizeof(msg),0);

	return msg;
}
/******************************************************/


/************************************************************/
/*     Fecha o socket e termina a ligaçao com o SICStus     */
/************************************************************/
void socket_close(void)
{
	closesocket(clientSocket);
	WSACleanup();
}
/************************************************************/


/*******************************************************************************/
/*     Verifica se um movimento para a linha line e coluna column é valido     */
/*******************************************************************************/
bool valid_move(int line1, int column1, int line2, int column2)
{
	char *msg;
	char send_msg[256];
	
	sprintf(send_msg,"valid_move(%d,%d,%d,%d).",line1,column1,line2,column2);
	socket_send(send_msg);

	msg = socket_receive();
	printf("Casa %d - %d livre? %s\n",line2,column2,msg);

	if(strcmp(msg,"false") == 0)
		return false;
	else
		return true;
}
/*******************************************************************************/


/************************************************************************/
/*     Executa um movimento de [line1,column1] para [line2,column2]     */
/************************************************************************/
void execute_move(int line1, int column1, int line2, int column2)
{
	char *msg;
	char send_msg[256];
	
	sprintf(send_msg,"execute_move(%d,%d,%d,%d).",line1,column1,line2,column2);
	socket_send(send_msg);

	msg = socket_receive();
	printf("Movimento efectuado [%d:%d - %d:%d]? %s\n",line1,column1,line2,column2,msg);
}
/************************************************************************/


/********************************************************/
/*     Devolve o ultimo movimento efectuado no jogo     */
/********************************************************/
move get_last_move(void)
{
	char *msg;
	char send_msg[256];
	move last_move;
	
	sprintf(send_msg,"get_last_move.");
	socket_send(send_msg);

	msg = socket_receive();
	last_move.line1 = ((int) msg[0]) - 48;
	msg = socket_receive();
	if(strlen(msg) == 1)
		last_move.column1 = ((int) msg[0]) - 48;
	else
		last_move.column1 = ((((int) msg[0]) - 48) * 10) + (((int) msg[1]) - 48);
	msg = socket_receive();
	last_move.line2 = ((int) msg[0]) - 48;
	msg = socket_receive();
	if(strlen(msg) == 1)
		last_move.column2 = ((int) msg[0]) - 48;
	else
		last_move.column2 = ((((int) msg[0]) - 48) * 10) + (((int) msg[1]) - 48);
	
	printf("Movimento recebido: [%d:%d - %d:%d]\n",last_move.line1,last_move.column1,last_move.line2,last_move.column2);

	return last_move;
}
/********************************************************/


/**********************************************************/
/*     Devolve o movimento a efectuar pelo computador     */
/**********************************************************/
move select_computer_move(void)
{
	char *msg;
	char send_msg[256];
	move computer_move;

	sprintf(send_msg,"select_computer_move.");
	socket_send(send_msg);

	msg = socket_receive();
	computer_move.line1 = ((int) msg[0]) - 48;
	msg = socket_receive();
	if(strlen(msg) == 1)
		computer_move.column1 = ((int) msg[0]) - 48;
	else
		computer_move.column1 = ((((int) msg[0]) - 48) * 10) + (((int) msg[1]) - 48);
	msg = socket_receive();
	computer_move.line2 = ((int) msg[0]) - 48;
	msg = socket_receive();
	if(strlen(msg) == 1)
		computer_move.column2 = ((int) msg[0]) - 48;
	else
		computer_move.column2 = ((((int) msg[0]) - 48) * 10) + (((int) msg[1]) - 48);
	
	printf("Movimento recebido: [%d:%d - %d:%d]\n",computer_move.line1,computer_move.column1,computer_move.line2,computer_move.column2);

	return computer_move;
}
/**********************************************************/


/***************************************/
/*     Verifica se o jogo terminou     */
/***************************************/
bool end_test(void)
{
	char *msg;
	char send_msg[256];
	
	sprintf(send_msg,"end_test.");
	socket_send(send_msg);

	msg = socket_receive();
	printf("Fim do jogo? %s\n",msg);

	if(strcmp(msg,"false") == 0)
		return false;
	else
		return true;
}
/***************************************/


/******************************************************/
/*     Define qual o nivel de dificuldade do jogo     */
/******************************************************/
void set_level(int level)
{
	char *msg;
	char send_msg[256];
	
	sprintf(send_msg,"set_level(%d).",level);
	socket_send(send_msg);

	msg = socket_receive();

	printf("Nivel modificado? %s\n",msg);
}
/******************************************************/


/**************************************/
/*     Define qual o tipo de jogo     */
/**************************************/
void set_game_type(int game_type)
{
	char *msg;
	char send_msg[256];

	sprintf(send_msg,"set_game_type(%d).",game_type);
	socket_send(send_msg);

	msg = socket_receive();

	printf("Tipo de jogo modificado? %s\n",msg);
}
/**************************************/


/********************************************************/
/*     Devolve o numero de movimentos ja realizados     */
/********************************************************/
int get_num_moves(void)
{
	char *msg;
	char send_msg[256];

	sprintf(send_msg,"get_num_moves.");
	socket_send(send_msg);

	msg = socket_receive();
	int num_moves = ((int) msg[0]) - 48;

	printf("Numero de movimentos na lista: %s\n",msg);

	return num_moves;
}
/********************************************************/


/**********************************************************/
/*     Devolve o jogador que esta a jogar actualmente     */
/**********************************************************/
int get_player(void)
{
	char *msg;
	char send_msg[256];

	sprintf(send_msg,"get_player.");
	socket_send(send_msg);

	msg = socket_receive();
	int player = ((int) msg[0]) - 48;

	printf("Jogador: %s\n",msg);

	return player;
}
/**********************************************************/


/******************************************/
/*     Define o jogador que ira jogar     */
/******************************************/
void set_player(int player)
{
	char *msg;
	char send_msg[256];

	sprintf(send_msg,"set_player(%d).",player);
	socket_send(send_msg);

	msg = socket_receive();

	printf("Jogador modificado? %s\n",msg);
}
/******************************************/


/*************************************/
/*     Devolve o tipo de jogador     */
/*************************************/
int get_player_type(void)
{
	char *msg;
	char send_msg[256];

	sprintf(send_msg,"get_player_type.");
	socket_send(send_msg);

	msg = socket_receive();
	int player_type = ((int) msg[0]) - 48;

	printf("Tipo de jogador: %s\n",msg);

	return player_type;
}
/*************************************/


/**************************************************/
/*     Verifica se casa [line,column] é preta     */
/**************************************************/
bool black_house(int line, int column)
{
	char *msg;
	char send_msg[256];
	
	sprintf(send_msg,"black_house(%d,%d).",line,column);
	socket_send(send_msg);

	msg = socket_receive();
	printf("Casa Preta? %s\n",msg);

	if(strcmp(msg,"false") == 0)
		return false;
	else
		return true;
}
/**************************************************/
