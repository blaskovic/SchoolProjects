% delka linearniho seznamu
delka([],0).
delka([_|T],S) :- delka(T,SS), S is SS + 1.

% je prvek clenem lin. seznamu?
jePrvek([X|_],X).
jePrvek([_|T],X) :- jePrvek(T,X). 

% spojeni dvou linearnich seznamu
spoj([],L,L).
spoj([H|T],L,[H|TT]) :- spoj(T,L,TT).


% doplnte nasledujici predikaty:

% Vytvorte funkci pro rozdeleni linearniho seznamu na poloviny
half(INPUT1,INPUT2,HALF1,HALF2) :- delka(INPUT1,LEN1), delka(INPUT2,LEN2), LEN1 >= LEN2, HALF1 = INPUT1, HALF2 = INPUT2,!.
half(INPUT1,[TRANSFER|RIGHT],HALF1,HALF2) :- spoj(INPUT1, [TRANSFER], LEFT), half(LEFT, RIGHT, HALF1, HALF2).
divide_half(INPUT,HALF1,HALF2) :- half([], INPUT, HALF1, HALF2).

rem(IN1, IN2, _, OUT) :- delka(IN2, L1), L1 == 0, OUT = IN1, !.
rem(IN1, [TRANSFER|RIGHT], ITEM, OUT) :- ITEM == TRANSFER,  rem(IN1, RIGHT, ITEM, OUT), !.
rem(IN1, [TRANSFER|RIGHT], ITEM, OUT) :- is_list(TRANSFER), rem([], TRANSFER, ITEM, OUTTMP), spoj(IN1, [OUTTMP], JOINED), rem(JOINED, RIGHT, ITEM, OUT), !.
rem(IN1,[TRANSFER|RIGHT], ITEM, OUT) :- spoj(IN1, [TRANSFER], LEFT), rem(LEFT, RIGHT, ITEM, OUT).
remove_item_GEN(INPUT,ITEM,OUTPUT) :- rem([], INPUT, ITEM, OUTPUT).

% Vytvorte funkci pro reverzaci obecneho seznamu
rev(IN1, IN2, OUT) :- delka(IN2, L1), L1 == 0, OUT = IN1, !.
rev(IN1,[TRANSFER|RIGHT],OUT) :- is_list(TRANSFER), rev([], TRANSFER, OUTTMP), spoj([OUTTMP], IN1, JOINED), rev(JOINED, RIGHT, OUT), !.
rev(IN1,[TRANSFER|RIGHT],OUT) :- spoj([TRANSFER], IN1, LEFT), rev(LEFT, RIGHT, OUT).
reverse_GEN(INPUT,OUTPUT) :- rev([], INPUT, OUTPUT).

% Vytvorte funkci pro vlozeni prvku na n-tou pozici linearniho seznamu
ins(IN1, IN2, _, _, OUT, _) :- delka(IN2, L1), L1 == 0, OUT = IN1, !.
ins(_, IN2, POSITION, ITEM, OUT, ACT) :- ACT == 1, delka(IN2, L), L < POSITION, spoj(IN2, [ITEM], OUTTMP), ins(OUTTMP, [], POSITION, ITEM, OUT, ACT).
ins(IN1,[TRANSFER|RIGHT], POSITION, ITEM, OUT, ACT) :- ACT == POSITION, spoj([ITEM], [TRANSFER], JOINED), spoj(IN1, JOINED, LEFT), ACT2 is ACT + 1, ins(LEFT, RIGHT, POSITION, ITEM, OUT, ACT2), !.
ins(IN1,[TRANSFER|RIGHT], POSITION, ITEM, OUT, ACT) :- spoj(IN1, [TRANSFER], LEFT), ACT2 is ACT + 1, ins(LEFT, RIGHT, POSITION, ITEM, OUT, ACT2).
insert_pos(LIST,POSITION,ITEM,OUTPUT) :- ins([], LIST, POSITION, ITEM, OUTPUT, 1).

% Vytvorte funkci pro inkrementaci kazdeho prvku obecneho seznamu o hodnotu hloubky zanoreni prvku
inc(IN1, IN2, OUT, _) :- delka(IN2, L1), L1 == 0, OUT = IN1, !.
inc(IN1,[TRANSFER|RIGHT],OUT, LVL) :- is_list(TRANSFER), LVL2 is LVL+1, inc([], TRANSFER, OUTTMP, LVL2), spoj(IN1, [OUTTMP], JOINED), inc(JOINED, RIGHT, OUT, LVL), !.
inc(IN1,[TRANSFER|RIGHT],OUT, LVL) :- TRANTMP is TRANSFER+LVL, spoj(IN1, [TRANTMP], LEFT), inc(LEFT, RIGHT, OUT, LVL).
increment_general(INPUT,OUTPUT) :- inc([], INPUT, OUTPUT, 1).

