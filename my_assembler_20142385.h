/* 
 * my_assembler �Լ��� ���� ���� ���� �� ��ũ�θ� ��� �ִ� ��� �����̴�. 
 * 
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3
#define MAX_LTORG 10
#define STR_LEN 100

#define A 0
#define X 1
#define L 2
#define B 3
#define S 4
#define T 5
#define F 6
#define PC 7
#define SW 8

#define ADD 1
#define SUB 2

#define OUT_FILE "output_20142385.txt"
/* 
 * instruction ��� ���Ϸ� ���� ������ �޾ƿͼ� �����ϴ� ����ü �����̴�.
 * ���� ���� �ϳ��� instruction�� �����Ѵ�.
 */
struct inst_unit {
	char str[10];
	unsigned char op;
	int format;
	int ops;
};
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;

/*
 * ����� �� �ҽ��ڵ带 �Է¹޴� ���̺��̴�. ���� ������ ������ �� �ִ�.
 */
char *input_data[MAX_LINES];
static int line_num;


/* 
 * ����� �� �ҽ��ڵ带 ��ū������ �����ϱ� ���� ����ü �����̴�.
 * operator�� renaming�� ����Ѵ�.
 * nixbpe�� 8bit �� ���� 6���� bit�� �̿��Ͽ� n,i,x,b,p,e�� ǥ���Ѵ�.
 */
struct token_unit {
	char label[20];
	char operator[20]; 
	char operand[MAX_OPERAND][20];
	char comment[100];
	char nixbpe; // ���� ������Ʈ���� ���ȴ�.
	int pc;
};

typedef struct token_unit token; 
token *token_table[MAX_LINES]; 
static int token_line;
static int token_start;

/*
 * �ɺ��� �����ϴ� ����ü�̴�.
 * �ɺ� ���̺��� �ɺ� �̸�, �ɺ��� ��ġ�� �����ȴ�.
 * ���� ������Ʈ���� ���ȴ�.
 */
struct symbol_unit {
	char symbol[10];
	int addr;
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];
static int sym_index;
static int sym_start;

char ltorg[MAX_LTORG][STR_LEN];
static int ltorg_index;

char code[MAX_LINES][STR_LEN];
static int code_line;
static int code_start;

static int locctr;

struct extern_ref {
	char ref[20];
	int addr[10];
	int op[10];
	int count;
};

typedef struct extern_ref extr;
extr extref[MAX_OPERAND];
//--------------

static char *input_file;
static char *output_file;
int init_my_assembler(void);
int init_inst_file(char *inst_file);
int init_input_file(char *input_file);
int token_parsing(char *str);
int search_opcode(char *str);
static int assem_pass1(void);
void make_opcode_output(char *file_name);

/* ���� ������Ʈ���� ����ϰ� �Ǵ� �Լ�*/
void make_symtab_output(char *file_name);
static int assem_pass2(void);
void make_objectcode_output(char *file_name);
int calculate(char *str);