
/**********************************************************/
/*     Inicializacoes de modulos e tipos de variaveis     */
/**********************************************************/
:- use_module(library(sockets)).
:- use_module(library(lists)).
:- use_module(library(random)).
:- dynamic(logic_tab/1).
:- dynamic(moves_list/1).
:- dynamic(level/1).
:- dynamic(game_type/1).
:- dynamic(player/1).
/**********************************************************/


/***************************************************/
/*     Tabuleiro e lista de movimentos do jogo     */
/***************************************************/
logic_tab([[-1,-1, 2, 3, 0, 1, 0, 2, 3,-1,-1],
     	   [-1, 0, 1, 0, 1, 0, 1, 0, 1, 0,-1],
     	   [ 3, 1, 0, 1, 0, 1, 0, 1, 0, 1, 2],
     	   [ 2, 0, 1, 0, 1, 0, 1, 0, 1, 0, 3],
	   [-1, 1, 0, 1, 0, 1, 0, 1, 0, 1,-1],
     	   [-1,-1, 3, 2, 1, 0, 1, 3, 2,-1,-1]]).

moves_list([]).
level(0).
game_type(0).
player(0).
/***************************************************/


/****************************************************/
/*     Funcoes que comunicam com o visualizador     */
/****************************************************/
server(Port) :-
	assert(logic_tab(_)),
	assert(moves_list(_)),
	assert(level(_)),
	assert(player(_)),
	assert(game_type(_)),
	current_host(Host),
	server(Host,Port).

server(Host,Port) :-
	socket('AF_INET',Socket),
	socket_bind(Socket,'AF_INET'(Host,Port)),
	socket_listen(Socket,5),
	socket_accept(Socket,_Client,Stream),
	write('Cliente ligado'), nl,
	server_loop(Stream),
	socket_close(Socket),
	nl, write('Exit server'),
	halt.

server_loop(Stream) :-
	repeat,
	read(Stream,ClientRequest),
	format('Server received: ~q~n',[ClientRequest]),
	server_input(ClientRequest,ServerReply),
	server_reply(ServerReply,Stream),
	ServerReply == client_shutdown, !.

% Termina o servidor
server_input(ClientRequest,ServerReply) :-
	ClientRequest = end_of_file, !,
	ServerReply = client_shutdown.
server_input(ClientRequest,ServerReply) :-
	ClientRequest = client_shutdown, !,
	ServerReply = thread_shutdown.

% Devolve um movimento
server_input(ClientRequest,ServerReply) :-
	(ClientRequest = get_last_move ; ClientRequest = select_computer_move),
	ServerReply = ClientRequest, !.

% Devolve um valor
server_input(ClientRequest,ServerReply) :-
	(ClientRequest = get_num_moves, get_num_moves(Value)
	;
	ClientRequest = get_player, get_player(Value)
	;
	ClientRequest = get_level, get_level(Value)
	; 
	ClientRequest = get_player_type, get_player_type(Value)),
	ServerReply = Value, !.

% Devolve true or false
server_input(ClientRequest,ServerReply) :-
	(ClientRequest, ServerReply = true
	;
	ServerReply = false), !.

% Devolve erro de pedido do visualizador
server_input(ClientRequest,ServerReply) :-
	ServerReply = unknown_request(ClientRequest),
	format('Unknown client request: ~w~n',[ClientRequest]).

server_reply(client_shutdown,_ServerOut) :- write('Server: end of file.'), !.
server_reply(get_last_move,ServerOut) :- get_last_move(ServerOut), !.
server_reply(select_computer_move,ServerOut) :- select_computer_move(ServerOut), !.
server_reply(ServerReply,ServerOut) :-
	format(ServerOut,'~q',[ServerReply]),
	flush_output(ServerOut), !.

server_reply_move([H|_],ServerOut) :-
	get_list(0,X1,H), get_list(1,Y1,H), get_list(2,X2,H), get_list(3,Y2,H),
	waitA(1000), write('A enviar dados...'), nl,
	format(ServerOut,'~q',[X1]), flush_output(ServerOut),
	format(ServerOut,'~q',[Y1]), flush_output(ServerOut),
	format(ServerOut,'~q',[X2]), flush_output(ServerOut),
	format(ServerOut,'~q',[Y2]), flush_output(ServerOut).
/****************************************************/

waitA(0).
waitA(N) :-
	write(N), nl,
	N2 is N - 1,
	waitA(N2).

/***********************************************/
/*     Predicados chamados do visualizador     */
/***********************************************/
valid_move(X1,Y1,X2,Y2) :-
	retract(player(Player)),
	abolish(player),
	assert(player(Player)),
	retract(logic_tab(LogicTab)),
	abolish(logic_tab),
	assert(logic_tab(LogicTab)),
	get_element(X1,Y1,E1,LogicTab),
	get_element(X2,Y2,E2,LogicTab),
	(Player == 1, E1 == 2 ; Player == 2, E1 == 3),
	(E2 == 0 ; E2 == 1).

execute_move(Move) :-
	get_list(0,X1,Move), get_list(1,Y1,Move), get_list(2,X2,Move), get_list(3,Y2,Move),
	execute_move(X1,Y1,X2,Y2).

execute_move(X1,Y1,X2,Y2) :-
	retract(logic_tab(LogicTab)),
	abolish(logic_tab),
	update_tab(X1,Y1,X2,Y2,LogicTab,NLogicTab),
	assert(logic_tab(NLogicTab)),
	add_move(X1,Y1,X2,Y2),
	retract(player(Player)),
	abolish(player),
	change_player(Player,NPlayer),
	assert(player(NPlayer)).

add_move(X1,Y1,X2,Y2) :-
	retract(moves_list(MovesList)),
	abolish(moves_list),
	append([[X1,Y1,X2,Y2]],MovesList,NMovesList),
	assert(moves_list(NMovesList)).

get_last_move(ServerOut) :-
	retract(moves_list(MovesList)),
	abolish(moves_list),
	server_reply_move(MovesList,ServerOut),
	remove_last_move(MovesList).

remove_last_move([H|T]) :-
	get_list(0,X1,H), get_list(1,Y1,H), get_list(2,X2,H), get_list(3,Y2,H),
	retract(logic_tab(LogicTab)),
	abolish(logic_tab),
	update_tab(X2,Y2,X1,Y1,LogicTab,NLogicTab),
	write(NLogicTab), nl,
	assert(logic_tab(NLogicTab)),
	retract(player(Player)),
	abolish(player),
	change_player(Player,NPlayer),
	assert(player(NPlayer)),
	assert(moves_list(T)).

get_num_moves(NumMoves) :-
	retract(moves_list(MovesList)),
	abolish(moves_list),
	length(MovesList,NumMoves),
	assert(moves_list(MovesList)).

set_level(Level) :-
	abolish(level),
	assert(level(Level)).

set_game_type(GameType) :-
	abolish(game_type),
	assert(game_type(GameType)).

get_player(Player) :-
	retract(player(Player)),
	abolish(player),
	assert(player(Player)).

set_player(Player) :-
	abolish(player),
	assert(player(Player)).

get_player_type(PlayerType) :-
	retract(player(Player)),
	abolish(player),
	assert(player(Player)),
	retract(game_type(GameType)),
	abolish(game_type),
	assert(game_type(GameType)),
	type(GameType,Player,PlayerType).

select_computer_move(ServerOut) :-
	retract(player(Player)),
	abolish(player),
	assert(player(Player)),
	retract(logic_tab(LogicTab)),
	abolish(logic_tab),
	assert(logic_tab(LogicTab)),
	select_move(Player,LogicTab,Move),
	execute_move(Move),
	retract(moves_list(NMovesList)),
	abolish(moves_list),
	assert(moves_list(NMovesList)),
	server_reply_move(NMovesList,ServerOut).

end_test :-
	retract(logic_tab(LogicTab)),
	abolish(logic_tab),
	end_test_aux(0,LogicTab,LogicTab,A),
	assert(logic_tab(LogicTab)),
	A == 1.
/***********************************************/

change_player(1,2).
change_player(2,1).

/***********************************
      Devolve o tipo de jogador
 ***********************************/
type(1,1,TJ) :- TJ is 0.
type(1,2,TJ) :- TJ is 1.
type(2,1,TJ) :- TJ is 0.
type(2,2,TJ) :- TJ is 0.
type(3,1,TJ) :- TJ is 1.
type(3,2,TJ) :- TJ is 1.
/***********************************/


/******************************************************/
/*     Actualiza a posiçao das peças no tabuleiro     */
/******************************************************/
update_tab(X1,Y1,X2,Y2,LogicTab,NLogicTab) :-
	get_element(X1,Y1,N1,LogicTab),
	get_element(X2,Y2,N2,LogicTab),
	(vizinho(X1,Y1,X2,Y2,_), update_tab_aux(X1,Y1,N2,LogicTab,NewLogicTab)
	;
	set_element(X1,Y1,N2,LogicTab,NewLogicTab)),
	set_element(X2,Y2,N1,NewLogicTab,NLogicTab).

update_tab_aux(X1,Y1,N2,LogicTab,NLogicTab) :-
	(N2 == 0, A is 1
	;
	N2 == 1, A is 0),
	set_element(X1,Y1,A,LogicTab,NLogicTab).
/******************************************************/


/**********************************/
/*     Teste de final do jogo     */
/**********************************/
end_test(LogicBoard) :-
	end_test_aux(0,LogicBoard,LogicBoard,A),
	A == 1.

end_test_aux(_,[],_,_).
end_test_aux(X,[Linha|Resto],LogicTab,A) :-
	end_test_aux_line(X,0,Linha,LogicTab,A),
	X2 is X + 1,
	end_test_aux(X2,Resto,LogicTab,A).

end_test_aux_line(_,_,[],_,_).
end_test_aux_line(X,Y,[_|Resto],LogicTab,A) :-
	get_element(X,Y,E,LogicTab),
	((E == 2 ; E == 3), get_moves(X,Y,LogicTab,L), L == [], A is 1
	;
	true),
	Y2 is Y + 1,
	end_test_aux_line(X,Y2,Resto,LogicTab,A).
/**********************************/


/*********************************************************/
/*     Determina a jogada a efectuar pelo computador     */
/*********************************************************/
select_move(J,LogicBoard,Move) :-
	retract(level(Level)),
	abolish(level),
	assert(level(Level)),
	get_pieces_moves(J,0,LogicBoard,LogicBoard,[],LM),
	computer_move(Level,J,LogicBoard,LM,Move).

computer_move(1,_,_,LM,Move) :-
	length(LM,L),
	Len is L - 1,
	random(0,Len,N),
	get_list(N,Move1,LM),
	append([],Move1,Move).

computer_move(2,J,LogicBoard,LM,Move) :- 
	get_best_move(J,LogicBoard,LM,Move1,0),
	append([],Move1,Move).

computer_move(3,J,LogicBoard,LM,Move) :- 
	best(J,3,3,LogicBoard,LM,Move1,Val),
	append([],Move1,Move).

computer_move(4,J,LogicBoard,LM,Move) :- 
	best(J,3,4,LogicBoard,LM,Move1,Val),
	append([],Move1,Move).

computer_move(5,J,LogicBoard,LM,Move) :-
	best(J,3,5,LogicBoard,LM,Move1,Val),
	append([],Move1,Move).
/*
select_move(J,LogicBoard,Move,MovesList) :-
	retract(level(Level)),
	abolish(level),
	assert(level(Level)),
	get_pieces_moves(J,0,LogicBoard,LogicBoard,[],LM),
	computer_move(Level,J,LogicBoard,LM,Move,MovesList).

computer_move(1,_,_,LM,Move,MovesList) :-
	get_list(0,Move1,LM),
	(equal_move(MovesList,Move1), get_list(1,Move,LM)
	;
	append([],Move1,Move)).

computer_move(2,J,LogicBoard,LM,Move,MovesList) :- 
	get_best_move(J,LogicBoard,LM,Move1,0),
	(equal_move(MovesList,Move1), takeout(Move1,LM,LM2), get_best_move(J,LogicBoard,LM2,Move,0)
	;
	append([],Move1,Move)).

computer_move(3,J,LogicBoard,LM,Move,MovesList) :- 
	best(J,3,3,LogicBoard,LM,Move1,Val),
	(equal_move(MovesList,Move1), takeout(Move1,LM,LM2), best(J,3,3,LogicBoard,LM2,Move,Val)
	;
	append([],Move1,Move)).

computer_move(4,J,LogicBoard,LM,Move,MovesList) :- 
	best(J,3,4,LogicBoard,LM,Move1,Val),
	(equal_move(MovesList,Move1), takeout(Move1,LM,LM2), best(J,3,4,LogicBoard,LM2,Move,Val)
	;
	append([],Move1,Move)).

computer_move(5,J,LogicBoard,LM,Move,MovesList) :-
	best(J,3,5,LogicBoard,LM,Move1,Val),
	(equal_move(MovesList,Move1), takeout(Move1,LM,LM2), best(J,3,5,LogicBoard,LM2,Move,Val)
	;
	append([],Move1,Move)).*/
/*********************************************************/


/************************************************************************
       Verifica se o último movimento do jogador é igual ao actual
 ************************************************************************/
equal_move(MovesList,Move) :-
	length(MovesList,N), N >= 2,
	get_list(1,LastMove,MovesList),
	reverse_move(LastMove,LastMoveInv), LastMoveInv == Move.

reverse_move(Move,MoveInv) :-
	get_list(0,X1,Move), get_list(1,Y1,Move), get_list(2,X2,Move), get_list(3,Y2,Move),
	append([X2,Y2],[X1,Y1],MoveInv).
/************************************************************************/


/************************************************************/
/*     Devolve o movimento no qual o tabuleiro é melhor     */
/************************************************************/
get_best_move(_,_,[],_,_).
get_best_move(J,LB,[H|T],Move,V) :-
	get_list(0,X1,H), get_list(1,Y1,H), get_list(2,X2,H), get_list(3,Y2,H),
	update_tab(X1,Y1,X2,Y2,LB,NLB),
	evaluate_pieces(J,NLB,V1),
	(V1 > V, V2 is V1, append([],H,Move)
	;
	V2 is V),
	get_best_move(J,LB,T,Move,V2).
/************************************************************/


/*************************************************************/
/*     Devolve os movimentos possiveis de todas as peças     */
/*************************************************************/
get_pieces_moves(_,_,[],_,L,LM) :- append([],L,LM).
get_pieces_moves(J,X,[H|T],LogicBoard,L,LM) :-
	get_pieces_moves_aux(J,X,0,H,LogicBoard,[],NL),
	append(L,NL,L2),
	X2 is X + 1,
	get_pieces_moves(J,X2,T,LogicBoard,L2,LM).

get_pieces_moves_aux(_,_,_,[],_,L,NL):- append([],L,NL).
get_pieces_moves_aux(J,X,Y,[H|T],LogicBoard,L,NL):-
	Y2 is Y + 1,
	(tab_in(X,Y), black_house(X,Y), get_moves(X,Y,LogicBoard,L1)
	;
	tab_in(X,Y), white_house(X,Y), get_moves_compose(X,Y,LogicBoard,L1)
	;
	true),
	length(L1,N),
	(J == 1, H == 2, add_xy(N,L1,X,Y,L,L2), get_pieces_moves_aux(J,X,Y2,T,LogicBoard,L2,NL)
	;
	J == 2, H == 3, add_xy(N,L1,X,Y,L,L2), get_pieces_moves_aux(J,X,Y2,T,LogicBoard,L2,NL)
	;
	get_pieces_moves_aux(J,X,Y2,T,LogicBoard,L,NL)).
/*************************************************************/


/****************************************************/
/*     Avaliação de todas as peças do jogador J     */
/****************************************************/
evaluate_pieces(J,LogicBoard,V) :-
	(J == 1, A is 2 
	;
	J == 2, A is 3),
	findall(Val,(member_board(A,X,Y,LogicBoard),evaluate_piece(X,Y,LogicBoard,Val)),L),
	sum_list(L,V).

evaluate_piece(X,Y,LogicBoard,100) :-
	num_pieces_arround(X,Y,N,LogicBoard),
	N == 2.
evaluate_piece(X,Y,LogicBoard,100) :-
	border(X,Y),
	num_pieces_arround(X,Y,N,LogicBoard),
	N == 1.
evaluate_piece(X,Y,LogicBoard,50) :-
	num_pieces_arround(X,Y,N,LogicBoard),
	N == 1.
evaluate_piece(X,Y,_,50) :-
	border(X,Y).
evaluate_piece(_,_,_,0).
/****************************************************/


/*********************************************************/
/*     Avaliação de todos os movimentos do jogador J     */
/*********************************************************/
evaluate_moves(J,LogicBoard,V) :-
	get_pieces_moves(J,0,LogicBoard,LogicBoard,[],LM),
	evaluate_moves_aux(J,LM,LogicBoard,V).

evaluate_moves_aux(_,[],_,I) :- I is 0.
evaluate_moves_aux(J,[H|T],LB,I) :-
	get_list(0,X1,H), get_list(1,Y1,H), get_list(2,X2,H), get_list(3,Y2,H),
	update_tab(X1,Y1,X2,Y2,LB,NLB),
	(end_test(NLB), I is 1000
	;
	evaluate_moves_aux(J,T,LB,I)).
/*********************************************************/


/**********************************/
/*     Avaliação do tabuleiro     */
/**********************************/
evaluate_tab(J,LogicBoard,N) :-
	change_player(J,J2),
	evaluate_pieces(J,LogicBoard,V1),
	evaluate_moves(J2,LogicBoard,V2),
	evaluate_pieces(J2,LogicBoard,V3),
	evaluate_moves(J,LogicBoard,V4),
	(end_test(LogicBoard), V5 = 10000 ; V5 = 0), !,
	N is (10 * V1) - V2 - (10 * V3) + V4 + V5.
/**********************************/


/**************************************************************************/
/*     Devolve o numero de peças que se encontram a volta de uma peça     */
/**************************************************************************/
num_pieces_arround(X,Y,N,LogicBoard) :-
	get_moves(X,Y,LogicBoard,L),
	length(L,LT),
	(border(X,Y), N is 2 - LT
	;
	N is 3 - LT).
/**************************************************************************/


/***********************************************************************/
/*     Devolve os movimentos possiveis a partir de uma localizaçao     */
/***********************************************************************/
get_moves_compose(X,Y,LogicTab,L) :-
	get_moves(X,Y,LogicTab,L1),
	get_moves_compose_aux(X,Y,L1,LogicTab,[],L2),
	append(L1,L2,L).

get_moves_compose_aux(_,_,[],_,LA,L) :- append([],LA,L).
get_moves_compose_aux(X,Y,[H|T],LogicTab,LA,L) :-
	get_list(0,X1,H), get_list(1,Y1,H),
	update_tab(X,Y,X1,Y1,LogicTab,LB),
	get_moves(X1,Y1,LB,L1),
	takeout([X,Y],L1,L3),
	append(LA,L3,L2),
	get_moves_compose_aux(X,Y,T,LogicTab,L2,L).

get_moves(X,Y,LogicTab,L) :-
	findall([X2,Y2],possible_moves(X,Y,X2,Y2,LogicTab),L).

possible_moves(X1,Y1,X2,Y2,LogicTab) :-
	X2 is X1 - 1,
	Y2 is Y1,
	tab_in(X2,Y2),
	check_move(X1,Y1,X2,Y2,LogicTab).

possible_moves(X1,Y1,X2,Y2,LogicTab) :-
	X2 is X1 + 1,
	Y2 is Y1,
	tab_in(X2,Y2),
	check_move(X1,Y1,X2,Y2,LogicTab).

possible_moves(X1,Y1,X2,Y2,LogicTab) :-
	X2 is X1,
	Y2 is Y1 - 1,
	tab_in(X2,Y2),
	check_move(X1,Y1,X2,Y2,LogicTab).

possible_moves(X1,Y1,X2,Y2,LogicTab) :-
	X2 is X1,
	Y2 is Y1 + 1,
	tab_in(X2,Y2),
	check_move(X1,Y1,X2,Y2,LogicTab).
/***********************************************************************/


/*********************************************/
/*     Verifica se um movimento é valido     */
/*********************************************/
check_move(X1,Y1,X2,Y2,LogicTab) :-
	check_move_aux(X1,Y1,X2,Y2),
	free_house(X2,Y2,LogicTab), !.

check_move_aux(X1,Y1,X2,Y2) :-
	black_house(X1,Y1),
	vizinho(Y1,X1,Y2,X2,1).

check_move_aux(X1,Y1,X2,Y2) :-
	white_house(X1,Y1),
	vizinho(Y1,X1,Y2,X2,2).
/*********************************************/


/*************************************/
/*     Verificaçoes no tabuleiro     */
/*************************************/
tab_in(X,Y) :- X == 0, Y > 1, Y < 9.
tab_in(X,Y) :- X == 1, Y > 0, Y < 10.
tab_in(X,Y) :- X == 4, Y > 0, Y < 10.
tab_in(X,Y) :- X == 5, Y > 1, Y < 9.
tab_in(X,Y) :- (X == 2 ; X == 3), Y >= 0, Y < 11.

member_board(Peca,X,Y,Board):-
	member_pos_lista(Linha,Y,Board),
	member_pos_lista(Peca,X,Linha).

member_pos_lista(Membro,N,Lista):-
	member_pos_procura(Membro,0,N,Lista).

member_pos_procura(Membro,N,N,[Membro|_]).
member_pos_procura(Membro,P,N,[_|T]):-
	P2 is P + 1,
	member_pos_procura(Membro,P2,N,T).

free_house(0).
free_house(1).
free_house(X,Y,LogicTab) :-
	get_element(X,Y,N,LogicTab),
	free_house(N).

black_house(X,Y) :-
	(par(X), par(Y)
	;
	\+ par(X), \+par(Y)).

white_house(X,Y) :-
	(par(X), \+ par(Y)
	;
	\+ par(X), par(Y)).

dark_piece(2).
dark_piece(X,Y,LogicTab) :-
	get_element(X,Y,N,LogicTab),
	dark_piece(N).

light_piece(3).
light_piece(X,Y,LogicTab) :-
	get_element(X,Y,N,LogicTab),
	light_piece(N).

vizinho(X1,Y1,X2,Y2,1) :-
	Y1 == Y2,
	(X is X2 - 1, X1 == X
	;
	X is X2 + 1, X1 == X).

vizinho(X1,Y1,X2,Y2,1) :-
	X1 == X2,
	Y is Y2 - 1,
	Y1 == Y.

vizinho(X1,Y1,X2,Y2,2) :-
	Y1 == Y2,
	(X is X2 - 1, X1 == X
	;
	X is X2 + 1, X1 == X).

vizinho(X1,Y1,X2,Y2,2) :-
	X1 == X2,
	Y is Y2 + 1,
	Y1 == Y.

border(_,Y) :- Y == 0.
border(_,Y) :- Y == 10.
border(X,Y) :- 
	X == 0,
	Y \== 4,
	Y \== 6.
border(X,Y) :- 
	X == 5,
	Y \== 4,
	Y \== 6.
border(X,Y) :-
	Y == 1,
	(X == 1 ; X == 4).
border(X,Y) :-
	Y == 9,
	(X == 1 ; X == 4).
/*************************************/


/************************************************/
/*      Predicados que trabalham com listas     */
/************************************************/
takeout(X,[X|R],R).
takeout(X,[F|R],[F|S]) :- takeout(X,R,S).

set_element(X,Y,E,B,NB) :-
	get_list(X,L,B),
	set_list(Y,E,L,NL),
	set_list(X,NL,B,NB).

set_list(N,E,L,NL) :- set_list_aux(N,0,E,L,NL).

set_list_aux(_,_,_,[],[]).
set_list_aux(N,N,E,[_|T],[E|L]) :-
	!,NewN is N + 1,
	set_list_aux(N,NewN,E,T,L).
set_list_aux(N,I,E,[H|T],[H|L]) :-
	NewN is I + 1,
	set_list_aux(N,NewN,E,T,L).

set_list_jogo(X,Y,E,B,NB) :-
	get_lista(X,L,B),
	set_list(Y,E,L,NL),
	set_list(X,NL,B,NB).

get_element(X,Y,E,L) :-
	get_list(X,A,L),
	get_list(Y,E,A).

get_list(N,E,L) :- get_list_aux(N,0,E,L).

get_list_aux(_,_,_,[]).
get_list_aux(N,N,E,[E|T]) :-
	!,NewN is N + 1,
	get_list_aux(N,NewN,E,T).
get_list_aux(N,NC,E,[_|T]) :-
	NewN is NC + 1,
	get_list_aux(N,NewN,E,T).

add_xy(0,[],_,_,L,L2) :- append([],L,L2).
add_xy(1,[H|T],X,Y,L,L2) :-
	append([X,Y],[],L1),
	append([],T,L3),
	append(H,L3,L4),
	append(L1,L4,NL),
	append([NL],L,L2).

add_xy(N,[H|T],X,Y,L,L2) :-
	add_xy(1,[H],X,Y,L,L1),
	N2 is N - 1,
	add_xy(N2,T,X,Y,L1,L2).

build_list([H|T],H1,T1) :-
	H is H1,
	T is T1.
/************************************************/


/**************************************************************
                  Implementaçao do minimax
 **************************************************************/
best(J,Depth,MaxDepth,LB,[Move],Move,Val) :-
	get_list(0,X1,Move),get_list(1,Y1,Move),get_list(2,X2,Move),get_list(3,Y2,Move),
	update_tab(X1,Y1,X2,Y2,LB,NLB),
	minimax(J,Depth,MaxDepth,NLB,_,Val), !.

best(J,Depth,MaxDepth,LB,[Move1|LM],BestMove,BestVal) :-
	get_list(0,X1,Move1),get_list(1,Y1,Move1),get_list(2,X2,Move1),get_list(3,Y2,Move1),
	update_tab(X1,Y1,X2,Y2,LB,NLB),
	minimax(J,Depth,MaxDepth,NLB,_,Val1), !,
	best(J,Depth,MaxDepth,LB,LM,Move2,Val2),
	betterof(J,Move1,Val1,Move2,Val2,BestMove,BestVal).

betterof(J,Move1,Val1,Move2,Val2,Move1,Val1) :-
	(J == 2, Val1 > Val2, !
	;
	J == 1, Val1 < Val2, !).
betterof(J,Move1,Val1,Move2,Val2,Move2,Val2).

minimax(J,Depth,MaxDepth,LB,BestMove,Val) :-
	Depth < MaxDepth,
	change_player(J,J2),
	Depth2 is Depth + 1,
	get_pieces_moves(J2,0,LB,LB,[],LM),
	LM \== [],
	best(J2,Depth2,MaxDepth,LB,LM,BestMove,Val).

minimax(J,_,_,LB,_,Val) :-
	evaluate_tab(J,LB,Val).
/**************************************************************/


/*********************************************
            Predicados auxiliares
 *********************************************/
par(N) :-
	N1 is N rem 2,
	N1 == 0.
/*********************************************/
