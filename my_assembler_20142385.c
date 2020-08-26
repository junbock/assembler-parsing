/*
 * 화일명 : my_assembler_00000000.c 
 * 설  명 : 이 프로그램은 SIC/XE 머신을 위한 간단한 Assembler 프로그램의 메인루틴으로,
 * 입력된 파일의 코드 중, 명령어에 해당하는 OPCODE를 찾아 출력한다.
 * 파일 내에서 사용되는 문자열 "00000000"에는 자신의 학번을 기입한다.
 */

/*
 *
 * 프로그램의 헤더를 정의한다. 
 *
 */
#pragma warning(disable:4996)
#pragma warning(disable:4477)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>


// 파일명의 "00000000"은 자신의 학번으로 변경할 것.
#include "my_assembler_20142385.h"

/* ----------------------------------------------------------------------------------
 * 설명 : 사용자로 부터 어셈블리 파일을 받아서 명령어의 OPCODE를 찾아 출력한다.
 * 매계 : 실행 파일, 어셈블리 파일 
 * 반환 : 성공 = 0, 실패 = < 0 
 * 주의 : 현재 어셈블리 프로그램의 리스트 파일을 생성하는 루틴은 만들지 않았다. 
 *		   또한 중간파일을 생성하지 않는다. 
 * ----------------------------------------------------------------------------------
 */
int main(int args, char *arg[]) 
{
	if(init_my_assembler()< 0)
	{
		printf("init_my_assembler: 프로그램 초기화에 실패 했습니다.\n"); 
		return -1; 
	}

	if(assem_pass1() < 0 ){
		printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n") ; 
		return -1 ; 
	}
	
	return 0;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 프로그램 초기화를 위한 자료구조 생성 및 파일을 읽는 함수이다. 
 * 매계 : 없음
 * 반환 : 정상종료 = 0 , 에러 발생 = -1
 * 주의 : 각각의 명령어 테이블을 내부에 선언하지 않고 관리를 용이하게 하기 
 *		   위해서 파일 단위로 관리하여 프로그램 초기화를 통해 정보를 읽어 올 수 있도록
 *		   구현하였다. 
 * ----------------------------------------------------------------------------------
 */
int init_my_assembler(void)
{
	int result ; 

	if((result = init_inst_file("inst.data")) < 0 )
		return -1 ;
	if((result = init_input_file("input.txt")) < 0 )
		return -1 ; 
	return result ; 
}

/* ----------------------------------------------------------------------------------
 * 설명 : 머신을 위한 기계 코드목록 파일을 읽어 기계어 목록 테이블(inst_table)을 
 *        생성하는 함수이다. 
 * 매계 : 기계어 목록 파일
 * 반환 : 정상종료 = 0 , 에러 < 0 
 * 주의 : 기계어 목록파일 형식은 자유롭게 구현한다. 예시는 다음과 같다.
 *	
 *	===============================================================================
 *		   | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 | NULL|
 *	===============================================================================	   
 *		
 * ----------------------------------------------------------------------------------
 */
int init_inst_file(char *inst_file)
{
	FILE * file;
	inst *instbuf;

	int errno;

	int str_idx, buf_idx, n;
	int hex;
	char buf[STR_LEN] = { 0 };
	char strbuf[STR_LEN] = { 0 };
	char ch;
	
	if ((file = fopen(inst_file, "r")) == NULL) {
		fprintf(stderr, "fopen error for %s", inst_file);
		errno = -1;
	}
	else {
		memset(strbuf, 0, STR_LEN);
		instbuf = (inst *)calloc(1, sizeof(inst));
		str_idx = 0;
		buf_idx = 0;
		inst_index = 0;
		n = 0;
		while (fread(&ch, sizeof(char), 1, file)) {
			//개행 문자까지의 문자를 strbuf에 저장 
			if (ch != '\n')
				strbuf[str_idx++] = ch;
			else {
				n = 0;
				if (str_idx > 0) {
					//op 문자 저장
					while (strbuf[n] != ' ' && strbuf[n] != '\t') {
						instbuf->str[n] = strbuf[n];
						n++;
					}
					//공백무시
					while (strbuf[n] == ' ' || strbuf[n] == '\t') {
						n++;
					}
					
					//op 포맷 저장
					memset(buf, 0, STR_LEN);
					buf_idx = 0;
					while (strbuf[n] != ' ' && strbuf[n] != '\t') {
						buf[buf_idx++] = strbuf[n++];
					}
					instbuf->format = atoi(buf);
					//공백무시
					while (strbuf[n] == ' ' || strbuf[n] == '\t') {
						n++;
					}

					//op 코드 저장
					memset(buf, 0, STR_LEN);
					buf_idx = 0;
					while (strbuf[n] != ' ' && strbuf[n] != '\t') {
						buf[buf_idx++] = strbuf[n++];
					}
					sscanf(buf, "%X", &hex);
					instbuf->op = (unsigned char)hex;
					//공백 무시
					while (strbuf[n] == ' ' || strbuf[n] == '\t') {
						n++;
					}

					//operand 수 저장
					memset(buf, 0, STR_LEN);
					buf_idx = 0;
					while (strbuf[n] != ' ' && strbuf[n] != '\t' && strbuf[n] != 0) {
						buf[buf_idx++] = strbuf[n++];
					}
					instbuf->ops = atoi(buf);

					//inst_buf를 inst_table에 저장
					inst_table[inst_index++] = instbuf;

					memset(strbuf, 0, STR_LEN);
					//다음 instbuf생성
					instbuf = (inst *)calloc(1, sizeof(inst));
					str_idx = 0;
				}
			}
		}

	}
	/* add your code here */
	fclose(file);
	return errno;
	
}

/* ----------------------------------------------------------------------------------
 * 설명 : 어셈블리 할 소스코드를 읽어 소스코드 테이블(input_data)를 생성하는 함수이다. 
 * 매계 : 어셈블리할 소스파일명
 * 반환 : 정상종료 = 0 , 에러 < 0  
 * 주의 : 라인단위로 저장한다.
 *		
 * ----------------------------------------------------------------------------------
 */
int init_input_file(char *input_file)
{
	FILE * file;
	int errno;

	char ch=0;
	char *strbuf;
	int str_idx;

	if ((file = fopen(input_file, "r")) == NULL) {
		fprintf(stderr, "fopen error for %s", input_file);
		errno = -1;
	}
	else {
		line_num = 0;
		str_idx = 0;
		strbuf = (char *)calloc(STR_LEN, sizeof(char));
		
		fseek(file, -1, SEEK_CUR);

		while (fread(&ch, sizeof(char), 1, file)) {
			//개행문자를 만나기 전까지의 문자들을 strbuf에 저장
			if (ch != '\n')
				strbuf[str_idx++] = ch;
			else {
				//strbuf를 input_data에 저장
				input_data[line_num++] = strbuf;
				str_idx = 0;
				//새로운 strbuf 생성
				strbuf = (char *)calloc(STR_LEN, sizeof(char));
				
			}
		}
	}
	/* add your code here */
	fclose(file);
	return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다.
 *        패스 1로 부터 호출된다.
 * 매계 : 파싱을 원하는 문자열
 * 반환 : 정상종료 = 0 , 에러 < 0
 * 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다.
 * ----------------------------------------------------------------------------------
 */
int token_parsing(char *str)
{
	int str_index = 0, op_index, format = 0;
	int i, j;
	int answer = 0;
	token *new_token;
	token *tmp_token;

	//주석무시
	if (str[0] == '.') {
		return 0;
	}

	//token버퍼 생성
	new_token = (token *)calloc(1, sizeof(token));

	//label
	for (i = 0; str[str_index] != '\t' && str[str_index] != 0; str_index++, i++) {
		if (str[str_index] == 0) {
			return -1;
		}
		new_token->label[i] = str[str_index];
	}
	
	str_index++;

	//operator
	for (i = 0; str[str_index] != '\t' && str[str_index] != 0; str_index++, i++) {
		new_token->operator[i] = str[str_index];
	}

	str_index++;

	//opcode_search
	if (new_token->operator[0] == '+') {
		op_index = search_opcode(&new_token->operator[1]);
		if (op_index == -1) {
			return -1;
		}
		else {
			if (inst_table[op_index]->format != 34)
				return -1;
			else {
				new_token->nixbpe |= 1;
				format = 4;
			}
		}
		
	}
	else {
		op_index = search_opcode(&new_token->operator[0]);
		//op코드 아닌경우
		if (op_index == -1) {
			if (strcmp(new_token->operator, "CSECT") == 0 || strcmp(new_token->operator, "END") == 0) {
				answer = 1;
				//ltorg
				if (ltorg_index > 0) {
					tmp_token = new_token;

					for (i = 0; i < ltorg_index; i++) {
						strcpy(sym_table[sym_index].symbol, ltorg[i]);
						sym_table[sym_index++].addr = locctr;

						if (ltorg[i][1] == 'X') {
							locctr += 1;
						}
						else if (ltorg[i][1] == 'C') {
							for (j = 3; ltorg[i][j] != '\''; j++) {}
							locctr += j - 3;
						}

						new_token = (token *)calloc(1, sizeof(token));
						strcpy(new_token->operator, "=");
						strcpy(new_token->operand, &ltorg[i][1]);
						token_table[token_line++] = new_token;
					}
					new_token = (token *)calloc(1, sizeof(token));
					strcpy(new_token->operator, "LTORG");
					new_token->pc = locctr;
					token_table[token_line++] = new_token;

					memset(ltorg, 0, sizeof(ltorg));
					ltorg_index = 0;
					new_token = tmp_token;
				}
			}
		}
		else {
			if (inst_table[op_index]->format == 34) {
				format = 3;
			}
			else
				format = inst_table[op_index]->format;
		}
	}

	//operand
	for (i = 0, j = 0; str[str_index] != '\t' && str[str_index] != 0; str_index++) {
		if (str[str_index] == ',') {
			i++;
			j = 0;
			continue;
		}
		new_token->operand[i][j++] = str[str_index];
	}


	str_index++;

	//NIXBPE
	if (new_token->operand[1][0] == 'X')
		new_token->nixbpe |= 1 << 3;
	if (format != 0) {
		if (new_token->operand[0][0] == '#')
			new_token->nixbpe |= 1 << 4;
		else if (new_token->operand[0][0] == '@')
			new_token->nixbpe |= 1 << 5;
		else {
			new_token->nixbpe |= 1 << 4;
			new_token->nixbpe |= 1 << 5;
		}
		
		if (new_token->operand[0][0] == '=') {
			for (i = 0; i < ltorg_index; i++) {
				if (strcmp(ltorg[i], new_token->operand[0]) == 0)
					break;
			}
			if (i == ltorg_index)
				strcpy(ltorg[ltorg_index++], new_token->operand[0]);
		}
	}

	//comment
	for (i = 0; str[str_index] != 0; str_index++, i++) {
		new_token->comment[i] = str[str_index];
	}

	//start, csect addr
	if (strcmp(new_token->operator, "START") == 0) {
		locctr = atoi(new_token->operand[0]);
	}
	if (strcmp(new_token->operator, "CSECT") == 0) {
		locctr = 0;
	}

	//symbol
	if (strlen(new_token->label) > 0) {
		strcpy(sym_table[sym_index].symbol, new_token->label);
		if (strcmp(new_token->operator, "EQU") == 0) {
			sym_table[sym_index++].addr = calculate(new_token->operand[0]);
			
		}
		else {
			sym_table[sym_index++].addr = locctr;
		}
	}

	//next locctr
	if (format > 0 && format < 5) {
		locctr += format;
		new_token->pc = locctr;
	}
	else if (format == 0) {
		if (strcmp(new_token->operator, "RESW") == 0) {
			locctr += atoi(new_token->operand[0]) * 3;
		}
		else if (strcmp(new_token->operator, "RESB") == 0) {
			locctr += atoi(new_token->operand[0]);
		}
		else if (strcmp(new_token->operator, "WORD") == 0) {
			locctr += 3;
		}
		else if (strcmp(new_token->operator, "BYTE") == 0) {
			if (new_token->operand[0][0] == 'X') {
				locctr += 1;
			}
			else if (new_token->operand[0][0] == 'C') {
				for (i = 2; new_token->operand[0][i] != '\''; i++) {}
				locctr += i - 2;
			}
		}
		else if (strcmp(new_token->operator, "LTORG") == 0) {
			//create LTORG token
			tmp_token = new_token;

			for (i = 0; i < ltorg_index; i++) {
				strcpy(sym_table[sym_index].symbol, ltorg[i]);
				sym_table[sym_index++].addr = locctr;

				if (ltorg[i][1] == 'X') {
					locctr += 1;
				}
				else if (ltorg[i][1] == 'C') {
					for (j = 3; ltorg[i][j] != '\''; j++) {}
					locctr += j - 3;
				}

				new_token = (token *)calloc(1, sizeof(token));
				strcpy(new_token->operator, "=");
				strcpy(new_token->operand, &ltorg[i][1]);
				token_table[token_line++] = new_token;
			}
			new_token = tmp_token;
			memset(ltorg, 0, sizeof(ltorg));
			ltorg_index = 0;
		}
		new_token->pc = locctr;
	}

	token_table[token_line++] = new_token;

	return answer;
}


/* ----------------------------------------------------------------------------------
 * 설명 : 입력 문자열이 기계어 코드인지를 검사하는 함수이다. 
 * 매계 : 토큰 단위로 구분된 문자열 
 * 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0 
 * 주의 : 
 *		
 * ----------------------------------------------------------------------------------
 */
int search_opcode(char *str) 
{
	/* add your code here */
	int i;
	
	for (i = 0; i < inst_index; i++) {
		if (strcmp(inst_table[i]->str, str) == 0)
			break;
	}

	if (i == inst_index)
		return -1;
	else
		return i;
}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 위한 패스1과정을 수행하는 함수이다.
*		   패스1에서는..
*		   1. 프로그램 소스를 스캔하여 해당하는 토큰단위로 분리하여 프로그램 라인별 토큰
*		   테이블을 생성한다.
*
* 매계 : 없음
* 반환 : 정상 종료 = 0 , 에러 = < 0
* 주의 : 현재 초기 버전에서는 에러에 대한 검사를 하지 않고 넘어간 상태이다.
*	  따라서 에러에 대한 검사 루틴을 추가해야 한다.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
	/* add your code here */

	/* input_data의 문자열을 한줄씩 입력 받아서 
	 * token_parsing()을 호출하여 token_unit에 저장
	 */ 
	int i, end;
	sym_start = 0;
	token_start = 0;
	for (i = 0; i < line_num; i++) {
		end = token_parsing(input_data[i]);
		if (end) {
			make_symtab_output("symtab.txt", sym_start);
			assem_pass2(token_start);
			token_start = token_line - 1;
			sym_start = sym_index - 1;
		}
	}

	return 0;
}


/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 명령어 옆에 OPCODE가 기록된 표(과제 4번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*        또한 과제 4번에서만 쓰이는 함수이므로 이후의 프로젝트에서는 사용되지 않는다.
* -----------------------------------------------------------------------------------
*/
void make_opcode_output(char *file_name)
{
	/* add your code here */
	FILE *file;
	int i,j;
	int inst_idx;
	
	char str[STR_LEN] = { 0 };
	char hex[5] = { 0 };

	if ((file = fopen(file_name, "w")) == NULL) {
		fprintf(stderr, "fopen error for %s", file_name);
		return;
	}

	for (i = 0; i < line_num; i++) {
		if ((inst_idx = token_parsing(input_data[i])) >= 0) {
			memset(str, 0, sizeof(str));
			strcpy(str, token_table[token_line - 1]->label);
			strcat(str, "\t");
			strcat(str, token_table[token_line - 1]->operator);
			strcat(str, "\t");

			for (j = 0; j < inst_table[inst_idx]->ops; j++) {
				strcat(str, token_table[token_line - 1]->operand[j]);
				if (j < inst_table[inst_idx]->ops - 1)
					strcat(str, ", ");
			}
			
			memset(hex, 0, sizeof(hex));
			sprintf(hex, "\t%X\n", inst_table[inst_idx]->op);
			strcat(str, hex);

			fwrite(str, strlen(str), 1, file);
		}

	}
	fclose(file);
}

/* --------------------------------------------------------------------------------*
* ------------------------- 추후 프로젝트에서 사용할 함수 --------------------------*
* --------------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 SYMBOL별 주소값이 저장된 TABLE이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_symtab_output(char *file_name)
{
	/* add your code here */
	int i;
	static FILE *file;
	char str[STR_LEN] = { 0 };
	
	if (file == NULL)
		if ((file = fopen(file_name, "w")) == NULL) {
			fprintf(stderr, "fopen error for %s", file_name);
			return;
		}

	for (i = sym_start; i < sym_index - 1; i++) {
		sprintf(str, "%s	%X\n", sym_table[i].symbol, sym_table[i].addr);
		fwrite(str, strlen(str), 1, file);
		memset(str, 0, sizeof(str));
	}

	str[0] = '\n';
	fwrite(str, strlen(str), 1, file);

}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행하는 함수이다.
*		   패스 2에서는 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
*		   다음과 같은 작업이 수행되어 진다.
*		   1. 실제로 해당 어셈블리 명령어를 기계어로 바꾸는 작업을 수행한다.
* 매계 : 없음
* 반환 : 정상종료 = 0, 에러발생 = < 0
* 주의 :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2()
{
	/* add your code here */
	int i, j, k;
	int opcode, symbol = -1, codebuf;
	int start, end, count = 0, addr = 0;
	int format, length, byte, program_start = -1;

	char strbuf[STR_LEN] = { 0 };
	char str_byte[STR_LEN] = { 0 };
	char *charpointer = NULL;

	memset(extref, 0, sizeof(extref));

	for (i = token_start; i < token_line - 1; i++) {
		//FORMAT
		if (token_table[i]->operator[0] == '+') {
			opcode = search_opcode(&token_table[i]->operator[1]);
			if (opcode >= 0)
				format = 4;
			else
				format = -1;
		}
		else {
			opcode = search_opcode(token_table[i]->operator);
			if (opcode >= 0) {
				if (inst_table[opcode]->format == 34)
					format = 3;
				else
					format = inst_table[opcode]->format;
			}
			else
				format = -1;
		}
	
		//NOT OPCODE
		if (format == -1) {
			//START, CSECT
			if (strcmp(token_table[i]->operator, "START") == 0 || strcmp(token_table[i]->operator, "CSECT") == 0) {
				//Generate Head
				memset(strbuf, 0, sizeof(strbuf));
				strbuf[0] = 'H';
				strcat(strbuf, token_table[i]->label); strcat(strbuf, "\t");
				strcpy(code[code_line++], strbuf); count++;

				memset(strbuf, 0, sizeof(strbuf));
				if (strlen(token_table[i]->operand[0]) == 0) {
					sprintf(strbuf, "%06X", 0);
				}
				else
					sprintf(strbuf, "%06X", atoi(token_table[i]->operand[0]));
				strcpy(code[code_line++], strbuf); count++;

				memset(strbuf, 0, sizeof(strbuf));
				length = token_table[token_line - 2]->pc - token_table[token_start]->pc;
				sprintf(strbuf, "%06X", length);
				strcpy(code[code_line++], strbuf); count++;

				make_objectcode_output(OUT_FILE);
				code_start = code_line;
				start = token_table[i]->pc;
				end = start;
				count = 0;

				if (strcmp(token_table[i]->operator, "START") == 0)
					program_start = start;
			}

			//EXTDEF
			if (strcmp(token_table[i]->operator, "EXTDEF") == 0) {
				code[code_line][0] = 'D';
				
				for (j = 0; strlen(token_table[i]->operand[j]) != 0 && j < 3; j++) {
					memset(strbuf, 0, sizeof(strbuf));
					strcat(code[code_line], token_table[i]->operand[j]);
					for (k = sym_start; k < sym_index - 1; k++) {
						if (strcmp(token_table[i]->operand[j], sym_table[k].symbol) == 0) {
							break;
						}
					}
					if (k == sym_index - 1) {
						return -1;
					}
					else
						sprintf(strbuf, "%06X", sym_table[k].addr);

					strcat(code[code_line++], strbuf);
				}

				make_objectcode_output(OUT_FILE);
				code_start = code_line;
				start = token_table[i]->pc;
				end = start;
				count = 0;
			}

			//EXTREF
			if (strcmp(token_table[i]->operator, "EXTREF") == 0) {
				code[code_line][0] = 'R';

				for (j = 0; strlen(token_table[i]->operand[j]) != 0 && j < 3; j++) {
					strcat(code[code_line], token_table[i]->operand[j]);
					strcpy(extref[j].ref, token_table[i]->operand[j]);
					strcat(code[code_line], "\t");
				}
				code_line++;
				make_objectcode_output(OUT_FILE);
				code_start = code_line;
				start = token_table[i]->pc;
				end = start;
				count = 0;
			}

			//RESERVE
			if (strcmp(token_table[i]->operator, "RESW") == 0 || strcmp(token_table[i]->operator, "RESB") == 0) {
				//finish line
				while (strcmp(token_table[i]->operator, "RESW") == 0 || strcmp(token_table[i]->operator, "RESB") == 0) {
					i++;
				}
				i--;
				sprintf(code[code_start], "T%06X%02X", start, end - start);
				make_objectcode_output(OUT_FILE);
				code_start = code_line;
				start = token_table[i]->pc;
				end = start;
				count = 0;
			}

			//WORD
			if (strcmp(token_table[i]->operator, "WORD") == 0) {
				addr = -1;
				if (code_line == code_start) {
					code_line++;
				}

				for (j = sym_start; j < sym_index - 1; j++) {
					if (strstr(sym_table[j].symbol, token_table[i]->operand[0]) != NULL) {
						break;
					}
				}

				if (j < sym_index - 1) {				
					addr = calculate(token_table[i]->operand[0]);
				}
				else {
					for (j = 0; j < MAX_OPERAND; j++) {
						if ((charpointer = strstr(token_table[i]->operand[0], extref[j].ref)) != NULL) {
							extref[j].addr[extref[j].count] = (token_table[i]->pc - 3) * 16 * 16;
							extref[j].addr[extref[j].count] += 6;
							if (charpointer == token_table[i]->operand[0])
								extref[j].op[extref[j].count++] = ADD;
							else {
								if (*(charpointer - 1) == '+')
									extref[j].op[extref[j].count++] = ADD;
								else if (*(charpointer - 1) == '-')
									extref[j].op[extref[j].count++] = SUB;
							}
							addr = 0;
						}
					}
				}

				if (addr == -1)
					addr = calculate(token_table[i]->operand[0]);

				memset(strbuf, 0, sizeof(strbuf));
				sprintf(strbuf, "%06X", addr);
				strcpy(code[code_line++], strbuf); count++;
				end = token_table[i]->pc;
			}

			//BYTE
			if (strcmp(token_table[i]->operator, "BYTE") == 0) {
				if (code_line == code_start) {
					code_line++;
				}

				memset(strbuf, 0, sizeof(strbuf));
				memset(str_byte, 0, sizeof(str_byte));

				for (j = 2; token_table[i]->operand[0][j] != '\''; j++) {
					strbuf[j - 2] = token_table[i]->operand[0][j];
				}

				if (token_table[i]->operand[0][0] == 'X') {
					sscanf(strbuf, "%X", &byte);
					sprintf(str_byte, "%02X", byte);
				}
				else if (token_table[i]->operand[0][0] == 'C') {
					for (j = 0; j < strlen(strbuf); j++) {
						sscanf(str_byte + j * 2, "%2X", strbuf[j]);
					}
				}

				strcpy(code[code_line++], str_byte); count++;
				end = token_table[i]->pc;
			}

			//LTORG
			if (strcmp(token_table[i]->operator, "=") == 0) {
				if (code_line == code_start) {
					code_line++;
				}

				memset(strbuf, 0, sizeof(strbuf));
				memset(str_byte, 0, sizeof(str_byte));

				for (j = 2; token_table[i]->operand[0][j] != '\''; j++) {
					strbuf[j - 2] = token_table[i]->operand[0][j];
				}

				if (token_table[i]->operand[0][0] == 'X') {
					sscanf(strbuf, "%X", &byte);
					sprintf(code[code_line++], "%02X", byte); count++;
					end += 1;
				}
				else if (token_table[i]->operand[0][0] == 'C') {
					for (j = 0; j < strlen(strbuf); j++) {
						memset(str_byte, 0, sizeof(str_byte));
						sprintf(str_byte, "%2X", strbuf[j]);
						if (j == 0)
							strcpy(code[code_line], str_byte);
						else
							strcat(code[code_line], str_byte);
						end += 1;
					}
					code_line++;
					count++;
					
				}
							
			}
		}
		else {
			codebuf = 0;
			addr = 0;
			codebuf |= inst_table[opcode]->op << (format - 1) * 8;
			
			//T,addr,length 부분 비워둠
			if (code_line == code_start) {
				code_line++;
			}

			//pc relative
			if (strlen(token_table[i]->operand[0]) > 0) {
				if (token_table[i]->operand[0][0] == '#') {
					for (j = sym_start; j < sym_index; j++) {
						if (strcmp(&token_table[i]->operand[0][1], sym_table[j].symbol) == 0) {
							symbol = j;
							break;
						}
					}

					if (symbol == sym_index - 1 && format != 4) {
						token_table[i]->nixbpe |= 1 << 1;
					}
				}
				else {
					if (format != 4)
						token_table[i]->nixbpe |= 1 << 1;
					for (j = sym_start; j < sym_index; j++) {
						if (strcmp(&token_table[i]->operand[0][0], sym_table[j].symbol) == 0) {
							symbol = j;
							break;
						}
						if (strcmp(&token_table[i]->operand[0][1], sym_table[j].symbol) == 0) {
							symbol = j;
							break;
						}
					}
				}
			}

			//Format 3,4
			if (format >= 3) {
				codebuf |= token_table[i]->nixbpe << ((format - 3) * 8 + 12);
				
				if (strlen(token_table[i]->operand[0]) > 0) {
					if (j < sym_index) {
						addr = sym_table[symbol].addr;
						addr -= token_table[i]->pc;
					}
					else {
						for (j = 0; j < MAX_OPERAND; j++) {
							if (strcmp(extref[j].ref, token_table[i]->operand[0]) == 0)
								break;
							else if (strcmp(extref[j].ref, &token_table[i]->operand[0][1]) == 0)
								break;
						}
						if (j == MAX_OPERAND) {
							if (token_table[i]->operand[0][0] == '#')
								addr = atoi(&token_table[i]->operand[0][1]);
							else {
								addr = atoi(token_table[i]->operand[0]);
								addr -= token_table[i]->pc;
							}
						}
						else {
							extref[j].addr[extref[j].count] = (token_table[i]->pc - (format - 1)) * 16 * 16;
							extref[j].addr[extref[j].count] += format * 2 - 3;
							extref[j].op[extref[j].count++] = ADD;
						}
					}
				}
				else
					addr = 0;

				byte = 0;
				for (j = 0; j < format * 4; j++) {
					byte |= 1 << j;
				}
				addr &= byte;
				
				codebuf |= addr;
			}

			//Format 2
			if (format == 2) {
				for (j = 0; strlen(token_table[i]->operand[j]) != 0; j++) {
					if ((byte = get_register(token_table[i]->operand[j])) >= 0) {
						codebuf |= byte << (4 - j * 4);
					}
				}
			}

			//generate string
			if (format == 4) {
				sprintf(code[code_line++], "%08X", codebuf);
			}
			else if (format == 3) {
				sprintf(code[code_line++], "%06X", codebuf);
			}
			else if (format == 2) {
				sprintf(code[code_line++], "%04X", codebuf);
			}
			else
				sprintf(code[code_line++], "%02X", codebuf);
			
			count++;
			end = token_table[i]->pc;
		}

		//Generate Text
		if (end - start >= 29) {
			sprintf(code[code_start], "T%06X%02X", start, end - start);
			make_objectcode_output(OUT_FILE);
			code_start = code_line;
			start = token_table[i]->pc;
			end = start;
			count = 0;
		}

	}

	//Generate Text
	if (count > 0) {
		sprintf(code[code_start], "T%06X%02X", start, end - start);
		make_objectcode_output(OUT_FILE);
		code_start = code_line;
		start = token_table[i]->pc;
		end = start;
		count = 0;
	}

	//Modification
	for (i = 0; i < MAX_OPERAND; i++) {
		if (strlen(extref[i].ref) > 0) {
			for (j = 0; j < extref[i].count; j++) {
				memset(strbuf, 0, sizeof(strbuf));
				code[code_line][0] = 'M';
				if (extref[i].op[j] == ADD)
					sprintf(strbuf, "%08X+%s", extref[i].addr[j], extref[i].ref);
				else if (extref[i].op[j] == SUB)
					sprintf(strbuf, "%08X-%s", extref[i].addr[j], extref[i].ref);
				strcat(code[code_line++], strbuf);
				
				make_objectcode_output(OUT_FILE);
				code_start = code_line;
			}
		}
	}

	memset(strbuf, 0, sizeof(strbuf));
	code[code_line][0] = 'E';
	if (program_start >= 0) {
		sprintf(strbuf, "%06X", program_start);
		strcat(code[code_line], strbuf);
	}
	strcat(code[code_line], "\n");
	code_line++;
	make_objectcode_output(OUT_FILE);
	code_start = code_line;
	return 0;

}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 object code (프로젝트 1번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char *file_name)
{
	/* add your code here */
	int i;
	static FILE *file;
	char str[STR_LEN] = { 0 };

	if (file == NULL)
		if ((file = fopen(file_name, "w")) == NULL) {
			fprintf(stderr, "fopen error for %s", file_name);
			return;
		}

	for (i = code_start; i < code_line; i++) {
		fwrite(code[i], strlen(code[i]), 1, file);
	}
	fwrite("\n", 1, 1, file);

}

//토큰을 잘라서 계산
int calculate(char *str)
{
	int i, j, buf_index, op_index;
	int answer = 0, num[10] = { 0 };
	char strbuf[10][STR_LEN] = { 0 };
	char op[9] = { 0 };

	for (i = 0, j = 0, buf_index = 0, op_index = 0; i < strlen(str); i++) {
		if (str[i] == '+' || str[i] == '-') {
			op[op_index++] = str[i];
			buf_index += 1;
			j = 0;
			continue;
		}
		strbuf[buf_index][j++] = str[i];
	}
	
	for (i = 0; i <= buf_index; i++) {
		if (strcmp(strbuf[i], "*") == 0)
			num[i] = locctr;
		else {
			for (j = sym_start; j < sym_index; j++) {
				if (strcmp(sym_table[j].symbol, strbuf[i]) == 0) {
					num[i] = sym_table[j].addr;
					break;
				}
			}
			if (j == sym_index)
				num[i] = atoi(strbuf[i]);
		}
	}

	answer = num[0];
	
	for (i = 0; i < op_index; i++) {
		if (op[i] == '+')
			answer += num[i + 1];
		else if (op[i] == '-')
			answer -= num[i + 1];
	}

	return answer;
}

//레지스터
int get_register(char *str) 
{
	int answer = -1;
	
	if (strcmp(str, "A") == 0)
		answer = A;
	else if (strcmp(str, "X") == 0)
		answer = X;
	else if (strcmp(str, "L") == 0)
		answer = L;
	else if (strcmp(str, "B") == 0)
		answer = B;
	else if (strcmp(str, "S") == 0)
		answer = S;
	else if (strcmp(str, "T") == 0)
		answer = T;
	else if (strcmp(str, "F") == 0)
		answer = F;
	else if (strcmp(str, "PC") == 0)
		answer = PC;
	else if (strcmp(str, "SW") == 0)
		answer = SW;

	return answer;
}
