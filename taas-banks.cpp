/* ============================================================================================================== */
/* == BEGIN FILE ================================================================================================ */
/* ============================================================================================================== */
/*
 ============================================================================
 Name        : taas-banks.cpp
 Author      : Matthias Thimm
 Version     : 1.0.1
 Copyright   : GPL3
 Description : The taas-banks solver for abstract argumentation.

============================================================================
*/
#define TRUE 1
#define FALSE 0
/* ============================================================================================================== */
/* ============================================================================================================== */
#include <stdio.h>
#include <ctype.h>
#include <cmath>
#include <glib.h>
#include <assert.h>
#include <stdbool.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <functional>

#include "util/bitset.c"
#include "util/raset.c"
#include "util/miscutil.c"
#include "util/settrie.cpp"

#include "taas/taas_aaf.c"

#include "sat/ipasir_solver.cpp"

#include "taas/taas_inout.c"
#include "taas/taas_labeling.c"
#include "taas/taas_basics.c"
#include "taas/taas_stcomp.cpp"
#include "taas/taas_stcompiler.cpp"

/* ============================================================================================================== */
/* ============================================================================================================== */
void solve_banks(struct TaskSpecification *task, struct AAF* aaf) {
    if(strcmp(task->problem,"BATCH-ST") == 0){
        std::vector<std::string> tasks;
        std::vector<int> args;
        for(int j = 0; j < task->number_of_additional_arguments; j++){
            tasks.push_back(task->additional_keys[j]);
            args.push_back(atoi(task->additional_values[j]));
        }
        SatSetTrieStableCompiler compiler;
        auto compilation = compiler.compile(aaf);
        for(int i = 0; i < tasks.size(); i++){
            if(tasks[i].compare("DS") == 0){
                std::cout << ( compilation.query(args[i], InferenceMode::SKEPTICAL) ? "YES" : "NO" ) << std::endl;
            }else if(tasks[i].compare("DC") == 0){
                std::cout << ( compilation.query(args[i], InferenceMode::CREDULOUS) ? "YES" : "NO" ) << std::endl;
            }else {
                std::cout << "UNKNOWN QUERY" << std::endl;
            }
        }
      return;
    }else if(strcmp(task->problem,"DS-ST") == 0){
        SatSetTrieStableCompiler compiler;
        auto compilation = compiler.compile(aaf);
        int arg = 0;
        for(int j = 0; j < task->number_of_additional_arguments; j++){
            if(strcmp(task->additional_keys[j],"-a") == 0)
                arg = atoi(task->additional_values[j]);
        }
        std::cout << ( compilation.query(arg, InferenceMode::SKEPTICAL) ? "YES" : "NO" ) << std::endl;
        return;
    }else if(strcmp(task->problem,"DC-ST") == 0){
        SatSetTrieStableCompiler compiler;
        auto compilation = compiler.compile(aaf);
        int arg = 0;
        for(int j = 0; j < task->number_of_additional_arguments; j++){
            if(strcmp(task->additional_keys[j],"-a") == 0)
                arg = atoi(task->additional_values[j]);
        }
        std::cout << ( compilation.query(arg, InferenceMode::CREDULOUS) ? "YES" : "NO" ) << std::endl;
        return;
    }
}
/* ============================================================================================================== */
/* ============================================================================================================== */
void solve_baseline_no_comp(struct TaskSpecification *task, struct AAF* aaf) {
    printf("BASELINE-NO-COMP\n");
}

/* ============================================================================================================== */
/* ============================================================================================================== */
void solve_baseline_ext_expl(struct TaskSpecification *task, struct AAF* aaf) {
    printf("BASELINE-EXT-EXPLICIT\n");
}

/* ============================================================================================================== */
/* ============================================================================================================== */
void solve(struct TaskSpecification *task, struct AAF* aaf) {
    switch(task->solver_type){
        case 1 :    solve_banks(task,aaf);
                    break;
        case 2 :    solve_baseline_no_comp(task,aaf);
                    break;
        case 3 :    solve_baseline_ext_expl(task,aaf);
                    break;
    }
}

/* ============================================================================================================== */
int main(int argc, char *argv[]){
  // General solver information
	struct SolverInformation *info = taas__solverinformation(
			(char*) "taas-banks v1.0.1 (2024-11-29)\nMatthias Thimm (matthias.thimm@fernuni-hagen.de)",
			(char*) "[i23]",
			(char*) "[DS-ST,DC-ST,BATCH-ST]"
		);
  return taas__solve(argc,argv,info,solve);
}

/* ============================================================================================================== */
/* == END FILE ================================================================================================== */
/* ============================================================================================================== */
