/* ============================================================================================================== */
/* == BEGIN FILE ================================================================================================ */
/* ============================================================================================================== */
/*
 ============================================================================
 Name        : taas_basics.c
 Author      : Matthias Thimm
 Version     : 2.0
 Copyright   : GPL3
 Description : Basic reasoning functions for AAFs for taas solvers.
 ============================================================================
 */

/**
 * The general solve method for taas solvers, handles easy cases
 * and does the input/output etc.
 */
int taas__solve(int argc,
				char *argv[],
				struct SolverInformation* info,
				void (*doSolve)(struct TaskSpecification*,struct AAF*)){
	// Handle command parameters
	struct TaskSpecification *task = taas__cmd_handle(argc,argv,info);
	// if only solver information was asked, terminate
	if(task != NULL){
	    // read file
	    struct AAF *aaf = (struct AAF*) malloc(sizeof(struct AAF));
		taas__readFile_i23(task->file,aaf);
      	doSolve(task,aaf);
	    taas__aaf_destroy(aaf);
	  }
	taas__solverinformation_destroy(info);
	taas__cmd_destroy(task);
	return 0;
}
/* ============================================================================================================== */
/* == END FILE ================================================================================================== */
/* ============================================================================================================== */
