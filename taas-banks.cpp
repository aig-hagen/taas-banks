/* ============================================================================================================== */
/* == BEGIN FILE ================================================================================================ */
/* ============================================================================================================== */
/*
 ============================================================================
 Name        : taas-banks.cpp
 Author      : Matthias Thimm
 Version     : 1.0.3
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
bool solve_baseline_no_comp_ds(struct TaskSpecification *task, struct AAF* aaf, int arg) {
    // Base encoding for stable semantics
    IpasirSolver solver;
    sat__init(solver, aaf->number_of_arguments);
    // add arg
    sat__addClause1(solver,-arg);
    // Rules for stable semantics
    for (int a = 1; a <= aaf->number_of_arguments; a++) {
        if (aaf->number_of_attackers[a-1] == 0) {
            sat__addClause1(solver,a);
        } else {
            std::vector<int> attackersOr;
            for(GSList* node2 = aaf->parents[a-1]; node2 != NULL; node2 = node2->next){
                attackersOr.push_back(*(int*)node2->data);
                sat__addClause2(solver,-a,-*(int*)node2->data);
            }
            attackersOr.push_back(a);
            attackersOr.push_back(0);
            sat__addClauseZTVec(solver,attackersOr);
        }
    }
    return ( sat__solve(solver) == 10 ? false : true );
}

bool solve_baseline_no_comp_dc(struct TaskSpecification *task, struct AAF* aaf, int arg) {
    // Base encoding for stable semantics
    IpasirSolver solver;
    sat__init(solver, aaf->number_of_arguments);
    // add arg
    sat__addClause1(solver,arg);
    // Rules for stable semantics
    for (int a = 1; a <= aaf->number_of_arguments; a++) {
        if (aaf->number_of_attackers[a-1] == 0) {
            sat__addClause1(solver,a);
        } else {
            std::vector<int> attackersOr;
            for(GSList* node2 = aaf->parents[a-1]; node2 != NULL; node2 = node2->next){
                attackersOr.push_back(*(int*)node2->data);
                sat__addClause2(solver,-a,-*(int*)node2->data);
            }
            attackersOr.push_back(a);
            attackersOr.push_back(0);
            sat__addClauseZTVec(solver,attackersOr);
        }
    }
    return ( sat__solve(solver) == 10 ? true : false );
}

void solve_baseline_no_comp(struct TaskSpecification *task, struct AAF* aaf) {
    if(strcmp(task->problem,"BATCH-ST") == 0){
        std::vector<std::string> tasks;
        std::vector<int> args;
        for(int j = 0; j < task->number_of_additional_arguments; j++){
            tasks.push_back(task->additional_keys[j]);
            args.push_back(atoi(task->additional_values[j]));
        }
        for(int i = 0; i < tasks.size(); i++){
            if(tasks[i].compare("DS") == 0){
                std::cout << ( solve_baseline_no_comp_ds(task,aaf,args[i]) ? "YES" : "NO" ) << std::endl;
            }else if(tasks[i].compare("DC") == 0){
                std::cout << ( solve_baseline_no_comp_dc(task,aaf,args[i]) ? "YES" : "NO" ) << std::endl;
            }else {
                std::cout << "UNKNOWN QUERY" << std::endl;
            }
        }
      return;
    }else if(strcmp(task->problem,"DS-ST") == 0){
        int arg = 0;
        for(int j = 0; j < task->number_of_additional_arguments; j++){
            if(strcmp(task->additional_keys[j],"-a") == 0)
                arg = atoi(task->additional_values[j]);
        }
        std::cout << ( solve_baseline_no_comp_ds(task,aaf,arg) ? "YES" : "NO" ) << std::endl;
        return;
    }else if(strcmp(task->problem,"DC-ST") == 0){
        int arg = 0;
        for(int j = 0; j < task->number_of_additional_arguments; j++){
            if(strcmp(task->additional_keys[j],"-a") == 0)
                arg = atoi(task->additional_values[j]);
        }
        std::cout << ( solve_baseline_no_comp_dc(task,aaf,arg) ? "YES" : "NO" ) << std::endl;
        return;
    }
}

/* ============================================================================================================== */
/* ============================================================================================================== */
std::set<std::set<int>> all_exts(struct AAF* aaf){
    // Base encoding for stable semantics
    IpasirSolver solver;
    sat__init(solver, aaf->number_of_arguments);
    // Rules for stable semantics
    for (int a = 1; a <= aaf->number_of_arguments; a++) {
        if (aaf->number_of_attackers[a-1] == 0) {
            sat__addClause1(solver,a);
        } else {
            std::vector<int> attackersOr;
            for(GSList* node2 = aaf->parents[a-1]; node2 != NULL; node2 = node2->next){
                attackersOr.push_back(*(int*)node2->data);
                sat__addClause2(solver,-a,-*(int*)node2->data);
            }
            attackersOr.push_back(a);
            attackersOr.push_back(0);
            sat__addClauseZTVec(solver,attackersOr);
        }
    }
    // Iteratively find and add extensions
    std::set<std::set<int>> exts;
    while (true) {
        int sat = sat__solve(solver);
        if (sat == 20) {
            break; // No more extensions
        }
        std::set<int> ext;
        std::vector<int> clause;
        for (int a = 1; a <= aaf->number_of_arguments; a++){
            if (sat__get(solver,a) > 0) {
                ext.insert(a);
                clause.push_back(-a);
            }
        }
        clause.push_back(0);
        exts.insert(ext);
        sat__addClauseZTVec(solver,clause);
    }
    return exts;
}

void solve_baseline_ext_expl(struct TaskSpecification *task, struct AAF* aaf) {
    if(strcmp(task->problem,"BATCH-ST") == 0){
        std::vector<std::string> tasks;
        std::vector<int> args;
        for(int j = 0; j < task->number_of_additional_arguments; j++){
            tasks.push_back(task->additional_keys[j]);
            args.push_back(atoi(task->additional_values[j]));
        }
        // determine all extensions
        std::set<std::set<int>> exts = all_exts(aaf);
        for(int i = 0; i < tasks.size(); i++){
            if(tasks[i].compare("DS") == 0){
                bool is_accepted = true;
                for(std::set<int> ext: exts){
                    if(ext.find(args[i]) == ext.end()){
                        is_accepted = false;
                        break;
                    }
                }
                std::cout << (is_accepted ? "YES" : "NO" ) << std::endl;
            }else if(tasks[i].compare("DC") == 0){
                bool is_accepted = false;
                for(std::set<int> ext: exts){
                    if(ext.find(args[i]) != ext.end()){
                        is_accepted = true;
                        break;
                    }
                }
                std::cout << ( is_accepted ? "YES" : "NO" ) << std::endl;
            }else {
                std::cout << "UNKNOWN QUERY" << std::endl;
            }
        }
      return;
    }else if(strcmp(task->problem,"DS-ST") == 0){
        std::set<std::set<int>> exts = all_exts(aaf);
        int arg = 0;
        for(int j = 0; j < task->number_of_additional_arguments; j++){
            if(strcmp(task->additional_keys[j],"-a") == 0)
                arg = atoi(task->additional_values[j]);
        }
        bool is_accepted = true;
        for(std::set<int> ext: exts){
            if(ext.find(arg) == ext.end()){
                is_accepted = false;
                break;
            }
        }
        std::cout << ( is_accepted ? "YES" : "NO" ) << std::endl;
        return;
    }else if(strcmp(task->problem,"DC-ST") == 0){
        std::set<std::set<int>> exts = all_exts(aaf);
        int arg = 0;
        for(int j = 0; j < task->number_of_additional_arguments; j++){
            if(strcmp(task->additional_keys[j],"-a") == 0)
                arg = atoi(task->additional_values[j]);
        }
        bool is_accepted = false;
        for(std::set<int> ext: exts){
            if(ext.find(arg) != ext.end()){
                is_accepted = true;
                break;
            }
        }
        std::cout << ( is_accepted ? "YES" : "NO" ) << std::endl;
        return;
    }
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
			(char*) "taas-banks v1.0.3 (2024-12-11)\nMatthias Thimm (matthias.thimm@fernuni-hagen.de)",
			(char*) "[i23]",
			(char*) "[DS-ST,DC-ST,BATCH-ST]"
		);
  return taas__solve(argc,argv,info,solve);
}

/* ============================================================================================================== */
/* == END FILE ================================================================================================== */
/* ============================================================================================================== */
