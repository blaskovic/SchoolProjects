/**
 * ------------------------------------------
 * Project : Interpreter jazyka IFJ11 
 * Name    : ial
 * File    : ial.h
 * Authors : Karel Hala (xhalak00)
 *         : Pavlina Bartikova (xbarti00)
 *         : Tomas Goldmann (xgoldm03)
 *         : Branislav Blaskovic (xblask00)
 * ------------------------------------------
 */


//funkce pro BVS
Tbin_tree* bvs_init_tree(Tbin_tree *init);
Tbin_tree* bvs_make_leaf(char* value,Tbin_tree *leaf_);
Tbin_tree* bvs_make_tree(Tbin_tree* bin_tree,char* var, int not_declared);
int bvs_search(Tbin_tree* bin_tree,char* var);
int bvs_pre_order (Tbin_tree* bin_tree);
void bvs_destroy_tree(Tbin_tree *tree_);
Tbin_tree* bvs_update_inst_addr(Tbin_tree* bin_tree,char* var, TInstruction* instruction);
Tbin_tree *bvs_get_key(Tbin_tree* bin_tree,char* var);
Tbin_tree* bvs_increment_function_arg(Tbin_tree* bin_tree, char* var);
int bvs_get_function_arg(Tbin_tree* bin_tree, char* var);
TInstruction *bvs_get_instruction_address(Tbin_tree* bin_tree, char* var);
Tbin_tree* bvs_set_declaration(Tbin_tree* bin_tree,char* var, int not_declared);
int bvs_is_not_declared(Tbin_tree *bin_tree);


//funkce pouzite pro serazeni
TData quick_sort (TData *param1);
void sort_me (char* array,int left, int right);

//funkce pouzite pro find
int find_it (char* source,char* find,int* table);
TData t_boyer_moor (TData *param1,TData *param2);
int* t_char_jump (char* string,int* char_jump);
