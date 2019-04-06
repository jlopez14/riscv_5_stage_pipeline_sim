
/*
 *
 * pipeline.c
 *
 */


#include <stdlib.h> 
#include <string.h>
#include "fu.h" 
#include "pipeline.h"

/* Convert a 4-byte sequence into a 32-bit integer */ 
int bytes_to_int(char *addr, unsigned long arr_offst){
    int byte_pos,result;
    unsigned char cur_byte;
    unsigned int shift;
    unsigned int byte_pr;
    
    for (byte_pos=3;byte_pos>=0;byte_pos--){
        /* Left shift result by 8 bits to make room for the next byte */
        shift=result<<8;

        /* Grab next byte from memory */
        cur_byte=addr[byte_pos+arr_offst];

        /* Add it onto the result */
        result=shift+cur_byte;
    }
    return result;
}

/* Convert a 32-bit integer into 4-byte sequence */
void int_to_bytes(unsigned char *result, int addr, unsigned long arr_offst){
    int byte_pos;
    unsigned char byte_t;
    //printf("Converting %d to hex\n",addr);
    for (byte_pos=3;byte_pos>=0;byte_pos--){
        byte_t=( (addr>>(8*byte_pos)) & (0xFF) );
        //printf("Sending byte %x to index: %d\n",byte_t,byte_pos+arr_offst);
        result[byte_pos+arr_offst] = byte_t;

    }
}

float bytes_to_flt(unsigned char * src, unsigned long arr_offset){
  union {
    float flt;
    unsigned char bytes[4];
  } conv;
  memcpy(conv.bytes, src+arr_offset, 4);
  return conv.flt;
}

void flt_to_bytes(unsigned char *result, float addr, unsigned long arr_offset){ 
  union {
    float flt;
    unsigned char bytes[4];
  } conv;
  conv.flt = addr;
  memcpy(result+arr_offset, conv.bytes, 4);
}

int int_reg_busy(int reg, fu_int_t *int_list, int data_t) {
    fu_int_t *fu_temp;
    fu_int_stage_t *temp;

    fu_temp=int_list;
    temp=fu_temp->stage_list;

    while (fu_temp!=NULL) {
      while (temp!=NULL){

          if (temp->dest_reg==reg) {
              if (reg!=0 || data_t==DATA_TYPE_F){              
                //printInstructions(temp->instr);
                //printf("INT REG: R%d in use\n",temp->dest_reg);
                return 1;
              }
          }
          if (temp->prev==NULL){
              break;
          }

          temp=temp->prev;

      }
      fu_temp=fu_temp->next;
    }
    return 0;
}

int fp_reg_busy(int reg, fu_fp_t *fp_list) {
    fu_fp_t *fu_temp;
    fu_fp_stage_t *temp;

    fu_temp=fp_list;
    temp=fu_temp->stage_list;

    while (fu_temp!=NULL) {
      while (temp!=NULL){

          if (temp->dest_reg==reg && temp->dest_reg>=0) {
              //printf("FP REG: F%d in use\n",temp->dest_reg);
              return 1;
          }

          if (temp->prev==NULL){
              break;
          }

          temp=temp->prev;

      }
      fu_temp=fu_temp->next;
    }

    return 0;
}

int check_latency(int reg, fu_fp_t *fp_list, fu_fp_t *fp_late) {
    fu_fp_t *fu_temp;
    fu_fp_stage_t *temp;
    
    fu_temp=fp_late;
    temp=fu_temp->stage_list;
    int stage_ctr=fu_temp->num_stages-1;

/*
    while (temp->prev != NULL)
      temp = temp->prev;
    if (temp->current_cycle == -1) {
      //printf("Current Cycle: %d, No. of Cycles: %d\n",temp->current_cycle,temp->num_cycles-1);
      return 0;
    }
*/

    while (fu_temp!=NULL) {
        while (temp!=NULL){
                  
            if (temp->dest_reg==reg && temp->dest_reg>=0 && stage_ctr>fp_list->num_stages) {
              //printf("FP REG: F%d in use\n",temp->dest_reg);
                return 1;
            }
            
            if (temp->prev==NULL){
                break;
            }
            stage_ctr--;
            temp=temp->prev;

        }
        fu_temp=fu_temp->next;
    }

    return 0;
}

int check_int_latency(int reg, fu_int_t *int_list, fu_int_t *int_late) {
    fu_int_t *fu_temp;
    fu_int_stage_t *temp;
    
    fu_temp=int_late;
    temp=fu_temp->stage_list;
    int stage_ctr=fu_temp->num_stages;

    while (temp->prev != NULL) {
      temp = temp->prev; 
      stage_ctr-=1;
    }
    if (temp->current_cycle == -1) {
      //printf("Stage %d - Current Cycle: %d, No. of Cycles: %d\n",stage_ctr,temp->current_cycle,temp->num_cycles-1);
      return 0;
    }

/*    while (fu_temp!=NULL) {
      while (temp!=NULL){
                  
          if (temp->dest_reg==reg && temp->dest_reg>=0 && temp->current_cycle>fp_list->num_cycles) {
              //printf("FP REG: F%d in use\n",temp->dest_reg);
              return 1;
          }

          if (temp->prev==NULL){
              break;
          }

          temp=temp->prev;

      }
      fu_temp=fu_temp->next;
    }

    return 0;
*/
}

void writeback(state_t *state, int *num_insn) {
    //printf("\nWB_INIT=%d\n",state->wb_init);
    int fu_num, instr, use_imm, op, i, data_t;
    char store[4];         

    if (state->wb_init==1) {
        int int_end;
        int fp_end;
        
        
        if ((fu_int_done(state->fu_int_list)==TRUE) && (state->int_wb.instr==0)){
            int_end=1;
        }    
          
        if ((fu_fp_done(state->fu_add_list)==TRUE) && (fu_fp_done(state->fu_mult_list)==TRUE) && (fu_fp_done(state->fu_div_list)==TRUE) && (state->fp_wb.instr==0)){
            fp_end=1;
        }

        if (int_end==1 && fp_end==1){
            //printf("Ending Simulation...\n");
            exit(0);
        }
        //printf("Instr: %d, Result_Written: %d\n",state->int_wb.instr, state->int_wb.result_written);
        instr=state->int_wb.instr;
        const op_info_t *instr_info=decode_instr(instr,&use_imm);
        op=instr_info->operation;
        data_t=instr_info->data_type;

        if (state->int_wb.instr!=0 && state->int_wb.result_written==1){
            //printf("New INT Instruction has reached WB\n");
            //printf("Result is now being written to INT RF\n");
            //printf("Signed=%d\n",state->int_wb.op_unsign);
            switch(op){
                case OPERATION_LOAD:
                    switch (data_t){
                        case DATA_TYPE_W:
                            state->int_wb.result.integer.w=bytes_to_int(state->mem,state->int_wb.result.integer.w);
                            //printf("%d -> R%d\n",state->int_wb.result.integer.w,state->int_wb.dest_reg);
                            state->rf_int.reg_int[state->int_wb.dest_reg].w=state->int_wb.result.integer.w;
                            break;
                        case DATA_TYPE_F:
                            state->int_wb.result.flt=bytes_to_flt(state->mem,state->int_wb.result.integer.w);
                            //printf("%f -> F%d\n",state->int_wb.result.flt,state->int_wb.dest_reg);
                            state->rf_fp.reg_fp[state->int_wb.dest_reg]=state->int_wb.result.flt;
                            break;
                    }
                    break;
                case OPERATION_STORE:
                    switch (data_t){
                        case DATA_TYPE_W:
                            int_to_bytes(state->mem, state->rf_int.reg_int[state->int_wb.dest_reg].w, state->int_wb.result.integer.w);
                            break;
                        case DATA_TYPE_F:
                            flt_to_bytes(state->mem, state->rf_fp.reg_fp[state->int_wb.dest_reg], state->int_wb.result.integer.w);
                    }
                    break;
                default:
                    switch(state->int_wb.op_unsign){
                        case 1:
                            //printf("%d -> R%d\n",state->int_wb.result.integer.wu,state->int_wb.dest_reg);
                            state->rf_int.reg_int[state->int_wb.dest_reg].wu=state->int_wb.result.integer.wu;
                            break;
                        case 0:
                            //printf("%d -> R%d\n",state->int_wb.result.integer.w,state->int_wb.dest_reg);
                            state->rf_int.reg_int[state->int_wb.dest_reg].w=state->int_wb.result.integer.w;
                            break;
                        default:
                            //printf("Unknown signedness... %d\n",state->int_wb.op_unsign);
                            break;                
                    }
                    break;
            }
            //printf("Write is Complete.\n");
            state->int_wb.result_written=0;
            state->int_wb.instr=0;
            state->int_wb.dest_reg=-1;
            state->int_wb.src_reg=-1;
            state->int_wb.tgt_reg=-1;

            if (state->fetch_lock==1){
                state->fetch_lock=0;
            }

        }

        if (state->fp_wb.result_written==1 && state->fp_wb.instr!=0){
            //printf("New FP instruction has reached WB\n");
            //printf("Result is now being written to FP RF\n");
            //printf("%f -> F%d\n",state->fp_wb.result.flt,state->fp_wb.dest_reg);
            state->rf_fp.reg_fp[state->fp_wb.dest_reg]=state->fp_wb.result.flt;
            state->fp_wb.result_written=0;
            state->fp_wb.instr=0;
            state->fp_wb.dest_reg=-1;
            state->fp_wb.src_reg=-1;
            state->fp_wb.tgt_reg=-1;
            if (state->fetch_lock==1){
                state->fetch_lock=0;
            }
        }
        //printf("Fetch Lock: %d\n",state->fetch_lock);
        *num_insn=*num_insn+1;
    }
}

void execute(state_t *state) {
    if (state->ex_init==TRUE){
        int fu_num, instr, use_imm, int_stat, add_stat, mult_stat, div_stat, i, op;
        int_stat=advance_fu_int(state->fu_int_list, &(state->int_wb));
        add_stat=advance_fu_fp(state->fu_add_list, &(state->fp_wb));
        mult_stat=advance_fu_fp(state->fu_mult_list, &(state->fp_wb));
        div_stat=advance_fu_fp(state->fu_div_list, &(state->fp_wb));


        if (int_stat==0){
            perform_operation(state->int_wb.instr,state->int_wb.pc,state->int_wb.operand1,state->int_wb.operand2,&(state->int_wb.result));
            state->int_wb.result_written=1;
            state->wb_init=1;
        }

        if (decode_instr(state->if_id.instr, &use_imm)->fu_group_num==FU_GROUP_BRANCH){
            state->fetch_lock=FALSE;
        }
                
        if (add_stat==0 || mult_stat==0 || div_stat==0){
            perform_operation(state->fp_wb.instr,state->fp_wb.pc,state->fp_wb.operand1,state->fp_wb.operand2,&(state->fp_wb.result));
            state->fp_wb.result_written=1;
            state->wb_init=1;
        }
    }
}


int decode(state_t *state) {
    int use_imm=0, instr=0, latency=0, op_int=0, op_unsign=0, reg_haz=0;
    int struct_haz=0, data_haz=0;
    char* reg_haz_t="a";

    instr=state->if_id.instr;
    fu_fp_t *fp_temp, *fp_late1, *fp_late2;

    int src_reg=-2;
    int dest_reg=-2;
    int tgt_reg=-2;
    operand_t operand1,operand2;

    const op_info_t *instr_info=decode_instr(instr,&use_imm);
    const int op=instr_info->operation;
    const int data_t=instr_info->data_type;
    const int fu_num=instr_info->fu_group_num;
   
    //printf("\nOp:%d, fu_num:%d\n",instr_info->operation,instr_info->fu_group_num);

    switch (fu_num) {
        case FU_GROUP_INT:
            latency=state->fu_int_list->num_stages;
            op_int=1;
            switch (op) {
                case OPERATION_ADDU:
                case OPERATION_SUBU:
                case OPERATION_SLTU:
                case OPERATION_SGTU:
                    op_unsign=1;
                    break;
                default:
                    op_unsign=0;
                    break;
            }   
            switch(use_imm) {
                case FALSE:
                    dest_reg=FIELD_R3(instr);
                    tgt_reg=FIELD_R2(instr);
                    src_reg=FIELD_R1(instr);

                    if (src_reg!=0 && src_reg==state->int_wb.dest_reg && state->int_wb.result_written!=0){
                        //printf("Register in Writeback: R%d\n",state->int_wb.dest_reg);
                        data_haz=1;
                        reg_haz=src_reg;
                    }
                    if (src_reg!=0 && tgt_reg==state->int_wb.dest_reg && state->int_wb.result_written!=0){
                        data_haz=1;
                        reg_haz=tgt_reg;
                    }
                    if (int_reg_busy(src_reg,state->fu_int_list, data_t)==1){
                        data_haz=1;
                        reg_haz=src_reg;
                    }
                    if (int_reg_busy(tgt_reg,state->fu_int_list, data_t)==1){
                        data_haz=1;
                        reg_haz=tgt_reg;
                    }
                    break;

                case TRUE:
                    dest_reg=FIELD_R2(instr);
                    src_reg=FIELD_R1(instr);
                    if (src_reg!=0 && src_reg==state->int_wb.dest_reg && state->int_wb.result_written!=0){
                        data_haz=1;
                        reg_haz=src_reg;
                    }
                    if (int_reg_busy(src_reg,state->fu_int_list, data_t)==1){
                        data_haz=1;
                        reg_haz=src_reg;
                    }
                    break;
            }
            break;

        case FU_GROUP_MEM:
            latency=state->fu_int_list->num_stages;
            op_int=1;

            dest_reg=FIELD_R2(instr);
            src_reg=FIELD_R1(instr);

            /* RAW dest(instr1)->src(instr2) */
            if (src_reg==state->int_wb.dest_reg && state->int_wb.result_written!=0){
                if (src_reg!=0 || data_t==DATA_TYPE_F){
                    data_haz=1;
                    reg_haz=src_reg;
                }
            }
            if (dest_reg==state->int_wb.dest_reg && op==OPERATION_STORE && state->int_wb.result_written!=0) {
                if (dest_reg!=0 || data_t==DATA_TYPE_F){
                    data_haz=1;
                    reg_haz=src_reg;
                }

            }
            if (int_reg_busy(src_reg,state->fu_int_list, data_t)==1){
                data_haz=1;
                reg_haz=src_reg;

            }
            if (int_reg_busy(dest_reg,state->fu_int_list, data_t)==1 && op==OPERATION_STORE){
                data_haz=1;
                reg_haz=src_reg;
            }
            break;
        
        case FU_GROUP_BRANCH:
            latency=state->fu_int_list->num_stages;
            op_int=1;
            /* FU Int unavailable */
            src_reg=FIELD_R1(instr);

            /* RAW dest(instr1)->src(instr2) */
            if (src_reg==state->int_wb.dest_reg && state->int_wb.result_written!=0){
                data_haz=1;
                reg_haz=src_reg;
            }

            if (int_reg_busy(src_reg,state->fu_int_list, data_t)==1){
                data_haz=1;
                reg_haz=src_reg;
            }
            break;

        case FU_GROUP_ADD:
            fp_temp=state->fu_add_list;
            fp_late1=state->fu_mult_list;
            fp_late2=state->fu_div_list;
            latency=state->fu_add_list->num_stages;
            break;

        case FU_GROUP_MULT:
            fp_temp=state->fu_mult_list;
            fp_late1=state->fu_add_list;
            fp_late2=state->fu_div_list;
            latency=state->fu_mult_list->num_stages;
            break;

        case FU_GROUP_DIV:
            fp_temp=state->fu_div_list;
            fp_late1=state->fu_mult_list;
            fp_late2=state->fu_add_list;
            latency=state->fu_div_list->num_stages;
            break;

        case FU_GROUP_HALT:
            state->fetch_lock=TRUE;
            return fu_num;
        
        case FU_GROUP_INVALID:
        case FU_GROUP_NONE:
            //state->id_ex.instr=0;
            return fu_num;
    }

    if (fu_num==FU_GROUP_ADD || fu_num==FU_GROUP_MULT || fu_num==FU_GROUP_DIV){
        dest_reg=FIELD_R3(instr);
        tgt_reg=FIELD_R2(instr);
        src_reg=FIELD_R1(instr);

        /* RAW dest(instr1)->src(instr2) */
        if (src_reg==state->fp_wb.dest_reg && state->fp_wb.result_written!=0){
            data_haz=1;
            reg_haz=src_reg;
        }
        /* RAW dest(instr1)->tgt(instr2) */
        if (tgt_reg==state->fp_wb.dest_reg && state->fp_wb.result_written!=0){
            data_haz=1;
            reg_haz=tgt_reg;
        }
            /* WAW dest(instr1)->dest(instr2) */
        if (dest_reg==state->fp_wb.dest_reg && state->fp_wb.result_written!=0){
            data_haz=1;
            reg_haz=dest_reg;
        }

        if (fp_reg_busy(src_reg,fp_temp)==1){
            data_haz=1;
            reg_haz=src_reg;
        }

        if (fp_reg_busy(tgt_reg,fp_temp)==1){
            data_haz=1;
            reg_haz=tgt_reg;
        }

        if (fp_reg_busy(src_reg,fp_late1)==1){
            data_haz=1;
            reg_haz=src_reg;
        }

        if (fp_reg_busy(tgt_reg,fp_late1)==1){
            data_haz=1;
            reg_haz=tgt_reg;
        }

        if (fp_reg_busy(src_reg,fp_late2)==1){
            data_haz=1;
            reg_haz=src_reg;
        }

        if (fp_reg_busy(tgt_reg,fp_late2)==1){
            data_haz=1;
            reg_haz=tgt_reg;
        }

        const op_info_t *wb_info=decode_instr(state->int_wb.instr, &use_imm);

        /* RAW dest(instr1)->src(instr2) */
        if (src_reg==state->int_wb.dest_reg && state->int_wb.result_written!=0 && wb_info->data_type==DATA_TYPE_F){
            
            data_haz=1;
            reg_haz=src_reg;
        }
        /* RAW dest(instr1)->tgt(instr2) */
        if (tgt_reg==state->int_wb.dest_reg && state->int_wb.result_written!=0 && wb_info->data_type==DATA_TYPE_F){
            data_haz=1;
            reg_haz=tgt_reg;
        }

        if (int_reg_busy(src_reg,state->fu_int_list, DATA_TYPE_F)==1){
            data_haz=1;
            reg_haz=src_reg;
        }
        if (int_reg_busy(tgt_reg,state->fu_int_list, DATA_TYPE_F)==1){
            data_haz=1;
            reg_haz=tgt_reg;
        }
            
    }

    /* If data hazard is detected, don't set any operands, just move on. */
    switch(op_int){
        case 0:
            reg_haz_t="F";
            break;
        case 1:
            reg_haz_t="R";
            break;
    }
    if (data_haz==1) {
        //printf("\nData Hazard on Reg: %s%d\n",reg_haz_t,reg_haz);
        state->fetch_lock=1;
        return -4;
    }

    //printf("Instr: %d\n",instr);
    switch (fu_num) {
        case FU_GROUP_INT:
        case FU_GROUP_MEM:

            if (use_imm==1 || fu_num==FU_GROUP_MEM) {
                switch(op_unsign){
                    case FALSE:
                        operand1.integer.w=state->rf_int.reg_int[src_reg].w;
//                        //printf("Op1: %d, Op2: %d\n",operand1.integer.w, operand2.integer.w);
                        operand2.integer.w=FIELD_IMM(instr);
                        break;
                    case TRUE:
                        operand1.integer.wu=state->rf_int.reg_int[src_reg].wu;
                        operand2.integer.wu=FIELD_IMMU(instr);
                        break;
                }
            }
            else {
                switch(op_unsign){
                    case FALSE:
                        operand1.integer.w=state->rf_int.reg_int[src_reg].w;
                        operand2.integer.w=state->rf_int.reg_int[tgt_reg].w;
                        break;
                    case TRUE:
                        operand1.integer.wu=state->rf_int.reg_int[src_reg].wu;
                        operand2.integer.wu=state->rf_int.reg_int[tgt_reg].wu;
                        break;
                    }
            }

            //printf("INT Latency: %d\n",check_int_latency(dest_reg,state->fu_int_list,state->fu_int_list));
            
            if (issue_fu_int(state->fu_int_list, instr, dest_reg, src_reg, tgt_reg, operand1, operand2, state->if_id.pc, use_imm, op_int, op_unsign)==-1) {
                //printf("\nStructural Hazard for INT!\n");
                state->fetch_lock=TRUE;
                return -5;
            }
            state->ex_init=1;
            break;
        
        case FU_GROUP_ADD:
        case FU_GROUP_MULT:
        case FU_GROUP_DIV:
            if (fp_temp!=NULL){
                operand1.flt=state->rf_fp.reg_fp[src_reg];
                operand2.flt=state->rf_fp.reg_fp[tgt_reg];

                if (check_latency(dest_reg, fp_temp, fp_late1)==1){
                    return -6;
                }
                if (check_latency(dest_reg, fp_temp, fp_late2)==1){
                    return -6;
                }

                if (issue_fu_fp(fp_temp, instr, dest_reg, src_reg, tgt_reg, operand1, operand2, state->if_id.pc, use_imm, op_int, op_unsign)==-1) {
                    //printf("\nStructural Hazard for %s!\n",fp_temp->name);
                    state->fetch_lock=TRUE;
                    return -5;
                }
                state->ex_init=1;
            }
            break;

        case FU_GROUP_BRANCH:
    
            operand1.integer.wu=state->rf_int.reg_int[src_reg].wu;
            operand2.integer.wu=FIELD_IMMU(instr);
            if (issue_fu_int(state->fu_int_list, instr, dest_reg, src_reg, tgt_reg, operand1, operand2, state->if_id.pc, use_imm, op_int, op_unsign)==-1) {
                //printf("\nStructural Hazard for INT!\n");
                state->fetch_lock=TRUE;
                return -5;
            }

            switch(op){
                case OPERATION_J:
                    state->pc = state->pc + FIELD_OFFSET(instr);
                    break;

                case OPERATION_JAL:
                    state->rf_int.reg_int[31].wu=state->if_id.pc;
                    state->pc = state->pc + FIELD_OFFSET(instr);
                    break;

                case OPERATION_JR:
                    state->pc = operand1.integer.wu;
                    break;

                case OPERATION_JALR:
                    state->rf_int.reg_int[31].wu=state->id_ex.pc;
                    state->pc = operand1.integer.wu;
                    break;

                case OPERATION_BEQZ:
                    state->pc = (operand1.integer.wu==0)?(state->pc + operand2.integer.wu):state->pc;
                    break;

                case OPERATION_BNEZ:
                    state->pc = (operand1.integer.wu!=0)?(state->pc + operand2.integer.wu):state->pc;
                    break;
            }
            /* Stall for control insruction */
            state->fetch_lock=TRUE;
            state->ex_init=1;

            break;
    }

    return 0;
    
}


void fetch(state_t *state, int data_haz) {
    int use_imm;

    if (state->ex_init){
        if (decode_instr(state->if_id.instr, &use_imm)->fu_group_num==FU_GROUP_BRANCH && data_haz>-4){
            state->fetch_lock=FALSE;
        }
    }

    if (state->fetch_lock==0){
        /*Get the instruction from Main Memory using the program counter as an index*/
        state->if_id.instr=bytes_to_int(state->mem,state->pc);

        /*Store the original program counter in the IF/ID pipeline register*/
        state->if_id.pc=state->pc;
    
        /*Increment the program counter to the index of the next 4 bytes*/
        state->pc=state->pc+4;

        state->id_init=TRUE;
    }
}

