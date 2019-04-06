
/*
 * 
 * pipeline.h
 * 
 */

#define TRUE 1
#define FALSE 0

/* Register state */
typedef struct _rf_int_t {
  int_t reg_int[NUMREGS];
} rf_int_t;

typedef struct _rf_fp_t {
  float reg_fp[NUMREGS];
} rf_fp_t;

/* Overall processor state */
typedef struct _state_t {
  /* memory */
  unsigned char mem[MAXMEMORY];

  /* register files */
  rf_int_t rf_int;
  rf_fp_t rf_fp;

  /* pipeline registers */
  unsigned long pc;
  if_id_t if_id;
  id_ex_t id_ex;

  fu_int_t *fu_int_list;
  fu_fp_t *fu_add_list;
  fu_fp_t *fu_mult_list;
  fu_fp_t *fu_div_list;

  ex_mem_wb_t int_wb;
  ex_mem_wb_t fp_wb;

  int fetch_lock;
  int wb_init;
  int ex_init;
  int id_init;

} state_t;

extern state_t *state_create(int *, FILE *, FILE *);
extern int bytes_to_int(char *, unsigned long);
extern void int_to_bytes(unsigned char *, int, unsigned long);
extern float bytes_to_flt(unsigned char *, unsigned long);
extern void flt_to_bytes(unsigned char *, float, unsigned long);
extern void writeback(state_t *, int *);
extern void execute(state_t *);
extern int decode(state_t *);
extern void fetch(state_t *, int);
extern int int_reg_busy(int, fu_int_t *, int);
extern int fp_reg_busy(int, fu_fp_t *);

