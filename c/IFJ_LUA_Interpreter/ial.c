/**
 * ------------------------------------------
 * Project : Interpreter jazyka IFJ11 
 * Name    : ial
 * File    : ial.c
 * Authors : Karel Hala (xhalak00)
 *         : Pavlina Bartikova (xbarti00)
 *         : Tomas Goldmann (xgoldm03)
 *         : Branislav Blaskovic (xblask00)
 * ------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "types.h"
#include "scaner.h"
#include "ial.h"
#include "interpreter.h"
#include "my_memory.h"

#define ALPHABET 255


//funkce pro praci s BVS
/**
 * Funkce pro prvotni deklaraci stromu
 * @param init Tbin_tree* strom, ktery se bude deklarovat
 * @return odkaz na nove deklarovany strom
 */
Tbin_tree* bvs_init_tree(Tbin_tree *init)
{
	//vytvori misto pro strom a nastavi zbytek hodnot na NULL
	if ((init=my_malloc(sizeof(Tbin_tree)))==NULL)
		return NULL;
	init->value = NULL;
	init->left = NULL;
	init->right = NULL;
	return init;
}

/**
 * Funkce pro vytvoreni podstromu (listu).
 * @param bin_tree Tbin_tree* odkaz na strom co prochazim
 * @param value char* nazev ukladaneho uzlu
 * @return ukazatel na podstrom (nove vytvoreny list)
 */
Tbin_tree* bvs_make_leaf(char* value,Tbin_tree *leaf_)
{
	//pokud nove vytvareny list nema udeleo nisto vrati NULL
	if(leaf_ == NULL)
	{
		return NULL;
	}
	else
	{
		//nastavi hodnoty na ove vytvoreny list (do prava a do leva NULL)
		leaf_->left=NULL;
		leaf_->right=NULL;
		leaf_->value=my_malloc(sizeof(char)*(strlen(value)+1));
		leaf_->function_arg_num = 0;
		leaf_->not_declared = 0;
		leaf_->order_of_var = function_num_of_var;
		strcpy(leaf_->value,value);
		return leaf_;
	}
}

/**
 * Funkce pro vytvoreni stromu.
 * @param bin_tree Tbin_tree* odkaz na strom co prochazim
 * @param value char* nazev ukladaneho uzlu
 * @return ukazatel na strom
 */
Tbin_tree* bvs_make_tree(Tbin_tree* bin_tree,char* var, int not_declared)
{
	Tbin_tree* leaf=NULL;
	
	//nove vytvareny strom
	if (bin_tree->value==NULL){

		leaf=bvs_make_leaf(var,bin_tree);
		bin_tree=leaf;
		bin_tree->not_declared = not_declared;
		return bin_tree;
	}

	//strom jiz ma nejmin jeden uzel
	if (bin_tree->value != NULL && (strcmp (bin_tree->value,var)>0))
	{
		//rekurzivne prochazi nejdrive do leva, potom do prava
		if (bin_tree->left==NULL){
			//pokud je na konci stromu, vytvori novy list a ulozi hodnoty
			leaf=my_malloc(sizeof(Tbin_tree));
			leaf=bvs_make_leaf(var,leaf);
			bin_tree->left=leaf;
			bin_tree->left->not_declared = not_declared;
			return bin_tree;
		}
		else
			bvs_make_tree(bin_tree->left,var, not_declared);
	}
	
	//rekurzivni pruchod do prava
	else if (strcmp (bin_tree->value,var)<0)
	{
		//pokud je na konci stromu, vytvori novy list a ulozi hodnoty
		if (bin_tree->right==NULL){
			leaf=my_malloc(sizeof(Tbin_tree));
			leaf=bvs_make_leaf(var,leaf);
			bin_tree->right=leaf;
			bin_tree->right->not_declared = not_declared;
			return bin_tree;
		}
		else 
			bvs_make_tree(bin_tree->right,var, not_declared);
	}
	
	return bin_tree;
}

/**
 * Funkce hledani ve stromu.
 * @param bin_tree Tbin_tree* odkaz na strom co prochazi
 * @param value char* nazev hledaneho uzlu
 * @return 0=nenalezeno,1=nalezeno,2=nalezene(nedeklarovane)
 */
int bvs_search(Tbin_tree* bin_tree,char* var)
{
	if(bin_tree == NULL) return 0;
	if(var == NULL) return 0;
	
	//pokud je aspon jednouzlovy strom
	if (bin_tree->value != NULL)
	{
		//rekuzivne prochazi nejdrive doleva
		if (strcmp(bin_tree->value,var)>0)
		{
			return bvs_search(bin_tree->left,var);
		}
		//potom doprava
		else if (strcmp(bin_tree->value,var)<0)
		{
			return bvs_search(bin_tree->right,var);
		}
		//a pokud ho najde vrati 1, nebo 2, podle deklarace
		else if (strcmp(bin_tree->value,var)==0)
		{
			if(bin_tree->not_declared == 1) return 2;
			else return 1;
		}
	}
	
	//pokud nic nenajde, vrati 0
	else
	{
		return 0;
	}
	
	return 0;
}

/**
 * Funkce projde rekurzovne strom a pokud najde hledany uzel, vrati jej do dalsi funkce.
 * @param bin_tree Tbin_tree* odkaz na strom co ukladam
 * @return ukazatel binarni strom
 */
Tbin_tree *bvs_get_key(Tbin_tree* bin_tree,char* var)
{
	
	if(bin_tree == NULL) return 0;
	if(var == NULL) return 0;
	
	
	//funkce prochazi rekurzovne strom
	if (bin_tree->value != NULL)
	{
		//nejdrive do leva
		if (strcmp(bin_tree->value,var)>0)
		{
			return bvs_get_key(bin_tree->left,var);
		}
		//potom do prava
		else if (strcmp(bin_tree->value,var)<0)
		{
			return bvs_get_key(bin_tree->right,var);
		}
		//a pokud najde uzel, ktery je hledany
		else if (strcmp(bin_tree->value,var)==0)
		{
			//vrati ukazatel na nej
			return bin_tree;
		}
	}
	//pokud, ale nic nenajde, tak vrati NULL
	else
	{
		return NULL;
	}
	
	return NULL;
}

/**
 * Funkce projde strom a postupne nastavi vsechny konecne uzly na nedeklarovane.
 * @param bin_tree Tbin_tree* odkaz na strom co ukladam
 * @return ukazatel binarni strom
 */
Tbin_tree* bvs_set_declaration(Tbin_tree* bin_tree,char* var, int not_declared)
{
	Tbin_tree *key = bvs_get_key(bin_tree, var);
	
	//pokud funkci prisel ukazatel na strom, nastavi deklaraci na nedeklarovano
	if(key != NULL)
	{
		key->not_declared = not_declared;
	}
	
	//vrati strom, ktery upravovala
	return bin_tree;
}

/**
 * Funkce pro inkrementaci poctu argumentu ve funkci.
 * @param bin_tree Tbin_tree* odkaz na strom co ukladam
 * @return ukazatel na binarni strom
 */
Tbin_tree* bvs_increment_function_arg(Tbin_tree* bin_tree, char* var)
{
	Tbin_tree *key = bvs_get_key(bin_tree, var);
	
	//pokud dostane funkce ukazatel na uzel
	if(key != NULL)
	{
		//zvysi pocet argumentu, ktere funkce obsahuje
		key->function_arg_num += 1;
	}
	
	//vrati strom, ktery upravovala
	return bin_tree;
}

/**
 * Projde strom a vrati argumenty stromu.
 * @param bin_tree Tbin_tree* odkaz na strom, ktery prochazim
 * @param var char* klic, podle ktereho prochazim strom
 * @return adresa
 */
int bvs_get_function_arg(Tbin_tree* bin_tree, char* var)
{
	Tbin_tree *key = bvs_get_key(bin_tree, var);
	
	//pokud dostane funkce ukazatel na uzel
	if(key != NULL)
	{
		//vrati pekne argumenty funkce
		return key->function_arg_num;
	}
	
	//jinak vrati 0
	return 0;
}

/**
 * Projde strom a vrati hondotu adresy instrukce v binarnim strome.
 * @param bin_tree Tbin_tree* odkaz na strom, ktery prochazim
 * @param var char* klic, podle ktereho prochazim strom
 * @return adresa.
 */
TInstruction *bvs_get_instruction_address(Tbin_tree* bin_tree, char* var)
{
	Tbin_tree *key = bvs_get_key(bin_tree, var);
	
	//pokud dostane funkce ukazatel na uzel
	if(key != NULL)
	{
		//vrati adresu instrukce
		return key->instruction;
	}
	
	//jinak vrati NULL
	return NULL;
}

/**
 * Funkce pro kontrolu, zda je funkce deklarovana.
 * @param bin_tree Tbin_tree* odkaz na strom, ktery kotroluju, zda je kompletne prazdny
 * @return cislo chyby
 */
int bvs_is_not_declared(Tbin_tree *bin_tree)
{
	int error = ERR_CODE_OK;
	
	//pokud je funkce na konci stromu rekurze konci
	if (bin_tree==NULL)
		return ERR_CODE_OK;
	
	// nasla sa nedeklarovana
	if(bin_tree->not_declared == 1)
	{
		return ERR_CODE_SEM;
	}
	
	//rekurzovne prochazi nejdrive do leva
	error = bvs_is_not_declared(bin_tree->left);
	
	//pokud dostane z rekurze ze funkce byla deklarovana pokracuje dal, jinak vraci cislo chyby
	if(error != ERR_CODE_OK) return error;
	
	//jeste projde do prava
	error = bvs_is_not_declared(bin_tree->right);
	
	//a nakonec vrati cislo chyby z pruchodu do prava
	return error;
}

/**
 * Funkce pro ukladani hodnot v interpretu.
 * @param bin_tree Tbin_tree* odkaz na strom co ukladam
 * @param var char* hodnota, co se uklada
 * @param values TInstruction* struktura ve ktere je ulozen bud string, nebo cislo
 * @return ukazatel na binarni strom
 */
Tbin_tree* bvs_update_inst_addr(Tbin_tree* bin_tree,char* var, TInstruction* instruction)
{
	Tbin_tree *key = bvs_get_key(bin_tree, var);
	
	//pokud dostane funkce ukazatel na uzel
	if(key != NULL)
	{
		//zmeni hodnotu v instrukcni pasce
		key->instruction = instruction;
	}
	
	//a vrati strom
	return bin_tree;
}

/**
 * Funkce pro pruchod stromem.
 * @param bin_tree Tbin_tree* odkaz na strom co prochazim
 * @return cislo chyby
 */
int bvs_pre_order (Tbin_tree* bin_tree)
{
	if (bin_tree==NULL)
		return 0;
	printf("%s\t\t VAR_ORDER: %d DECL: %d FUNC_NUM_ARG: %d\n", bin_tree->value, bin_tree->order_of_var, bin_tree->not_declared, bin_tree->function_arg_num);
	bvs_pre_order(bin_tree->left);
	bvs_pre_order(bin_tree->right);
	return 0;
}

/**
 * Funkce pro odstraneni stromu.
 * @param tree Tbin_tree* strom ktery chci rusit
 * @return void
 */
void bvs_destroy_tree(Tbin_tree *tree_)
{
	//rekurzovne prochazi strom
	if (tree_!=NULL)
	{
		//nejdrive do leva
		if (tree_->left!=NULL)
			bvs_destroy_tree(tree_->left);
			
		//potom taky do prava
		if (tree_->right!=NULL)
			bvs_destroy_tree(tree_->right);
		
		//pokdu jsme na konci stromu uvolnime jeho hodnoty a rekurze konci na tomhle uzlu
		my_free(tree_->value);
		my_free(tree_);	
	}
}



//funkce pro vyhledavani podretezce v retezci
/**
 * Funkce pro prevod stringu na pole cisel.
 * @param string char* vstupni retezec
 * @return pole cisel
 */
int* convert_to_numbers(char* string)
{
	int* value;
	
	//delka retezce
	int length=strlen(string);
	
	if ((value = my_malloc (sizeof(int)*length))==NULL)
		return (int*)1;

	//jednotlive pismena v reteci ulozim pod jejich ascii hodnotou	
	for (int x=0;x<length;x++)
		value[x] = string[x];
	
	return value;		
}

/**
 * Funkce pro vytvoreni tabulky, pro urceni o kolik ma v dannou chvili skocit.
 * @param string char* retezec, ktery se bude hledat
 * @param char_jump int* tabulka skoku
 * @return int* upravena tabulka skoku
 */
int* t_char_jump (char* string,int* char_jump)
{
	int* value;
	int length=strlen(string);
	value = convert_to_numbers(string);
	
	//od 0 do velikosti nacitane abecedy (mame 255 znaku) ulozim do tabulky postupne maximalni mozny skok
	for (int i=0;i<ALPHABET;i++)
	{
		char_jump[i]=length;
	}
	
	//Pro pismena obsazena v hledanem prvku nastavim nizsi "skoky", viz opora
	for (int i=(length-1);i>=0;i--)
	{
		
		char_jump[value[i]]=length-i-1;
	}
	my_free (value);
	return char_jump;
}

/**
 * Funkce pro urceni indexu. Porovna nacteny znak s poslednim znakem hledaneho retezce.
 * Nacteny znak urcuje podle tabulky skoku.
 * @param source int* zdroj ve kterem hledmae retezec
 * @param find char* hledany retezec
 * @param table int* tabulka skoku
 * @return int index na kterem se nachazi retezec(pri -1 se nic nenaslo)
 */
int find_it(char* source,char* find,int* table)
{
	//index obsahuje cislo, kde se zrovna nachazime v prohledavanem retezci, k je delka hledaneho retezce -1 (pro indexaci v poli)
	int index=strlen(find)-1,k=strlen(find)-1;
	int* value;
	value = convert_to_numbers(source);
	
	//pokud je velikost stringu 1, musime projit cely zdrojovy string, znak po znaku
	if (k==0)
	{
		for (int x=0;x<strlen(source);x++)
		{
			if (source[x]==find[k])
			{
				return x;
			}
		}
	}

	//(index<=strlen(source)-1) aby program necetl kde nema
	while ((index<=strlen(source)-1)&&(k>0))
	{
		if (source[index]==find[k])
		{
			
			//zkontrolujeme prvky od nejpravejsiho k nejlevejsimu (takhle funguje boyer-moor)
			for (int x=0;x<k;x++)
			{
				index--;
				//find[k-x-1] proto, aby jsme se posunuli o jeden prvek do leva + prvky co jsme uz zkontrolovali
				if (source[index]!=find[k-x-1])
				{
					k=strlen(find)-1;
					index+=k;
					break;
				}
				//pokud promenna, pres kterou se indexuji ve foru je rovna delce hledaneho pole nasli jsme co jsme hledali
				if ((x+1)==k)
				{
					my_free (value);
					return index;
				}
			}
		}
		else
		{
			//přičtu k indexu pres ktery indexuji prohledavany retezec pocet "skoku"
			index+=table[value[index]];
			k=strlen(find)-1;
		}
	}
	//Uvolnim co jsem namalocoval a vratim -1, nic nenasel
	my_free (value);
	return -1;
}


/**
 * Funkce pro jednoduchou praci s funkci pro hledani.
 * funkce se stara o inicializaci a uvolneni ruznych promennych.
 * @param param1 TData* zdroj ve kterem hledme retezec
 * @param param2 TData* hledany retezec
 * @return index prvku. Pokud je index vyssi, nebo roven delce prohledavaneho retezce, nic nenasel
 */
TData t_boyer_moor (TData *param1,TData *param2)
{
	TData result_;
	init_exp_data(&result_);
	
	//typova kontrola parametru
	if(param1->type != STRING  || param2->type != STRING)
	{
		result_.type=NIL;
		return result_;
	}
	int result=0;
	char* source=param1->val_str;
	char* find=param2->val_str;
	
	//pokud je hledany retezec 0 rovnou vraci hodnotu 0
	if (strlen(find)==0)
	{
		result_.type=NUMBER;
		result_.val_num=0;
		return result_;
	}
	//pokud je hledany retezec vetsi, nez retezec ve kterem hleda, rovnou vraci false
	if (strlen(source)<strlen(find))
	{
		result_.type=BOOL;
		result_.val_num=0;
		return result_;
	}
	
	//tabulka skoku
	int* table_jump;
	
	table_jump=my_malloc (sizeof (int)*ALPHABET);
	table_jump=t_char_jump(find,table_jump);
	
	//funkce na nalezeni podretezce	
	result=find_it(source,find,table_jump);
	my_free (table_jump);
	if (result==-1)
	{
		result_.type=BOOL;
		result_.val_num=0;
	}
	else
	{
		//navarat je result+1, kvuli tomu ze mame indexovat od 1
		result_.type=NUMBER;
		result_.val_num=(double)(result+1);
	}
	
	return result_;
}


//funkce pro serazeni retezce
/**
 * Funkce pro serazeni pole
 * Zvoli hodnotu (pivot) uprostred neserazeneho pole, podle ktere se bude radit
 * Prochazime pole a pokud je nacteny prvek, mensi/vetsi nez pivot, posunou se
 * indexy v poli a prohodi se jednotlive prvky.
 * Po prvnim pruchodu je pole rozdeleno na 2 casti, tak prjdu obe casti rekurzivne
 * a seradim je.
 * @param array char* pole prvku pro serazeni
 * @param begin int pocatecni index v poli, nebo v casti pole
 * @param end int konecny index v poli, nebo v casti pole
 * @return void
 */
void sort_me (char* array,int begin, int end)
{
	
	int pivot,item,left_index,right_index;
	//nastavim pivot na stred pole
	pivot = array[(begin+end)/2];
	
	//levy a pravy index nastavim na konec a zacatek pole/casti pole
	left_index=begin;
	right_index=end;
	
	while (left_index<=right_index)
	{
		//pokud je hodnota mensi/vetsi nez pivot zvysim/snizim indexy
		while (array[left_index] < pivot) left_index++;
        while (array[right_index] > pivot) right_index--;
        
		
        if (left_index<=right_index)
        {
			//prohodim prvky
			item=array[left_index];
			array[left_index]=array[right_index];
			array[right_index]=item;
			
			//snizim/zvysim indexy
			right_index--;
			left_index++;
			
		}
	}
	//prochazim zbytek neserazeneho pole
	if (right_index>begin) sort_me(array,begin,right_index);
	if (left_index<end) sort_me(array,left_index,end);
}

/**
 * Funkce pro odeslani zpravnych parametru pro radici funkci, dale ulehcuje 
 * volani radici funkce.
 * @param param1 TData* retezec znaku
 * @return vraci serazene pole stringu, pokud vrati hodnotu (char*)1 nastala chyba pri mallocu
 */
TData quick_sort (TData *param1)
{
	TData result;
	init_exp_data(&result);
	
	//typova kontrola parametru
	if(param1->type != STRING)
	{
		result.type=NIL;
		return result;
	}
	char* array=param1->val_str;
	char* value;
	int length;
	length = strlen(array);
	if ((value = my_malloc (sizeof(char)*(length+1)))==NULL)
		return result;
	//prevod pismen do cisel
	strcpy(value,array);
	
	my_free (array);
	//volani funkce na serazeni
	sort_me(value,0,length-1);
	
	result.type=STRING;
	result.val_str=value;
	
	return result;
}
