#include "satapi.h"


/******************************************************************************
 * We explain here the functions you need to implement
 *
 * Rules:
 * --You cannot change any parts of the function signatures
 * --You can/should define auxiliary functions to help implementation
 * --You can implement the functions in different files if you wish
 * --That is, you do not need to put everything in a single file
 * --You should carefully read the descriptions and must follow each requirement
 ******************************************************************************/


/******************************************************************************
 * Given a variable index i, you should return the corresponding variable
 * structure (notice you must return a pointer to the structure)
 *
 * Note variable indices range from 1 to n where n is the number of variables
 ******************************************************************************/
Var* index2varp(unsigned long i, SatState* sat_state) {

  if( sat_state != NULL && i > 0 && i <= sat_state->variables_size )
    return sat_state->variables[ i - 1 ];
  return NULL;
}


/******************************************************************************
 * Given a variable var, you should return
 * --its positive literal (pos_literal)
 * --its negative literal (neg_literal)
 *
 *
 * Given a literal lit, set_literal(lit) should return
 * --1 if lit is set in the current setting
 * --0 if lit is free
 *
 * Note a literal is set either by a decision or implication
 * Do not forget to update the status of literals when you run unit resolution
 ******************************************************************************/
Lit* pos_literal(Var* var) {

  if( var != NULL )
    return var->pos_literal;
  return NULL;
}

Lit* neg_literal(Var* var) {

  if( var != NULL )
    return var->neg_literal;
  return NULL;
}

BOOLEAN set_literal(Lit* lit) {

  if( lit != NULL )
    return lit->var_ptr->is_set;
  return 0;
}

BOOLEAN asserted_literal(Lit* lit) {

  if( lit != NULL && lit->var_ptr->is_set == 1 && lit->var_ptr->set_sign == ( lit->index > 0 ) )
    return 1;
  return 0;
}

BOOLEAN resolved_literal(Lit* lit) {

  if( lit != NULL && lit->var_ptr->is_set == 1 && lit->var_ptr->set_sign == ( lit->index < 0 ) )
    return 1;
  return 0;
}

/******************************************************************************
 * Given a clause index i, you should return the corresponding clause
 * structure (notice you must return a pointer to the structure)
 *
 * Note clause indices range from 1 to m where m is the number of clauses
 ******************************************************************************/
Clause* index2clausep(unsigned long i, SatState* sat_state) {

  if( sat_state != NULL && i > 0 && i <= sat_state->clauses_size )
    return sat_state->clauses[ i - 1 ];
  return NULL;
}


/******************************************************************************
 * Given a clause, you should return
 * --1 if the clause is subsumed in the current setting
 * --0 otherwise
 *
 * Note a clause is subsumed if at least one of its literals is implied
 * Do not forget to update the status of clauses when you run unit resolution
 ******************************************************************************/
BOOLEAN subsumed_clause(Clause* clause) {

  int i;

  if( clause == NULL )
    return 0;
  return clause->is_subsumed;
}


/******************************************************************************
 * A SatState should keep track of pretty much everything you will need to
 * condition/uncondition, perform unit resolution, and do clause learning
 *
 * Given a string cnf_fname, which is a file name of the input CNF, you should
 * construct a SatState
 *
 * This construction will depend on how you define a SatState
 * Still, you should at least do the following:
 * --read a CNF (in DIMACS format) from the file
 * --initialize variables (n of them)
 * --initialize literals  (2n of them)
 *
 * Once a SatState is constructed, all of the functions that work on a SatState
 * should be ready to use
 *
 * You should also write a function that frees the memory allocated by a
 * SatState (free_sat_state)
 ******************************************************************************/
SatState* construct_sat_state(char* cnf_fname) {
  SatState *ret = NULL;;
  FILE *fp;
  fp = fopen(cnf_fname, "r");

  if(fp == NULL){
    ret = NULL;
  }
  else{
    size_t buf = 80;
    char *buffer = malloc(buf * sizeof(char));
    int clause_count = 0;
    int indx = 0;
    while(getline(&buffer, &buf, fp)!=-1){
       char fst_char = buffer[0];
       if( fst_char == 'c')
         continue;
       else if( fst_char== 'p'){
         ret-> variables_size =(buffer[6]);
         ret-> clauses_size = buffer[8];
         clause_count = ret-> clauses_size;
         continue;
       }
       else if( !(fst_char >= 'a' && fst_char <= 'z') && clause_count > 0){
         if(indx == 0){
           Clause cls[clause_count];

         }
       }
       else
         continue;
    }


  }
  return ret; // dummy value
}

void free_sat_state(SatState* sat_state) {

  // TODO
  return; // dummy value
}

BOOLEAN unapply_literal(Lit *lit, SatState* sat_state) {
  unsigned long index;

  if( lit == NULL || !set_literal(lit) )
    return 0;

  lit->var_ptr->is_set = 1;
  for( index = 0; index < lit->var_ptr->used_clauses_size; ++index ) {
    var_ptr->needs_checking = 1;
  }

  return 1;
}

BOOLEAN apply_literal(Lit* lit, Clause* clause, SatState* sat_state) {

  long index, max_depth, max_level;

  if( lit == NULL || set_literal(lit) )
    return 0;

  lit->var_ptr->is_set = 1;
  lit->var_ptr->set_sign = lit->index > 0;
  lit->var_ptr->implication_clause = clause;

  // determine and set the depth and level of this new variable setting variable
  if( clause != NULL ) {
    max_level = 1;
    max_depth = -1;
    for( index = 0; index < clause->elements_size; ++index ) {
      if( clause->elements[index]->var_ptr->is_set == 0 || clause->elements[index] == lit )
        continue;
      if( clause->elements[index]->var_ptr->decision_level > max_level )
        max_level = clause->elements[index]->var_ptr->decision_level;
      if( clause->elements[index]->var_ptr->set_depth > max_depth )
        max_depth = clause->elements[index]->var_ptr->set_depth;
    }
    lit->var_ptr->decision_level = max_level;
    lit->var_ptr->set_depth = max_depth + 1;
  }
  else {
    lit->var_ptr->decision_level = sat_state->decisions_size + 1;
    lit->var_ptr->set_depth = 0;
  }

  // flag all the clauses that use this new setting
  for( index = 0; index < lit->var_ptr->used_clauses_size; ++index )
    if( lit->var_ptr->used_clauses[index]->is_subsumed == 0 )
      lit->var_ptr->used_clauses[index]->needs_checking = 1;

  return 1;
}

/******************************************************************************
 * This function should set literal lit to true and then perform unit resolution
 * It returns 1 if unit resolution succeeds, 0 otherwise
 *
 * Note if the current decision level is L in the beginning of the call, it
 * should be updated to L+1 so that the decision level of lit and all other
 * literals implied by unit resolution is L+1
 ******************************************************************************/
BOOLEAN decide_literal(Lit* lit, SatState* sat_state) {

  sat_state->decisions[ sat_state->decisions_size++ ] = lit;
  apply_literal(lit, NULL, sat_state);
  return unit_resolution(sat_state);
}

BOOLEAN imply_literal(Lit *lit, Clause *clause, SatState *sat_state) {

  if( sat_state == NULL || lit == NULL || set_literal(lit) )
    return 0;

  sat_state->implications[ sat_state->implications_size++ ] = lit;
  apply_literal(lit, clause, sat_state);
  ++sat_state->implications_size;

  return 1;
}


/******************************************************************************
 * Given a SatState, which should contain data related to the current setting
 * (i.e., decided literals, asserted literals, subsumed clauses, decision
 * level, etc.), this function should perform unit resolution at the current
 * decision level
 *
 * It returns 1 if succeeds, 0 otherwise (after constructing an asserting
 * clause)
 *
 * There are three possible places where you should perform unit resolution:
 * (1) after deciding on a new literal (i.e., decide_literal(SatState*))
 * (2) after adding an asserting clause (i.e., add_asserting_clause(SatState*))
 * (3) neither the above, which would imply literals appearing in unit clauses
 *
 * (3) would typically happen only once and before the other two cases
 * It may be useful to distinguish between the above three cases
 *
 * Note if the current decision level is L, then the literals implied by unit
 * resolution must have decision level L
 *
 * This implies that there must be a start level S, which will be the level
 * where the decision sequence would be empty
 *
 * We require you to choose S as 1, then literals implied by (3) would have 1 as
 * their decision level (this level will also be the assertion level of unit
 * clauses)
 *
 * Yet, the first decided literal must have 2 as its decision level
 ******************************************************************************/

unsigned long recurse_paths(Lit *lit, unsigned long level) {
  unsigned long paths, index;

  if( lit->var_ptr->implication_clause == NULL ) {
    paths = 1;
  }
  else {
    if( lit->var_ptr->implication_clause->elements_size == 1 )
      paths = 1;
    else {
      paths = 0;
      for( index = 0; index < lit->var_ptr->implication_clause->elements_size; ++index ) {
        if( lit->var_ptr->implication_clause->elements[index] == lit )
          continue;
        paths += recurse_paths( lit->var_ptr->implication_clause->elements[index], level );
        if( lit->var_ptr->set_depth > lit->var_ptr->implication_clause->elements[index]->var_ptr->used_depth )
          lit->var_ptr->implication_clause->elements[index]->var_ptr->used_depth = lit->var_ptr->set_depth;
      }
    }
  }
  lit->var_ptr->path_count += paths;

  if( lit->var_ptr->decision_level == level )
    return paths;
  return 0;
}

unsigned long calc_decision_level(Clause *clause) {
  unsigned long decision_level;

  // calculate the maximum level in the conflict clause
  decision_level = 0;
  for( index = 0; index < clause->elements_size; ++index ) {
    if( decision_level < clause->elements[index]->var_ptr->decision_level )
      decision_level = clause->elements[index]->var_ptr->decision_level;
  }

  return decision_level;
}

unsigned long calc_total_paths(Clause *clause, unsigned long decision_level, SatState *sat_state) {
  unsigned long total_paths, index;

  // clear out any old data in any of the implications and decisions
  for( index = 0; index < sat_state->implications_size; ++index ) {
    sat_state->implications[index]->var_ptr->path_count = 0;
    sat_state->implications[index]->var_ptr->used_depth = 0;
  }
  for( index = 0; index < sat_state->decisions_size; ++index ) {
    sat_state->decisions[index]->var_ptr->path_count = 0;
    sat_state->decisions[index]->var_ptr->used_depth = 0;
  }

  // calculate the total number of same-level paths from the conflict clause to the end
  total_paths = 0;
  for( index = 0; index < clause->elements_size; ++index ) {
    total_paths += recurse_paths( clause->elements[index], decision_level );
  }

  return total_paths;
}

Lit* find_UIP(SatState* sat_state, unsigned long decision_level, unsigned long total_paths) {
  long index;

  for(index = sat_state->implications_size-1; index >= 0; --index ) {
    if( sat_state->implications[index]->var_ptr->decision_level < decision_level ) {
      index = -1;
      break;
    }
    if( sat_state->implications[index]->var_ptr->path_count == total_paths )
      break;
  }

  if( index < 0 ) {
    if( decision_level > 1 )
      return sat_state->decisions[ decision_level - 2 ];
    return NULL;
  }
  return sat_state->implications[index];
}

// this will actually build the new clause
Clause* build_assertion_clause(Lit *uip, SatState *sat_state) {
  unsigned long index, cut_size;
  long assertion_level;
  Lit **cut;
  Clause *clause;

  // this would indicate we have no uip (conflict through only unit clause implications)
  if( uip == NULL )
    return NULL;

  cut = malloc( sat_state->variables_size * sizeof(Lit*) );
  cut_size = 0;
  for( index = 0; index < implications_size; ++index ) {
    if( sat_state->implications[index]->var_ptr->path_count > 0 ) {
      if( sat_state->implications[index]->var_ptr->decision_level < uip->var_ptr->decision_level
        && sat_state->implications[index]->var_ptr->decision_level > assertion_level ) {
        assertion_level = sat_state->implications[index]->var_ptr->decision_level;
      }
      if( sat_state->implications[index]->var_ptr->set_depth <= uip->var_ptr->set_depth
        && sat_state->implications[index]->var_ptr->used_depth >= uip->var_ptr->set_depth + 1 ) {
          cut[cut_size++] = sat_state->implications[index];
        }
    }
  }

  clause = malloc(sizeof(Clause));
  clause->elements = malloc( cut_size * sizeof(Lit*) );
  clause->elements[size] = cut_size;
  for( index = 0; index < cut_size; ++index ) {
    clause->elements[index] = cut[index];
  }
  free(cut);

  sat_state->assertion_clause = clause;
  sat_state->assertion_level = assertion_level;

  return clause;
}

// this method runs all relevant calculations for assertion clause generation
// THIS METHOD CALLS THE METHOD ABOVE, only cal this method directly
void generate_assertion_clause(Clause *clause, SatState* sat_state) {
  unsigned long total_paths, decision_level;
  Lit *uip;

  decision_level = calc_decision_level(clause);
  total_paths = calc_total_paths(clause, decision_level, sat_state);
  uip = find_UIP(sat_state, decision_level, total_paths);
  build_assertion_clause(uip, sat_state);

  return;
}

BOOLEAN check_clause( Clause* clause ) {
  long index;
  Lit *lit_1, *lit_2;
  BOOLEAN found_lit_1, found_lit_2;

  // if both watches are still free, then just return
  if( !set_literal( clause->watch_1 ) && !set_literal( clause->watch_2 ) && clause->watch_1 != clause->watch_2 )
    return 1;

  // Now we know that one of our watches has changed, search for two free watch statements
  found_lit_1 = 0;
  found_lit_2 = 0;
  for( index = 0; index < clause->elements_size && !( found_lit_1 && found_lit_2 ); ++index ) {
    if( asserted_literal(clause->elements[index]) ) {
      clause->is_subsumed = 1;
      return 1;
    }
    if( set_literal(clause->elements[index]) == 0 ) {
      if( !found_lit_1 ) {
        lit_1 = clause->elements[index];
        found_lit_1 = 1;
      }
      else {
        lit_2 = clause->elements[index];
        found_lit_2 = 1;
      }
    }
  }

  if( !found_lit_1 && !found_lit_2 ) {
    // if we have found a contradiction
    clause->is_subsumed = 0;
    return 0;
  }
  else if( found_lit_1 && !found_lit_2 ) {
    // if we have a new implication
    clause->is_subsumed = 1;
    imply_literal(lit_1);
  }
  else {
    // if the clause still has more than one free variable
    clause->watch_1 = lit_1;
    clause->watch_2 = lit_2;
    clause->is_subsumed = 0;
  }

  return 1;
}

BOOLEAN check_literal(Lit *lit, SatState* sat_state) {
  long index;

  for( index = 0; index < lit->var_ptr->used_clauses_size; ++index ) {
    if( lit->var_ptr->used_clauses[index]->needs_checking == 1 ) {
      lit->var_ptr->used_clause[index]->needs_checking = 1;
      if( check_clause(lit->var_ptr->used_clauses[index]) == 0 ) {
        generate_assertion_clause(lit->var_ptr->used_clause[index], sat_state);
        return 0;
      }
    }
  }
  return 1;
}

BOOLEAN unit_resolution(SatState* sat_state) {

  while( sat_state->decisions_applied < sat_state->decisions_size ) {
    if( !check_literal( sat_state->decisions[ sat_state->decisions_applied++ ] ) )
      return 0;
  }
  while( sat_state->implications_applied < sat_state->implications_size ) {
    if( !check_literal( sat_state->implications[ sat_state->implications_applied++ ] ) )
      return 0;
  }
  return 1;
}


/******************************************************************************
 * This function should simply undo all set literals at the current decision
 * level
 ******************************************************************************/
void undo_unit_resolution(SatState* sat_state) {
  unsigned long index;
  Lit *decision;

  if( sat_state == NULL || sat_state->decisions_size == 0 )
    return;

  decision = sat_state->decisions[--decisions_size];

  for( index = sat_state->implications_size-1;
    index >= 0 && sat_state->implications[index]->var_ptr->decision_level >= decision->var_ptr->decision_level;
    --index ) {
      unapply_literal(sat_state->implications[index]);
    }
  for( index = 0; index < sat_state->clauses_size; ++index ) {
    if( sat_state->clauses[index]->needs_checking ) {
      check_clause(sat_state->clauses[index]);
    }
  }
  return;
}


/******************************************************************************
 * This function should undo all set literals at the current decision level (you
 * can in fact call undo_unit_resolution(SatState*))
 *
 * Note if the current decision level is L in the beginning of the call, it
 * should be updated to L-1 before the call ends
 ******************************************************************************/
void undo_decide_literal(SatState* sat_state) {
  return undo_unit_resolution(sat_state);
}


/******************************************************************************
 * This function must be called after a contradiction has been found (by unit
 * resolution), an asserting clause constructed, and backtracking took place to
 * the assertion level (i.e., the current decision level is the same as the
 * assertion level of the asserting clause)
 *
 * This function should add the asserting clause into the set of learned clauses
 * (so that unit resolution from there on would also take into account the
 * asserting clause), and then perform unit resolution
 *
 * It returns 1 if unit resolution succeeds, which means the conflict is
 * cleared, and 0 otherwise (that is, we have a new asserting clause with a new
 * assertion level)
 *
 * Note since the learned clause is asserting and we are at the assertion level
 * of the clause, it will become a unit clause under the current setting
 *
 * Also, if the learned clause itself is a unit clause, its assertion level must
 * be the same as the start level S, which is 1 (i.e., the level in
 * which no decision is made)
 ******************************************************************************/
BOOLEAN add_asserting_clause(SatState* sat_state) {
  unsigned long index;

  if( sat_state == NULL )
    return 0;

  sat_state->assertion_clause->needs_checking = 1;
  sat_state->assertion_clause->is_subsumed = 0;

  for( index = 0; index < sat_state->assertion_clause->elements_size; ++index ) {
    // TODO: have to deal with over-capacity issue
    sat_state->assertion_clause->elements[index]->var_ptr->used_clauses[
        sat_state->assertion_clause->elements[index]->var_ptr->used_clauses_size++
      ] = sat_state->assertion_clause;
  }

  // TODO: have to deal with over-capacity issue
  sat_state->clauses[ sat_state->clauses_size++ ] = sat_state->assertion_clause;
  sat_state->assertion_clause = NULL;
  sat_state->assertion_clause_level = 0;

  return unit_resolution(sat_state);
}


/******************************************************************************
 * This function can be called after a contradiction has been found (by unit
 * resolution), an asserting clause constructed, and the conflict is not cleared
 * yet (that is, conflict_exists(SatState*) must return 1 at the time of call)
 *
 * It returns 1 if the current decision level is the same as the assertion level
 * of the asserting clause, 0 otherwise
 ******************************************************************************/
BOOLEAN at_assertion_level(SatState* sat_state) {

  if( sat_state != NULL )
    return sat_state->assertion_clausse_level == sat_state->decisions_size + 1;
  return 0;
}


/******************************************************************************
 * It returns 1 if the current decision level is the same as the start level,
 * which is 1 (i.e., the level in which no decision is made), 0 otherwise
 ******************************************************************************/
BOOLEAN at_start_level(SatState* sat_state) {

  if( sat_state != NULL )
    return sat_state->decisions_size == 0;
  return 0;
}


/******************************************************************************
 * It returns 1 if there is a conflict in the current setting, 0 otherwise
 *
 * --Initially there is no conflict
 * --If unit resolution finds a contradiction, then we have a conflict
 * --A conflict is cleared when we backtrack to the assertion level, add the
 * asserting clause into the set of learned clauses, and successfully perform
 * unit resolution (i.e., the call add_asserting_clause(SatState*) returns 1)
 ******************************************************************************/
BOOLEAN conflict_exists(SatState* sat_state) {

  if( sat_state != NULL ) {
    // Is this enough here, or do we ned to actually test: unit_resolution(sat_state);
    return sat_state->conflict_clause_level == sat_state->decisions_size;
  }
  return 0;
}

/******************************************************************************
 * end
 ******************************************************************************/
