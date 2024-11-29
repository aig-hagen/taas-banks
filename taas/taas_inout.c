/* ============================================================================================================== */
/* == BEGIN FILE ================================================================================================ */
/* ============================================================================================================== */
/*
 ============================================================================
 Name        : taas_inout.c
 Author      : Matthias Thimm
 Version     : 2.0
 Copyright   : GPL3
 Description : Utlity functions for taas solvers.
 ============================================================================
 */

/**
 * General solver information
 */
#include <cstdlib>
struct SolverInformation{
    char* description;
    char* formats;
    char* problems;
};

/**
 * Task specification
 */
struct TaskSpecification{
  /** The problem to be solved. */
  char* problem;
  /** The file path */
  char* file;
  /** solver_type=1 the banks compilation approach
    * solver_type=2 the baseline "no compilation" approach
    * solver_type=3 the baseline "store all extensions explictly" approach
    */
  int solver_type;
  /** additional arguments */
  int number_of_additional_arguments;
  char** additional_keys;
  char** additional_values;
};

/**
 * initialises general solver information
 */
struct SolverInformation* taas__solverinformation(char* description, char* formats, char* problems){
    struct SolverInformation *info = (struct SolverInformation *) malloc(sizeof(struct SolverInformation));
    info->description = description;
    info->formats = formats;
    info->problems = problems;
    return info;
}

void taas__solverinformation_destroy(struct SolverInformation *info){
    free(info);
}

/**
 * Handles the command. If basic solver information is asked for, NULL is returned;
 * otherwise the task specification is returned
 */
struct TaskSpecification* taas__cmd_handle(int argc, char *argv[], struct SolverInformation* info){
  // TODO: add some checks to validate input
  // if no arguments are given just print out the version info
  // parse for a problem
  struct TaskSpecification* task = (struct TaskSpecification*) malloc(sizeof(struct TaskSpecification));
  task->number_of_additional_arguments = 0;
  task->additional_keys = (char**) malloc(sizeof(char*));
  task->additional_values = (char**) malloc(sizeof(char*));
  task->solver_type = 1;
  int param = 0;
  for(int i = 1; i < argc; i++){
    if(strcmp(argv[i],"-p") == 0){
      task->problem = argv[++i];
      param++;
      continue;
    }
    if(strcmp(argv[i],"-f") == 0){
      task->file = argv[++i];
      param++;
      continue;
    }
    if(strcmp(argv[i],"-t") == 0){
      task->solver_type = atoi(argv[++i]);
      param++;
      continue;
    }
    // for the parameter "--formats" print out the formats and exit
    if(strcmp(argv[i],"--formats") == 0){
      printf("%s\n", info->formats);
      return NULL;
    }
    // for the parameter "--problems" print out the problems and exit
    if(strcmp(argv[i],"--problems") == 0){
      printf("%s\n", info->problems);
      return NULL;
    }
    // parse an additional argument
    task->number_of_additional_arguments++;
    task->additional_keys = (char **) realloc(task->additional_keys, task->number_of_additional_arguments * sizeof(char*));
    task->additional_values = (char **) realloc(task->additional_values, task->number_of_additional_arguments * sizeof(char*));
    task->additional_keys[task->number_of_additional_arguments-1] = argv[i];
    task->additional_values[task->number_of_additional_arguments-1] = argv[++i];
  }
  //if no problem and file are given, just print out information
  if(param < 2){
    printf("%s\n", info->description);
    return NULL;
  }
  return task;
}

void taas__cmd_destroy(struct TaskSpecification *task){
  free(task);
}

/**
 * Returns the value of an additional argument; if there is no
 * value with the given key, NULL is returned;
 */
char* taas__task_get_value(struct TaskSpecification *task, char* key){
  for(int i = 0; i < task->number_of_additional_arguments;i++){
    if(strcmp(task->additional_keys[i],key) == 0)
      return task->additional_values[i];
  }
  return NULL;
}

/** Read a file in ICCMA23 format into the data structures */
void taas__readFile_i23(char* path, struct AAF* aaf){
  // first get the number of arguments
  FILE* fp = fopen(path,"r");
  char* row = NULL;
  size_t len = 0;
  ssize_t read;
  while((read = getline(&row, &len, fp)) != -1) {
    if(strncmp(trimwhitespace(row),"#",1) == 0)
      continue;
    if(strncmp(trimwhitespace(row),"p af",4) == 0){
      aaf->number_of_arguments = atoi(&(trimwhitespace(row)[4]));
      break;
    }
  }
  // now do the actual parsing
  aaf->children = (GSList**) malloc(aaf->number_of_arguments * sizeof(GSList*));
  aaf->parents = (GSList**) malloc(aaf->number_of_arguments * sizeof(GSList*));
  aaf->number_of_attackers = (int*) malloc(aaf->number_of_arguments * sizeof(int));
  aaf->number_of_attacks = 0;
  aaf->initial = (struct BitSet*) malloc(sizeof(struct BitSet));
  bitset__init(aaf->initial, aaf->number_of_arguments);
  // all bits initially one
  bitset__setAll(aaf->initial);
  aaf->loops = (struct BitSet*) malloc(sizeof(struct BitSet));
  bitset__init(aaf->loops, aaf->number_of_arguments);
  // all bits initially zero
  bitset__unsetAll(aaf->loops);
  // initialise arguments
  for(int idx = 1; idx <= aaf->number_of_arguments; idx++){
    aaf->children[idx-1] = NULL;
    aaf->parents[idx-1] = NULL;
    aaf->number_of_attackers[idx-1] = 0;
  }
  while ((read = getline(&row, &len, fp)) != -1) {
    if(strcmp(trimwhitespace(row),"") == 0)
      continue;
		if(strncmp(trimwhitespace(row),"#",1) == 0)
      continue;
    // parse an attack
    aaf->number_of_attacks++;
	int idx = 0;
	while(row[idx] != ' ')idx++;
	row[idx] = 0;
    int* idx1 = (int*) malloc(sizeof(int));
    int* idx2 = (int*) malloc(sizeof(int));
    *idx1 = atoi(row);
	*idx2 = atoi(&row[idx+1]);
    aaf->children[*idx1-1] = g_slist_prepend(aaf->children[*idx1-1], idx2);
    aaf->parents[*idx2-1] = g_slist_prepend(aaf->parents[*idx2-1], idx1);
    aaf->number_of_attackers[*idx2-1]++;
    // if an argument is attacked, it is not initial
	bitset__unset(aaf->initial,*idx2-1);
    // check for self-attacking arguments
    if(*idx1 == *idx2){
      bitset__set(aaf->loops,*idx1-1);
    }
  }
  fclose(fp);
}

/* ============================================================================================================== */
/* == END FILE ================================================================================================== */
/* ============================================================================================================== */
