#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <ctype.h>	
#include <stdlib.h>	
#include <string>
#include <math.h>
using namespace std;

typedef struct {
	char OPCODE[6];
	int OPNUM;
}OPTABLE;

typedef struct {
	char label[100];
	char opcode[100];
	char operand[100];
	int LOCCTR;
}LINE;

typedef struct {
	char label[10];
	int address;
}SymTab;

OPTABLE OP[100];
SymTab sym[100];
int symIndex; //ã�� symbol�� �ε����� ����
int errorflag=0; //���� �߻��� ����
int countSym = 0; //�ɺ��� ������ ����
int OPpos = 0; //ã�� opcode�� ��ġ�� ����
int saveLOCCTR; //intermediate������ LOCCTR�� ����
int programlength; //program length

void OPTAB(); //OPTABLE�� ����
void PASS1(FILE* SourceCode);	//pass1
LINE Tokenize(char* read);	//pass1- source code���� ������ �а� ������ ����
bool SearchSymtab(char* label);
void InserttoSym(char *label, int LOCCTR);
bool searchOPTAB(char *opcode);
void PASS2();		//pass2
LINE Tokenize_again(char *read);

int main() {
	FILE* SourceCode;
	FILE* intermediate;
	FILE* ObjectProgram;

	SourceCode = fopen("SourceCode.txt", "r");
	if (SourceCode == NULL) {
		printf("���� ����");
		exit(1);
	}
	OPTAB(); //����� ����
	PASS1(SourceCode);
	PASS2();
	printf("�Ϸ�");
}

void OPTAB() { //OPTable
	strcpy(OP[0].OPCODE, "ADD"); OP[0].OPNUM = 0x18;
	strcpy(OP[1].OPCODE, "ADDF"); OP[1].OPNUM = 0x58;
	strcpy(OP[2].OPCODE, "ADDR"); OP[2].OPNUM = 0x90;
	strcpy(OP[3].OPCODE, "AND"); OP[3].OPNUM = 0x40;
	strcpy(OP[4].OPCODE, "CLEAR"); OP[4].OPNUM = 0x180;
	strcpy(OP[5].OPCODE, "COMP"); OP[5].OPNUM = 0x28;
	strcpy(OP[6].OPCODE, "COMPF"); OP[6].OPNUM = 0x88;
	strcpy(OP[7].OPCODE, "COMPR"); OP[7].OPNUM = 0x160;
	strcpy(OP[8].OPCODE, "DIV"); OP[8].OPNUM = 0x24;
	strcpy(OP[9].OPCODE, "DIVF"); OP[9].OPNUM = 0x64;
	strcpy(OP[10].OPCODE, "DIVR"); OP[10].OPNUM = 0x156;
	strcpy(OP[11].OPCODE, "FIX"); OP[11].OPNUM = 0x196;
	strcpy(OP[12].OPCODE, "FLOAT"); OP[12].OPNUM = 0x192;
	strcpy(OP[13].OPCODE, "HIO"); OP[13].OPNUM = 0x244;
	strcpy(OP[14].OPCODE, "J"); OP[14].OPNUM = 0x60;
	strcpy(OP[15].OPCODE, "JEQ"); OP[15].OPNUM = 0x30;
	strcpy(OP[16].OPCODE, "JGT"); OP[16].OPNUM = 0x34;
	strcpy(OP[17].OPCODE, "JLT"); OP[17].OPNUM = 0x38;
	strcpy(OP[18].OPCODE, "JSUB"); OP[18].OPNUM= 0x48;
	strcpy(OP[19].OPCODE, "LDA"); OP[19].OPNUM = 0x0;
	strcpy(OP[20].OPCODE, "LDB"); OP[20].OPNUM = 0x68;
	strcpy(OP[21].OPCODE, "LDCH"); OP[21].OPNUM = 0x50;
	strcpy(OP[22].OPCODE, "LDF"); OP[22].OPNUM = 0x70;
	strcpy(OP[23].OPCODE, "LDL"); OP[23].OPNUM = 0x8;
	strcpy(OP[24].OPCODE, "LDS"); OP[24].OPNUM = 0x108;
	strcpy(OP[25].OPCODE, "LDT"); OP[25].OPNUM = 0x74;
	strcpy(OP[26].OPCODE, "LDX"); OP[26].OPNUM = 0x4;
	strcpy(OP[27].OPCODE, "LPS"); OP[27].OPNUM = 0x208;
	strcpy(OP[28].OPCODE, "MUL"); OP[28].OPNUM = 0x20;
	strcpy(OP[29].OPCODE, "MULF"); OP[29].OPNUM = 0x60;
	strcpy(OP[30].OPCODE, "MULR"); OP[30].OPNUM = 0x98;
	strcpy(OP[31].OPCODE, "NORM"); OP[31].OPNUM = 0x200;
	strcpy(OP[32].OPCODE, "OR"); OP[32].OPNUM = 0x44;
	strcpy(OP[33].OPCODE, "RD"); OP[33].OPNUM = 0x216;
	strcpy(OP[34].OPCODE, "RMO"); OP[34].OPNUM = 0x172;
	strcpy(OP[35].OPCODE, "RSUB"); OP[35].OPNUM = 0x76;
	strcpy(OP[36].OPCODE, "SHIFTL"); OP[36].OPNUM = 0x164;
	strcpy(OP[37].OPCODE, "SHIFTR"); OP[37].OPNUM = 0x168;
	strcpy(OP[38].OPCODE, "SIO"); OP[38].OPNUM = 0x240;
	strcpy(OP[39].OPCODE, "SSK"); OP[39].OPNUM = 0x236;
	strcpy(OP[40].OPCODE, "STA"); OP[40].OPNUM = 0x12;
	strcpy(OP[41].OPCODE, "STB"); OP[41].OPNUM = 0x78;
	strcpy(OP[42].OPCODE, "STCH"); OP[42].OPNUM = 0x54;
	strcpy(OP[43].OPCODE, "STF"); OP[43].OPNUM = 0x80;
	strcpy(OP[44].OPCODE, "STI"); OP[44].OPNUM = 0x212;
	strcpy(OP[45].OPCODE, "STL"); OP[45].OPNUM = 0x14;
	strcpy(OP[46].OPCODE, "STS"); OP[46].OPNUM = 0x124;
	strcpy(OP[47].OPCODE, "STSW"); OP[47].OPNUM = 0x232;
	strcpy(OP[48].OPCODE, "STT"); OP[48].OPNUM = 0x84;
	strcpy(OP[49].OPCODE, "STX"); OP[49].OPNUM = 0x10;
	strcpy(OP[50].OPCODE, "SUB"); OP[50].OPNUM = 0x28;
	strcpy(OP[51].OPCODE, "SUBF"); OP[51].OPNUM = 0x92;
	strcpy(OP[52].OPCODE, "SUBR"); OP[52].OPNUM = 0x94;
	strcpy(OP[53].OPCODE, "SVC"); OP[53].OPNUM = 0x176;
	strcpy(OP[54].OPCODE, "TD"); OP[54].OPNUM = 0x224;
	strcpy(OP[55].OPCODE, "TIO"); OP[55].OPNUM = 0x248;
	strcpy(OP[56].OPCODE, "TIX"); OP[56].OPNUM = 0x44;
	strcpy(OP[57].OPCODE, "TIXR"); OP[57].OPNUM = 0x184;
	strcpy(OP[58].OPCODE, "WD"); OP[58].OPNUM = 0x220;

}

void PASS1(FILE* SourceCode) { //pass1
	FILE* intermediate;

	int LOCCTR;
	int StartAddress; //���� �ּҸ� �����Ѵ�
	char line[500] = "";	//SourceProgram�� ���� ����
	LINE linecode = { 0 };

	intermediate = fopen("intermediate.txt", "w"); //intermediate���� ����
	if (intermediate == NULL) {
		printf("����");
		exit(1);
	}
	fgets(line, 500, SourceCode); //Sourceprogram���� �� ���� �д´�
	//read first input line
	linecode = Tokenize(line);

	if (strcmp(linecode.opcode, "START") == 0) { //if OPCODE = 'START' then
		StartAddress = strtol(linecode.operand, NULL, 16); //16������ 10������
		//startaddress�� 10���� ����
		//save #[operand] as starting address
		LOCCTR = StartAddress;
		//initialize LOCCTR to Starting address
		fprintf(intermediate, "%s\t%s\t%s\t\t%04X\n", linecode.label, linecode.opcode, linecode.operand, LOCCTR);
		//write line to intermediate file
		fgets(line, 500, SourceCode);
		linecode = Tokenize(line);
		//read next input line
	}
	else {
		LOCCTR = 0; //else initialize LOCCTR to 0
	}

	while (strcmp(linecode.opcode, "END") != 0) { //while OPCODE <> 'END' do
		saveLOCCTR = LOCCTR; //���� LOCCTR�� ����
		if (strcmp(linecode.label, ".") != 0) { //if this is not a comment line - �ּ��� �ƴ϶��
			if (strcmp(linecode.label, "\t") != 0) { //if there is a symbol in the LABEL field then
				if (SearchSymtab(linecode.label) == true) { //search SYMTAB for LABEL
					errorflag++;
					printf("duplicate error\n");
					//if found then set error flag
				}
				else
					InserttoSym(linecode.label, LOCCTR);
			}
			if (searchOPTAB(linecode.opcode) == true) //��� ��ɾ�� 3byte�̴�
				LOCCTR += 3;
			else if (strcmp(linecode.opcode, "WORD") == 0)
				LOCCTR += 3;
			else if(strcmp(linecode.opcode, "RESW")==0)
				LOCCTR += (3 * atoi(linecode.operand));
			else if(strcmp(linecode.opcode,"RESB")==0)
				LOCCTR += atoi(linecode.operand);
			else if (strcmp(linecode.opcode, "BYTE") == 0) {
				if (linecode.operand[0] == 'X')
					LOCCTR += 1;
				else
					LOCCTR += 3;
			}
			else {
				printf("error - invalid operation code"); //invalid operation code
				errorflag++;
			}
		}
		if (strcmp(linecode.label, ".") != 0) {
			if (strcmp(linecode.label, "\t") == 0 && strcmp(linecode.operand, "\t") == 0) {
				fprintf(intermediate, "%s%s\t%s\t%04X\n", linecode.label, linecode.opcode, linecode.operand, saveLOCCTR);
				//intermediate file�� �ۼ�
			}
			else if (strcmp(linecode.label, "\t") != 0) {
				fprintf(intermediate, "%s\t%s\t%s\t\t%04X\n", linecode.label, linecode.opcode, linecode.operand, saveLOCCTR);
			}
			else {
				if (strcmp(linecode.operand, "BUFFER,X") == 0)
					fprintf(intermediate, "%s%s\t%s\t%04X\n", linecode.label, linecode.opcode, linecode.operand, saveLOCCTR);
				else
					fprintf(intermediate, "%s%s\t%s\t\t%04X\n", linecode.label, linecode.opcode, linecode.operand, saveLOCCTR);
			}
		}
		else {
			fprintf(intermediate, "%s\t%s%s\n", linecode.label, linecode.opcode, linecode.operand);
		}
		fgets(line,500, SourceCode); //read next input line
		linecode = Tokenize(line);
	}
	fprintf(intermediate, "%s%s\t%s", linecode.label, linecode.opcode, linecode.operand);
	//write last line to intermediate file
	programlength = LOCCTR - StartAddress; //save program length
	fclose(intermediate);

}

//int convDec(char *hex) {
//	int dec = 0;
//	int pos = 0;
//
//	for (int i = strlen(hex) - 1; i >= 0; i--) {
//		char ch = hex[i];
//		if (ch >= 48 && ch <= 57) {
//			dec += (ch - 48)*pow(16, pos);
//		}
//		else if (ch >= 65 && ch <= 70) {
//			dec += (ch - (65 - 10))*pow(16, pos);
//		}
//		else if (ch >= 97 && ch <= 102) {
//			dec += (ch - (97 - 10))*pow(16, pos);
//		}
//		pos++;
//	}
//	return dec;
//}

LINE Tokenize(char* read) { //�о���� ���ٿ��� ����
	char *temp;
	char first[10] = "";
	char second[10] = "";
	char third[10] = "";
	int i = 0;

	LINE save = { 0 };
	temp = strtok(read, "\t\n"); //���⸦ �������� ������
	strcpy(first, temp);
	if (strcmp(first, ".")==0) { //�ּ����� �Ǵ�
		strcpy(save.label, first);	//first�� �����Ѵ�
		temp = strtok(NULL, "\n");
		if (temp != NULL) {
			strcpy(save.opcode, temp);
			strcpy(save.operand, "\t");
		}
		printf("%s", save.opcode);
		return save; 
	}
	else {
		while (temp != NULL) {
			if (i == 1) strcpy(second,temp); //�ι�° ��ū
			else if (i == 2) strcpy(third, temp); //����° ��ū
			i++;
			temp = strtok(NULL, " .\t\n"); //�� ���� ���� �Է� �޴´�
		}
		if (i == 1) { //RSUB(Return Subroutine)�� ���
			strcpy(save.label,"\t");
			strcpy(save.opcode, first);
			strcpy(save.operand, "\t");
		}
		else if (i == 2) { //opcode, operand
			strcpy(save.label, "\t");
			strcpy(save.opcode, first);
			strcpy(save.operand, second);
		}
		else if (i == 3) { //label, opcode, operand
			strcpy(save.label, first);
			strcpy(save.opcode, second);
			strcpy(save.operand, third);
		}
		
		return save;

	}


}

bool SearchSymtab(char* label) {
	for (int i = 0; i < 100; i++) {
		if (strcmp(label, sym[i].label) == 0) {
			symIndex = i;
			return true;
		}
		else if (strcmp(label, "BUFFER,X") == 0) {
			symIndex = 1000;
			return true;
		}
	}
	return false;
}

void InserttoSym(char *label, int LOCCTR) {
	strcpy(sym[countSym].label, label);
	sym[countSym].address = LOCCTR;
	countSym += 1;
}

bool searchOPTAB(char *opcode) { //search SYMTAB for OPCODE
	for (int i = 0; i < 100; i++) {
		if (strcmp(opcode, OP[i].OPCODE) == 0) { //OPCODE�� SYTMTAB�� �����Ѵ�
			OPpos = i; //��ġ�� �ѱ��
			return true;
		}
	}
	return false;
}

void PASS2() {
	bool start = true;
	int StartAddress = 0;
	int operandAddress = 0;
	char sumAddress[10] = "";
	char WordsAdd[10] = "";
	char ObjectLine[61] = "";
	int temp;
	int Objnum=0;

	LINE linecode = { 0 };
	FILE *obj;
	FILE* intermediate;
	FILE* list;
	char line[500] = "";
	int lineopcode;

	if ((obj = fopen("ObjectProgram.txt", "w")) == NULL) { //object������ ����
		fprintf(stderr, "ObjectProgram.txt ����");
		exit(1);
	}
	if ((list = fopen("ListingFile.txt", "w")) == NULL) { //listing ������ ����
		fprintf(stderr, "ListingFile.txt ����");
		exit(1);
	}
	if ((intermediate = fopen("intermediate.txt", "r")) == NULL) { //intermediate������ ����
		fprintf(stderr, "intermediate.txt ����");
		exit(1);
	}
	//begin
	fgets(line, 500, intermediate); //read first input line (from intermediate file)
	linecode = Tokenize_again(line);
	if (strcmp(linecode.opcode, "START") == 0)		//if OPCODE = 'START' then //write listing line
	{
		//begin
		fprintf(obj, "H%-6s%06X%06X\n", linecode.label, linecode.LOCCTR, programlength); //write header record to object Program
		//Object Program : Header record + program name + location + program length
		fprintf(list, "%04X\t%s\t%s\t%s\t\n", linecode.LOCCTR, linecode.label, linecode.opcode, linecode.operand);
		//Listing File : Location counter + Label + Opcode + Operand
	}
	fgets(line, 255, intermediate);		//read next line
	linecode = Tokenize_again(line);
	
	while (strcmp(linecode.opcode, "END") != 0) {
		if (linecode.label[0] != '.') {
			if (start == true) {
				StartAddress = linecode.LOCCTR;
				start = false;
			}
		}
		if (linecode.label[0] != '.') {		//if this is not a comment line then 
											//begin
			if (searchOPTAB(linecode.opcode) == true) {		//search OPTAB for OPCODE 
															//if found then - OPpos�� ��ġ ���� 
															//begin
				lineopcode = OP[OPpos].OPNUM;		//opcode�� 10������ lineopcode�� ����

				if (linecode.operand[0] != '\0') {	//If there is a symbol in OPERAND field then 
													//begin
					if (SearchSymtab(linecode.operand) == true) {	//Search SYMTAB for OPERAND //if found then
																	//store symbol value as operand address
						operandAddress = sym[symIndex].address;
						if (strcmp(linecode.operand, "BUFFER,X") == 0) {	//BUFFER,X�� 9039�� object �ڵ忡 ǥ���ȴ�
							operandAddress = 36921;	//16���� ��ȯ�� 9039
						}
					}
					else {
						//begin
						operandAddress = 0; //store 0 as operand address
						errorflag++;	//set error flag(undefined symbol
					}
				}
				else {
					operandAddress = 0;	//store 0 as operand address
				}
				sprintf(sumAddress, "%02X%04X", lineopcode, operandAddress);	//assemble the object code instruction
			}
			else if (strcmp(linecode.opcode, "BYTE") == 0) {	//else if OPCODE = 'BYTE' or 'WORD' then 
																//(assembler directive�� ��ã�� ��� - ������ ���)
				if (strcmp(linecode.operand, "C'EOF'") == 0) {	//convert constant to object code
					sprintf(sumAddress, "%s", "454F46");		//C'EOF'�� object code
					sumAddress[6] = NULL;
				}
				else if (linecode.operand[0] == 'X') {	//convert constant to object code - X'F1',X'05'�� ���
					sprintf(sumAddress, "%c%c", linecode.operand[2], linecode.operand[3]);
					sumAddress[2] = NULL;
				}
			}
			else if (strcmp(linecode.opcode, "WORD") == 0) {	//else if OPCODE = 'BYTE' or 'WORD' then 
				if (strcmp(linecode.label, "MAXLEN") == 0) {
					temp = strtol(linecode.operand, NULL, 16);
					itoa(temp, WordsAdd, 10);	//10������ �ٲ��ش�
					sprintf(sumAddress, "00%s", WordsAdd);	//001000
				}
				else {
					sprintf(sumAddress, "%06s", linecode.operand);	//operand�� �ٷ� ���ش�
				}
			}
			else if (strcmp(linecode.opcode, "RESW") == 0) {
				sprintf(sumAddress, "%s", "");
			}
			else if (strcmp(linecode.opcode, "RESB") == 0) {
				sprintf(sumAddress, "%s", "");
			}
			if (strcmp(linecode.label, "\t") == 0 && strcmp(linecode.operand, "\t") == 0)//label�� operand�� ���� ���
			{
				fprintf(list, "%04X\t\t%s\t%s\t%s\n", linecode.LOCCTR, linecode.opcode, linecode.operand, sumAddress);
			}
			else if (strcmp(linecode.label, "\t") == 0)		//label�� ���� ���
			{
				if (strcmp(linecode.operand, "BUFFER,X") == 0)
					fprintf(list, "%04X\t\t%s\t%s\t%s\n", linecode.LOCCTR, linecode.opcode, linecode.operand, sumAddress);
				else
					fprintf(list, "%04X\t\t%s\t%s\t\t%s\n", linecode.LOCCTR, linecode.opcode, linecode.operand, sumAddress);
			}
			else {
				fprintf(list, "%04X\t%s\t%s\t%s\t\t%s\n", linecode.LOCCTR, linecode.label, linecode.opcode, linecode.operand, sumAddress);
			}

			strcat(ObjectLine, sumAddress);	//Object Program�� ������ object�ڵ带 �ֱ� ���� objectLine�̶�� ���ڿ� ���� ������ sumAddress�� ��� �̾�ٿ��ش�.
			Objnum += strlen(sumAddress);

			if (strcmp(linecode.opcode, "RESB") == 0) {
				Objnum += strtol(linecode.operand, NULL, 16);
			}
			if (strcmp(linecode.opcode, "RESW") == 0) {
				Objnum += strtol(linecode.operand, NULL, 16) * 3;
			}
			if (Objnum > 55) {
				fprintf(obj, "T%06X%02X%s\n", StartAddress, strlen(ObjectLine) / 2, ObjectLine);
				start = true;
				for (int i = 0; i < 61; i++) {
					ObjectLine[i] = '\0';
				}
				Objnum = 0;
			}
		}
		else
			fprintf(list, "\t%s\t%s\n", linecode.label, linecode.opcode);	//write listing line
		fgets(line, 500, intermediate);		//read next input line
		linecode = Tokenize_again(line);
	}
	fprintf(obj, "T%06X%02X%s\n", StartAddress, strlen(ObjectLine) / 2, ObjectLine);
	if (strcmp(linecode.opcode, "END") == 0) {	//end
		SearchSymtab(linecode.operand);
		fprintf(obj, "E%06X", sym[symIndex].address);	//write last Text record to object program
		fprintf(list, "\t\t%s\t%s", linecode.opcode, linecode.operand);	//write end record to object program

	}
	fclose(obj);
	fclose(list);
	fclose(intermediate);

}

LINE Tokenize_again(char *read) { //�Է� ���� ���� ��Ȳ�� �°� 4���� ����
	char *temp;
	char first[10] = "";
	char second[10] = "";
	char third[10] = "";
	char fourth[10] = "";
	int i = 0;

	LINE saveline = { 0 };
	temp = strtok(read, "\t\n");
	strcpy(first, temp);
	if (strcmp(first, ".") == 0) { //�ּ��� ���
		strcpy(saveline.label, first);
		temp = strtok(NULL, "\n");
		if (temp != NULL) {
			strcpy(saveline.opcode, temp);
			strcpy(saveline.operand, "\t");
		}
		printf("%s", saveline.opcode);
		return saveline;
	}
	else { //�ּ��� �ƴ� ���
		while (temp != NULL) {
			if (i == 1) strcpy(second, temp);
			else if (i == 2) strcpy(third, temp);
			else if (i == 3) strcpy(fourth, temp);
			i++;
			temp = strtok(NULL, " .\t\n");
		} //��ū�� ������ ���� ������
		if (strcmp(first, "END") == 0) {
			strcpy(saveline.opcode, first);
			strcpy(saveline.operand, second);
			return saveline;
		}
		if (i == 2) { //opcode, LOCCTR
			strcpy(saveline.label, "\t");
			strcpy(saveline.opcode, first);
			strcpy(saveline.operand, "\t");
			saveline.LOCCTR = (int)strtol(second, NULL, 16);
		}
		else if (i == 3) { //opcode, operand, LOCCTR
			strcpy(saveline.label, "\t");
			strcpy(saveline.opcode, first);
			strcpy(saveline.operand, second);
			saveline.LOCCTR= (int)strtol(third, NULL, 16);
		}
		else if (i == 4) { //lable, opcode, operand, LOCCTR
			strcpy(saveline.label, first);
			strcpy(saveline.opcode, second);
			strcpy(saveline.operand, third);
			saveline.LOCCTR= (int)strtol(fourth, NULL, 16);
		}
		return saveline;
	}
}
