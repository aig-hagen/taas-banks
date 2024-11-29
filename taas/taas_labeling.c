/* ============================================================================================================== */
/* == BEGIN FILE ================================================================================================ */
/* ============================================================================================================== */
/*
 ============================================================================
 Name        : taas_labeling.c
 Author      : Matthias Thimm
 Version     : 2.0
 Copyright   : GPL3
 Description : Labeling struct and functions for taas solvers.
 ============================================================================
 */

#define LAB_IN 1
#define LAB_OUT 2
#define LAB_UNDEC 4
#define LAB_UNLABELED -1

/**
 * Struct for (partial) labelings
 * Arguments not mentioned in either in or out
 * are either undecided or not included.
 */
struct Labeling{
  // Arguments that are in
  struct BitSet *in;
  // Arguments that are out
  struct BitSet *out;
  // whether this is a two-valued labeling
  // (just values IN and OUT); in this case
  // the bitset "out" is ignored
  char twoValued;
};

/*
 * Initialises a labeling.
 */
void taas__lab_init(struct Labeling* lab, char twoValued){
  lab->in = (struct BitSet*) malloc(sizeof(struct BitSet));
  lab->out = (struct BitSet*) malloc(sizeof(struct BitSet));
  lab->twoValued = twoValued;
}

/**
 * Randomly sets the given labeling
 */
void taas__lab_randomize(struct Labeling* lab){
  bitset__randomize(lab->in);
  if(!lab->twoValued){
    bitset__randomize(lab->out);
    for(int i = 0; i < lab->in->num_elements; i++)
      lab->out->data[i] = lab->out->data[i] & ~lab->in->data[i];
  }
}

/**
 * Returns the label of the given argument.
 */
int taas__lab_get_label(struct Labeling* lab, int arg){
  if(lab->twoValued){
    if(bitset__get(lab->in,arg-1))
      return LAB_IN;
    else return LAB_OUT;
  }
  if(bitset__get(lab->in,arg-1) && !bitset__get(lab->out,arg-1))
    return LAB_IN;
  if(!bitset__get(lab->in,arg-1) && bitset__get(lab->out,arg-1))
    return LAB_OUT;
  if(!bitset__get(lab->in,arg-1) && !bitset__get(lab->out,arg-1))
    return LAB_UNDEC;
  return LAB_UNLABELED;
}

/**
 * Sets the label of the given argument.
 */
void taas__lab_set_label(struct Labeling* lab, int arg, int label){
  if(label == LAB_IN){
      bitset__set(lab->in,arg-1);
      bitset__unset(lab->out,arg-1);
      return;
  }
  if(label == LAB_OUT){
    bitset__unset(lab->in,arg-1);
    bitset__set(lab->out,arg-1);
    return;
  }
  if(label == LAB_UNDEC){
    bitset__unset(lab->in,arg-1);
    bitset__unset(lab->out,arg-1);
    return;
  }
  bitset__set(lab->in,arg-1);
  bitset__set(lab->out,arg-1);
  return;
}

/*
 * Destroys a labeling
 */
void taas__lab_destroy(struct Labeling* lab){
  bitset__destroy(lab->in);
  if(!lab->twoValued)
    bitset__destroy(lab->out);
  else free(lab->out);
  free(lab);
}

 /* ============================================================================================================== */
 /* == END FILE ================================================================================================== */
 /* ============================================================================================================== */
