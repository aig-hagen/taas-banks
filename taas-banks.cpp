/* ============================================================================================================== */
/* == BEGIN FILE ================================================================================================ */
/* ============================================================================================================== */
/*
 ============================================================================
 Name        : taas-banks.cpp
 Author      : Matthias Thimm
 Version     : 1.0.0
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

#include "util/bitset.c"
#include "util/raset.c"
#include "util/miscutil.c"

#include "taas/taas_aaf.c"
#include "taas/taas_inout.c"
#include "taas/taas_labeling.c"
#include "taas/taas_basics.c"

#include "sat/ipasir_solver.cpp"

/* ============================================================================================================== */
/* ============================================================================================================== */
void solve(struct TaskSpecification *task, struct AAF* aaf) {
    printf("TODO");
}

/* ============================================================================================================== */
int main(int argc, char *argv[]){
  // General solver information
	struct SolverInformation *info = taas__solverinformation(
			(char*) "taas-banks v1.0.0 (2024-11-28)\nMatthias Thimm (matthias.thimm@fernuni-hagen.de)",
			(char*) "[i23]",
			(char*) "[BATCH-ST]"
		);
  return taas__solve(argc,argv,info,solve);
}

/* ============================================================================================================== */
/* == END FILE ================================================================================================== */
/* ============================================================================================================== */
