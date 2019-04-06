
/*
 *
 * fu.c
 * 
   This module was originally written by Paul Kohout and adapted for
   this simulator.

 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fu.h"
#include "pipeline.h"

#define MAX_FIELD_LENGTH 100

const char fu_group_int_name[] = "INT";
const char fu_group_add_name[] =  "ADD";
const char fu_group_mult_name[] =  "MULT";
const char fu_group_div_name[] =  "DIV";


/*
  {{name, fu_group_num, operation, data_type}, sub_table}
*/

const op_t op_table[] = {
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},op_special_table},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},op_fparith_table},
  {{"J",      FU_GROUP_BRANCH, OPERATION_J,    DATA_TYPE_NONE},NULL},
  {{"JAL",    FU_GROUP_BRANCH, OPERATION_JAL,  DATA_TYPE_NONE},NULL},
  {{"BEQZ",   FU_GROUP_BRANCH, OPERATION_BEQZ, DATA_TYPE_NONE},NULL},
  {{"BNEZ",   FU_GROUP_BRANCH, OPERATION_BNEZ, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{"ADDI",   FU_GROUP_INT,    OPERATION_ADD,  DATA_TYPE_NONE},NULL},
  {{"ADDUI",  FU_GROUP_INT,    OPERATION_ADDU, DATA_TYPE_NONE},NULL},
  {{"SUBI",   FU_GROUP_INT,    OPERATION_SUB,  DATA_TYPE_NONE},NULL},
  {{"SUBUI",  FU_GROUP_INT,    OPERATION_SUBU, DATA_TYPE_NONE},NULL},
  {{"ANDI",   FU_GROUP_INT,    OPERATION_AND,  DATA_TYPE_NONE},NULL},
  {{"ORI",    FU_GROUP_INT,    OPERATION_OR,   DATA_TYPE_NONE},NULL},
  {{"XORI",   FU_GROUP_INT,    OPERATION_XOR,  DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{"JR",     FU_GROUP_BRANCH, OPERATION_JR,   DATA_TYPE_NONE},NULL},
  {{"JALR",   FU_GROUP_BRANCH, OPERATION_JALR, DATA_TYPE_NONE},NULL},
  {{"SLLI",   FU_GROUP_INT,    OPERATION_SLL,  DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{"SRLI",   FU_GROUP_INT,    OPERATION_SRL,  DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{"SLTI",   FU_GROUP_INT,    OPERATION_SLT,  DATA_TYPE_NONE},NULL},
  {{"SGTI",   FU_GROUP_INT,    OPERATION_SGT,  DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{"LW",     FU_GROUP_MEM,    OPERATION_LOAD, DATA_TYPE_W},   NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{"L.S",    FU_GROUP_MEM,    OPERATION_LOAD, DATA_TYPE_F},   NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{"SW",     FU_GROUP_MEM,    OPERATION_STORE,DATA_TYPE_W},   NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{"S.S",    FU_GROUP_MEM,    OPERATION_STORE,DATA_TYPE_F},   NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{"SLTUI",  FU_GROUP_INT,    OPERATION_SLTU, DATA_TYPE_NONE},NULL},
  {{"SGTUI",  FU_GROUP_INT,    OPERATION_SGTU, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{"HALT",   FU_GROUP_HALT,   OPERATION_NONE, DATA_TYPE_NONE},NULL}
};

const sub_op_t op_special_table[] = {
  {"NOP",    FU_GROUP_NONE,   OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {"SLL",    FU_GROUP_INT,    OPERATION_SLL,  DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {"SRL",    FU_GROUP_INT,    OPERATION_SRL,  DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {"SLTU",   FU_GROUP_INT,    OPERATION_SLTU, DATA_TYPE_NONE},
  {"SGTU",   FU_GROUP_INT,    OPERATION_SGTU, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {"ADD",    FU_GROUP_INT,    OPERATION_ADD,  DATA_TYPE_NONE},
  {"ADDU",   FU_GROUP_INT,    OPERATION_ADDU, DATA_TYPE_NONE},
  {"SUB",    FU_GROUP_INT,    OPERATION_SUB,  DATA_TYPE_NONE},
  {"SUBU",   FU_GROUP_INT,    OPERATION_SUBU, DATA_TYPE_NONE},
  {"AND",    FU_GROUP_INT,    OPERATION_AND,  DATA_TYPE_NONE},
  {"OR",     FU_GROUP_INT,    OPERATION_OR,   DATA_TYPE_NONE},
  {"XOR",    FU_GROUP_INT,    OPERATION_XOR,  DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {"SLT",    FU_GROUP_INT,    OPERATION_SLT,  DATA_TYPE_NONE},
  {"SGT",    FU_GROUP_INT,    OPERATION_SGT,  DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE}
};

const sub_op_t op_fparith_table[] = {
  {"ADD.S",  FU_GROUP_ADD,    OPERATION_ADD,  DATA_TYPE_F},
  {"SUB.S",  FU_GROUP_ADD,    OPERATION_SUB,  DATA_TYPE_F},
  {"MULT.S", FU_GROUP_MULT,   OPERATION_MULT, DATA_TYPE_F},
  {"DIV.S",  FU_GROUP_DIV,    OPERATION_DIV,  DATA_TYPE_F},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE}
};

void init_operand(operand_t *op){
    op->integer.w=0;
    op->integer.wu=0;
    op->flt=0.0;
}

void init_int_stage(fu_int_stage_t *stage, int num_cycles) {
    stage->num_cycles=num_cycles;
    stage->current_cycle=-1;
    stage->instr=0;
    stage->pc=0;
    stage->dest_reg=-1;
    stage->tgt_reg=-1;
    stage->src_reg=-1;
    stage->op_imm=0;
    stage->op_unsign=0;
    stage->op_int=0;
    init_operand(&(stage->operand1));
    init_operand(&(stage->operand2));
}

void init_fp_stage(fu_fp_stage_t *stage, int num_cycles) {
    stage->num_cycles=num_cycles;
    stage->current_cycle=-1;
    stage->instr=0;
    stage->pc=0;
    stage->dest_reg=-1;
    stage->tgt_reg=-1;
    stage->src_reg=-1;
    stage->op_imm=0;
    stage->op_unsign=0;
    stage->op_int=0;
    init_operand(&(stage->operand1));
    init_operand(&(stage->operand2));
}

void init_int_wb_reg(ex_mem_wb_t *int_wb){
    int_wb->instr=0;
    int_wb->pc=0;
    int_wb->dest_reg=-1;
    int_wb->src_reg=-1;
    int_wb->tgt_reg=-1;
    int_wb->op_imm=0;
    int_wb->op_unsign=0;
    int_wb->op_int=0;
    int_wb->result_written=0;
    init_operand(&(int_wb->operand1));
    init_operand(&(int_wb->operand2));
    init_operand(&(int_wb->result));
}

void init_fp_wb_reg(ex_mem_wb_t *fp_wb){
    fp_wb->instr=0;
    fp_wb->pc=0;
    fp_wb->dest_reg=-1;
    fp_wb->src_reg=-1;
    fp_wb->tgt_reg=-1;
    fp_wb->op_imm=0;
    fp_wb->op_unsign=0;
    fp_wb->op_int=0;
    fp_wb->result_written=0;
    init_operand(&(fp_wb->operand1));
    init_operand(&(fp_wb->operand2));
    init_operand(&(fp_wb->result));
}

void init_id_ex_reg(id_ex_t *id_ex){
    id_ex->instr=0;
    id_ex->pc=0;
    id_ex->dest_reg=-1;
    id_ex->src_reg=-1;
    id_ex->tgt_reg=-1;
    id_ex->op_imm=0;
    id_ex->op_unsign=0;
    id_ex->op_int=0;
    init_operand(&(id_ex->operand1));
    init_operand(&(id_ex->operand2));
}

void init_if_id_reg(if_id_t *if_id){
    if_id->instr=0;
    if_id->pc=0;
}

state_t *
state_create(int *data_count, FILE *bin_file, FILE *fu_file) {
  state_t *state;
  fu_int_stage_t *cur_int_stage;
  fu_fp_stage_t *cur_fp_stage;
  char field[MAX_FIELD_LENGTH];
  char field_fmt[16];
  int num;
  int i;

  state = (state_t *)malloc(sizeof(state_t));
  if(state == NULL) {
    fprintf(stderr, "error: unable to allocate resources\n");
    return NULL;
  }

  memset(state, 0, sizeof(state_t));

  init_int_wb_reg(&(state->int_wb));
  init_fp_wb_reg(&(state->fp_wb));
  init_id_ex_reg(&(state->id_ex));
  init_if_id_reg(&(state->if_id));

  /* read machine-code file into instruction/data memory (starting at address 0) */
  i=0;
  while (!feof(bin_file)) {
    if (fread(&state->mem[i], 1, 1, bin_file) != 0) {
      i++;
    } else if (!feof(bin_file)) {
      fprintf(stderr, "error: cannot read address 0x%X from binary file\n", i);
      return NULL;      
    }
  }
  if (data_count != NULL)
    *data_count = i;

  /* allocate and initialize functional unit linked lists */
  while (!feof(fu_file)) {

    sprintf(field_fmt, " %%%d[^,\n]", MAX_FIELD_LENGTH);
    if (fscanf(fu_file, field_fmt, field) != 1) {
      fprintf(stderr, "error: cannot parse options file\n");
      return NULL;
    }
    i = strlen(field)-1;
    while (i > 0 && (field[i] == ' ' || field[i] == '\t'))
      field[i--] = 0;

    if (strcmp(field, fu_group_int_name) == 0) {
      if (fu_int_read(&state->fu_int_list, fu_file) != 0) {
	return NULL;
      }
    } else if (strcmp(field, fu_group_add_name) == 0) {
      if (fu_fp_read(&state->fu_add_list, fu_file) != 0) {
	return NULL;
      }
    } else if (strcmp(field, fu_group_mult_name) == 0) {
      if (fu_fp_read(&state->fu_mult_list, fu_file) != 0) {
	return NULL;
      }
    } else if (strcmp(field, fu_group_div_name) == 0) {
      if (fu_fp_read(&state->fu_div_list, fu_file) != 0) {
	return NULL;
      }
    } else {
      fprintf(stderr,"error: invalid functional unit group name\n");
      return NULL;
    }
  }
  if (state->fu_int_list == NULL) {
    fprintf(stderr,"error: no %s functional units\n", fu_group_int_name);
    return NULL;
  }
  if (state->fu_add_list == NULL) {
    fprintf(stderr,"error: no %s functional units\n", fu_group_add_name);
    return NULL;
  }
  if (state->fu_mult_list == NULL) {
    fprintf(stderr,"error: no %s functional units\n", fu_group_mult_name);
    return NULL;
  }
  if (state->fu_div_list == NULL) {
    fprintf(stderr,"error: no %s functional units\n", fu_group_div_name);
    return NULL;
  }

  /* initialize wb ports */
  state->int_wb.instr = 0/*NOP*/;
  state->fp_wb.instr = 0/*NOP*/;

  state->fetch_lock = FALSE;
  state->ex_init = FALSE;
  state->wb_init = FALSE;
  return state;
}


/* functions to parse FU file */
int
fu_int_read(fu_int_t **fu_int_list, FILE *file) {
  fu_int_t *fu_int, *fu_temp;
  fu_int_stage_t *stage, *temp;
  char field[MAX_FIELD_LENGTH];
  char field_fmt[16];
  int num;
  int i;

  sprintf(field_fmt, ", %%%d[^,\n]", MAX_FIELD_LENGTH);
  if (fscanf(file, field_fmt, field) != 1) {
    fprintf(stderr, "error: cannot parse options file\n");
    return -1;
  }
  i = strlen(field)-1;
  while (i > 0 && (field[i] == ' ' || field[i] == '\t'))
    field[i--] = 0;

  //printf("%s ",field);

  if (*fu_int_list == NULL) {
    *fu_int_list = (fu_int_t *)malloc(sizeof(fu_int_t));
    fu_int = *fu_int_list;
  } else {
    fu_int = *fu_int_list;
    while (fu_int->next != NULL) fu_int = fu_int->next;
    fu_int->next = (fu_int_t *)malloc(sizeof(fu_int_t));
    fu_int = fu_int->next;
  }
  if (fu_int == NULL) {
    fprintf(stderr, "error: unable to allocate resources\n");
    return -1;
  }

  fu_int->name = (char *)malloc(strlen(field)+1);
  if (fu_int->name == NULL) {
    fprintf(stderr, "error: unable to allocate resources\n");
    return -1;
  }

  fu_int->stage_list = NULL;
  fu_int->next = NULL;
  strcpy(fu_int->name, field);

  stage = NULL;
  while (fscanf(file, ", %d", &num) == 1) {

    if(num <= 0) {
      fprintf(stderr, "error: functional unit '%s' has a stage with an invalid number of cycles\n", fu_int->name);
      return -1;
    }

    //printf("%d ",num);

    stage = (fu_int_stage_t *)malloc(sizeof(fu_int_stage_t));
    if (stage == NULL) {
      fprintf(stderr, "error: unable to allocate resources\n");
      return -1;
    }

    stage->instr=0;
    stage->pc=0;
    stage->dest_reg=-1;
    stage->tgt_reg=-1;
    stage->src_reg=-1;
    stage->op_imm=0;
    stage->op_unsign=0;
    stage->op_int=0;
    init_operand(&(stage->operand1));
    init_operand(&(stage->operand2));

    stage->prev = fu_int->stage_list;
    fu_int->stage_list = stage;
    fu_int->num_stages+=num;
    stage->num_cycles = num;
    stage->current_cycle = -1;
  }

  if (fu_int->stage_list == NULL) {
    fprintf(stderr, "error: functional unit '%s' has no stages\n", fu_int->name);
    return -1;
  }

  fu_temp=fu_int;
  temp=fu_int->stage_list;
  
  //printf("Num Stages: %d",fu_int->num_stages);

  //printf("\n");
  while (fu_temp!=NULL) {
      while (temp!=NULL){

          //printf("Num Cycles:%d\nCurrent Cycle:%d\n",temp->num_cycles,temp->current_cycle);
          //printInstructions(temp->instr);

          if (temp->prev==NULL){
              break;
          }

          temp=temp->prev;

      }
      fu_temp=fu_temp->next;    
  }

  //printf("All %s FUs have been created\n\n",field);
  return 0;
}


int
fu_fp_read(fu_fp_t **fu_fp_list,FILE *file) {
  fu_fp_t *fu_fp, *fu_temp;
  fu_fp_stage_t *stage, *temp;
  char field[MAX_FIELD_LENGTH];
  char field_fmt[16];
  int num;
  int i;

  sprintf(field_fmt,", %%%d[^,\n]",MAX_FIELD_LENGTH);
  if(fscanf(file,field_fmt,field) != 1) {
    fprintf(stderr,"error: cannot parse options file\n");
    return -1;
  }
  i = strlen(field)-1;
  while(i > 0 && (field[i] == ' ' || field[i] == '\t'))
    field[i--] = 0;

  //printf("%s ",field);

  if(*fu_fp_list == NULL) {
    *fu_fp_list = (fu_fp_t *)malloc(sizeof(fu_fp_t));
    fu_fp = *fu_fp_list;
  } else {
    fu_fp = *fu_fp_list;
    while(fu_fp->next != NULL) fu_fp = fu_fp->next;
    fu_fp->next = (fu_fp_t *)malloc(sizeof(fu_fp_t));
    fu_fp = fu_fp->next;
  }
  if(fu_fp == NULL) {
    fprintf(stderr,"error: unable to allocate resources\n");
    return -1;
  }

  fu_fp->name = (char *)malloc(strlen(field)+1);
  if(fu_fp->name == NULL) {
    fprintf(stderr,"error: unable to allocate resources\n");
    return -1;
  }
  fu_fp->stage_list = NULL;
  fu_fp->next = NULL;
  strcpy(fu_fp->name,field);

  stage = NULL;
  while(fscanf(file,", %d",&num) == 1) {

    if(num <= 0) {
      fprintf(stderr,"error: functional unit '%s' has a stage with an invalid number of cycles\n",fu_fp->name);
      return -1;
    }

    //printf("%d ",num);

    stage = (fu_fp_stage_t *)malloc(sizeof(fu_fp_stage_t));
    if(stage == NULL) {
      fprintf(stderr,"error: unable to allocate resources\n");
      return -1;
    }

    stage->instr=0;
    stage->pc=0;
    stage->dest_reg=-1;
    stage->tgt_reg=-1;
    stage->src_reg=-1;
    stage->op_imm=0;
    stage->op_unsign=0;
    stage->op_int=0;
    init_operand(&(stage->operand1));
    init_operand(&(stage->operand2));

    stage->prev = fu_fp->stage_list;
    fu_fp->stage_list = stage;
    fu_fp->num_stages+=num;
    stage->num_cycles = num;
    stage->current_cycle = -1;
  }
  if(fu_fp->stage_list == NULL) {
    fprintf(stderr,"error: functional unit '%s' has no stages\n",fu_fp->name);
    return -1;
  }

  fu_temp=fu_fp;
  temp=fu_fp->stage_list;
  //printf("Num Stages: %d",fu_fp->num_stages);

  //printf("\n");
  while (fu_temp!=NULL) {
      while (temp!=NULL){

          //printf("Num Cycles:%d\nCurrent Cycle:%d\n",temp->num_cycles,temp->current_cycle);
          //printInstructions(temp->instr);

          if (temp->prev==NULL){
              break;
          }

          temp=temp->prev;

      }
      fu_temp=fu_temp->next;    
  }
  //printf("All %s FUs have been created\n\n",field);
  return 0;
}


/* Functions to allocate functional units */
int issue_fu_int(fu_int_t *fu_list, int instr, int dest_reg, int src_reg, int tgt_reg, operand_t op1, operand_t op2, unsigned int pc, int op_imm, int op_int, int op_unsign) {
  fu_int_t *fu;
  fu_int_stage_t *stage;

  fu = fu_list;
  while (fu != NULL) {
    stage = fu->stage_list;
    while (stage->prev != NULL)
      stage = stage->prev;
    if (stage->current_cycle == -1) {
      stage->current_cycle = stage->num_cycles-1;
      stage->instr = instr;
      stage->dest_reg=dest_reg;
      stage->src_reg=src_reg;
      stage->tgt_reg=tgt_reg;
      stage->operand1=op1;
      stage->operand2=op2;
      stage->operand1.integer.w=op1.integer.w;
      stage->operand1.integer.wu=op1.integer.wu;
      stage->operand2.integer.w=op2.integer.w;
      stage->operand2.integer.wu=op2.integer.wu;
      //printf("Op1: %d, Op2: %d\n",stage->operand1.integer.w, stage->operand2.integer.w);
      //printf("Op1: %d, Op2: %d\n",stage->operand1.integer.w, stage->operand2.integer.wu);
      stage->pc=pc;
      stage->op_imm=op_imm;
      stage->op_int=op_int;
      stage->op_unsign=op_unsign;
 
      return 0;
    }
    fu = fu->next;
  }
  return -1;   // structural hazard... stall
}


int issue_fu_fp(fu_fp_t *fu_list, int instr, int dest_reg, int src_reg, int tgt_reg, operand_t op1, operand_t op2, unsigned int pc, int op_imm, int op_int, int op_unsign) {
  fu_fp_t *fu;
  fu_fp_stage_t *stage;

  fu = fu_list;
  while (fu != NULL) {
    stage = fu->stage_list;
    while (stage->prev != NULL)
      stage = stage->prev;
    if (stage->current_cycle == -1) {
      stage->current_cycle = stage->num_cycles-1;
      stage->instr = instr;
      stage->dest_reg=dest_reg;
      stage->src_reg=src_reg;
      stage->tgt_reg=tgt_reg;
      stage->operand1=op1;
      stage->operand2=op2;
      stage->operand1.flt=op1.flt;
      stage->operand2.flt=op2.flt;
      stage->pc=pc;
      stage->op_imm=op_imm;
      stage->op_int=op_int;
      stage->op_unsign=op_unsign;
      return 0;
    }
    fu = fu->next;
  }
  return -1;   // structural hazard... stall
}


/* functions to cycle functional units */
int advance_fu_int(fu_int_t *fu_list, ex_mem_wb_t *int_wb) {
    fu_int_t *fu;
    fu_int_stage_t *stage, *next_stage;
    int i, stage_done;

    fu = fu_list;
    while (fu != NULL) {
        stage = fu->stage_list;
        next_stage = NULL;
        while (stage != NULL) {
            switch (stage->current_cycle) {
	            /* is fu stage free? */
                case -1:
	                break; /* do nothing */
	            /* is fu stage done processing? */
                case 0:
	                if (next_stage == NULL) {                    /* is this the last stage in the fu? */
	                    int_wb->instr = stage->instr;
                        int_wb->pc = stage->pc;
                        int_wb->dest_reg = stage->dest_reg;
                        int_wb->tgt_reg = stage->tgt_reg;
                        int_wb->src_reg = stage->src_reg;
                        int_wb->op_imm = stage->op_imm;
                        int_wb->op_unsign = stage->op_unsign;
                        int_wb->op_int = stage->op_int;
                        int_wb->operand1.integer.wu = stage->operand1.integer.wu;
                        int_wb->operand1.integer.w = stage->operand1.integer.w;
                        int_wb->operand2.integer.wu = stage->operand2.integer.wu;
                        int_wb->operand2.integer.w = stage->operand2.integer.w;
                        init_int_stage(stage, stage->num_cycles);
                        //printf("Op1: %d, Op2: %d\n",stage->operand1.integer.w, stage->operand2.integer.w);
                        //printf("Op1: %d, Op2: %d\n",stage->operand1.integer.w, stage->operand2.integer.wu);
                        //printf("Op1: %d, Op2: %d\n",int_wb->operand1.integer.w, int_wb->operand2.integer.w);
                        //printf("Op1: %d, Op2: %d\n",int_wb->operand1.integer.w, int_wb->operand2.integer.wu);

                        stage_done=1;
                        stage->dest_reg=-1;
                        stage->src_reg=-1;
                        stage->tgt_reg=-1;
	                    stage->current_cycle = -1;
	                } 
                    else {
	                    if (next_stage->current_cycle == -1) {     /* move to next fu stage */
	                        next_stage->instr = stage->instr;
                            next_stage->pc = stage->pc;
                            next_stage->dest_reg = stage->dest_reg;
                            next_stage->tgt_reg = stage->tgt_reg;
                            next_stage->src_reg = stage->src_reg;
                            next_stage->op_imm = stage->op_imm;
                            next_stage->op_unsign = stage->op_unsign;
                            next_stage->op_int = stage->op_int;
                            next_stage->operand1.integer.wu = stage->operand1.integer.wu;
                            next_stage->operand1.integer.w = stage->operand1.integer.w;
                            next_stage->operand2.integer.wu = stage->operand2.integer.wu;
                            next_stage->operand2.integer.w = stage->operand2.integer.w;

	                        next_stage->current_cycle = next_stage->num_cycles-1;
	                        next_stage->instr = stage->instr;
                            init_int_stage(stage,stage->num_cycles);
                            stage->instr=0;
                            stage->dest_reg=-1;
                            stage->src_reg=-1;
                            stage->tgt_reg=-1;
	                        stage->current_cycle = -1;
	                    }
	                }
	                break;

	            /* fu stage is still processing */
                default:
	                stage->current_cycle--;
            }
            next_stage = stage;
            stage = stage->prev;
        }
        fu = fu->next;
    }
    if (stage_done==1)
      return 0;

    return -1;
}

/* functions to cycle functional units */
int advance_fu_fp(fu_fp_t *fu_list, ex_mem_wb_t *fp_wb) {
    fu_fp_t *fu;
    fu_fp_stage_t *stage, *next_stage;
    int i, stage_done;

    fu = fu_list;
    while (fu != NULL) {
        stage = fu->stage_list;
        next_stage = NULL;
        while (stage != NULL) {
            switch (stage->current_cycle) {
	            /* is fu stage free? */
                case -1:
	                break; /* do nothing */
	            /* is fu stage done processing? */
                case 0:
	                if (next_stage == NULL) {                    /* is this the last stage in the fu? */
                        fp_wb->instr = stage->instr;
                        fp_wb->pc = stage->pc;
                        fp_wb->dest_reg = stage->dest_reg;
                        fp_wb->tgt_reg = stage->tgt_reg;
                        fp_wb->src_reg = stage->src_reg;
                        fp_wb->op_imm = stage->op_imm;
                        fp_wb->op_unsign = stage->op_unsign;
                        fp_wb->op_int = stage->op_int;
                        fp_wb->operand1.integer.wu = stage->operand1.integer.wu;
                        fp_wb->operand1.integer.w = stage->operand1.integer.w;
                        fp_wb->operand2.integer.wu = stage->operand2.integer.wu;
                        fp_wb->operand2.integer.w = stage->operand2.integer.w;
                        
                        stage_done=1;
                        init_fp_stage(stage, stage->num_cycles);
                        //stage->dest_reg=-1;
                        //stage->src_reg=-1;
                        //stage->tgt_reg=-1;
	                    //stage->current_cycle = -1;
	                } 
                    else {
	                    if (next_stage->current_cycle == -1) {     /* move to next fu stage */
	                        next_stage->instr = stage->instr;
                            next_stage->pc = stage->pc;
                            next_stage->dest_reg = stage->dest_reg;
                            next_stage->tgt_reg = stage->tgt_reg;
                            next_stage->src_reg = stage->src_reg;
                            next_stage->op_imm = stage->op_imm;
                            next_stage->op_unsign = stage->op_unsign;
                            next_stage->op_int = stage->op_int;
                            next_stage->operand1.integer.wu = stage->operand1.integer.wu;
                            next_stage->operand1.integer.w = stage->operand1.integer.w;
                            next_stage->operand2.integer.wu = stage->operand2.integer.wu;
                            next_stage->operand2.integer.w = stage->operand2.integer.w;
	                        next_stage->current_cycle = next_stage->num_cycles-1;
	                        next_stage->instr = stage->instr;
                            init_fp_stage(stage, stage->num_cycles);
	                        //stage->current_cycle = -1;
	                    }
	                }
	                break;

	            /* fu stage is still processing */
                default:
	                stage->current_cycle--;
            }
            next_stage = stage;
            stage = stage->prev;
        }
        fu = fu->next;
    }
    if (stage_done==1)
      return 0;

    return -1;
}

int
fu_int_done(fu_int_t *fu_list)
{
  fu_int_t *fu;
  fu_int_stage_t *stage;

  fu = fu_list;
  while (fu != NULL) {
    stage = fu->stage_list;
    while (stage != NULL) {
      if (stage->current_cycle != -1)
	return FALSE;
      stage = stage->prev;
    }
    fu = fu->next;
  }

  return TRUE;
}


int
fu_fp_done(fu_fp_t *fu_list)
{
  fu_fp_t *fu;
  fu_fp_stage_t *stage;

  fu = fu_list;
  while (fu != NULL) {
    stage = fu->stage_list;
    while (stage != NULL) {
      if (stage->current_cycle != -1)
	return FALSE;
      stage = stage->prev;
    }
    fu = fu->next;
  }

  return TRUE;
}


/* decode an instruction */
const op_info_t *
decode_instr(int instr, int *use_imm) {
  const op_info_t *op_info;

  if (op_table[FIELD_OPCODE(instr)].sub_table == NULL) {
    op_info = &op_table[FIELD_OPCODE(instr)].info;
    *use_imm = 1;
  } else {
    op_info = &op_table[FIELD_OPCODE(instr)].sub_table[FIELD_FUNC(instr)].info;
    *use_imm = 0;
  }
  return op_info;
}


/* perform an instruction */
void perform_operation(int instr, unsigned long pc, operand_t operand1,
		  operand_t operand2, operand_t *res_ptr)
{
  const op_info_t *op_info;
  int use_imm,fu,op;
  operand_t result;

  op_info = decode_instr(instr, &use_imm);
  fu=op_info->fu_group_num;
  op = op_info->operation;

  switch(fu) {
      case FU_GROUP_INT:
          switch(op) {
              case OPERATION_ADD:
                  result.integer.w = operand1.integer.w + operand2.integer.w;
                  //printf("ADD: %d + %d = %d\n",operand1.integer.w, operand2.integer.w, result.integer.w);
                  res_ptr->integer.w=result.integer.w;
                  break;
              case OPERATION_ADDU:
                  result.integer.wu = operand1.integer.wu + operand2.integer.wu;
                  res_ptr->integer.wu=result.integer.wu;
                  break;
              case OPERATION_SUB:
                  result.integer.w = operand1.integer.w - operand2.integer.w;
                  res_ptr->integer.w=result.integer.w;
                  break;
              case OPERATION_SUBU:
                  if (operand1.integer.wu>operand2.integer.wu){
                    result.integer.wu = 0;
                  }
                  else {
                    result.integer.wu = operand1.integer.wu - operand2.integer.wu;
                  }
                  res_ptr->integer.wu=result.integer.wu;
                  break;
              case OPERATION_AND:
                  result.integer.w = operand1.integer.w & operand2.integer.w;
                  res_ptr->integer.w=result.integer.w;
                  break;
              case OPERATION_OR:
                  result.integer.w = operand1.integer.w | operand2.integer.w;
                  res_ptr->integer.w=result.integer.w;
                  break;
              case OPERATION_XOR:
                  result.integer.w = operand1.integer.w ^ operand2.integer.w;
                  res_ptr->integer.w=result.integer.w;
                  break;
              case OPERATION_SLT:
                  result.integer.w = (operand1.integer.w<operand2.integer.w)?1:0;
                  res_ptr->integer.w=result.integer.w;
                  break;
              case OPERATION_SLTU:
                  result.integer.wu = (operand1.integer.wu<operand2.integer.wu)?1:0;
                  res_ptr->integer.wu=result.integer.wu;
                  break;
              case OPERATION_SGT:
                  result.integer.w = (operand1.integer.w>operand2.integer.w)?1:0;
                  res_ptr->integer.w=result.integer.w;
                  break;
              case OPERATION_SGTU:
                  result.integer.wu = (operand1.integer.wu>operand2.integer.wu)?1:0;
                  res_ptr->integer.wu=result.integer.wu;
                  break;
              case OPERATION_SLL:
                  result.integer.w = operand1.integer.w << operand2.integer.w;
                  res_ptr->integer.w=result.integer.w;
                  break;
              case OPERATION_SRL:
                  result.integer.w = operand1.integer.w >> operand2.integer.w;
                  res_ptr->integer.w=result.integer.w;
                  break;
          }
          break;

      case FU_GROUP_ADD:
          switch (op){
              case OPERATION_ADD:
                  result.flt = operand1.flt + operand2.flt;
                  break;
              case OPERATION_SUB:
                  result.flt = operand1.flt - operand2.flt;
                  break;
          }
          res_ptr->flt=result.flt;
          break;
      case FU_GROUP_MULT:
          result.flt = operand1.flt * operand2.flt;
          res_ptr->flt=result.flt;
          break;
      case FU_GROUP_DIV:
          result.flt = operand1.flt / operand2.flt;
          res_ptr->flt=result.flt;
          break;

      case FU_GROUP_MEM:
          result.integer.w = operand1.integer.w + operand2.integer.w;            
          //printf("%d + %d = %d\n",operand1.integer.w, operand2.integer.w, result.integer.w);
          res_ptr->integer.w=result.integer.w;
          break;

      case FU_GROUP_BRANCH:
      case FU_GROUP_HALT:
      case FU_GROUP_NONE:
          break;

      case FU_GROUP_INVALID:
          fprintf(stderr, "error: invalid opcode (instr = %.8X)\n", instr);
          break;
  }

}

void printInstructions(int instr) {
  const op_info_t *op_info;
  printf("Instruction: ");
  if(op_table[FIELD_OPCODE(instr)].sub_table == NULL) {
    op_info = &op_table[FIELD_OPCODE(instr)].info;
    
    if(op_info->name == NULL) {
      printf("0x%.8X",instr);
    }
    else {
        switch(op_info->fu_group_num) {
            case FU_GROUP_INT:
                printf("%s R%d R%d #%d",op_info->name,FIELD_R2(instr),FIELD_R1(instr),FIELD_IMM(instr));
                break;
            case FU_GROUP_MEM:
                switch(op_info->data_type) {
                    case DATA_TYPE_W:
                        printf("%s R%d (%d)R%d",op_info->name,FIELD_R2(instr),FIELD_IMM(instr),FIELD_R1(instr));
                        break;
                case DATA_TYPE_F:
                    printf("%s F%d (%d)R%d",op_info->name,FIELD_R2(instr),FIELD_IMM(instr),FIELD_R1(instr));
                    break;
                }
                break;
            case FU_GROUP_BRANCH:
                switch(op_info->operation) {

                    case OPERATION_JAL:
                    case OPERATION_J:
                        printf("%s #%d",op_info->name,FIELD_OFFSET(instr));
                        break;

                    case OPERATION_JALR:
                    case OPERATION_JR:
                        printf("%s R%d",op_info->name,FIELD_R1(instr));
                        break;

                    case OPERATION_BEQZ:
                    case OPERATION_BNEZ:
                        printf("%s R%d #%d",op_info->name,FIELD_R1(instr),FIELD_IMM(instr));
                        break;

                }
                break;
            default:
                printf("%s",op_info->name);
            }
        }
  } 
  else {
      op_info = &op_table[FIELD_OPCODE(instr)].sub_table[FIELD_FUNC(instr)].info;
      if(op_info->name == NULL) {
          printf("0x%.8X",instr);
      }
      else {
          switch(op_info->fu_group_num) {
              case FU_GROUP_INT:
                  printf("%s R%d R%d R%d",op_info->name,FIELD_R3(instr),FIELD_R1(instr),FIELD_R2(instr));
                  break;
              case FU_GROUP_ADD:
              case FU_GROUP_MULT:
              case FU_GROUP_DIV:
                  printf("%s F%d F%d F%d",op_info->name,FIELD_R3(instr),FIELD_R1(instr),FIELD_R2(instr));
                  break;
              default:
                  printf("%s",op_info->name);
          }
      }
  }
  printf("\n");
}
