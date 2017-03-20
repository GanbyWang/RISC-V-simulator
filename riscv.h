#ifndef RISCV_H
#define RISCV_H

#include <memory.h>
#include <map>
#include <string>

using namespace std;

#define MEM		(1<<28)
#define STACK		(0x0efffb50)
#define OPCODE(x)	(x&127)
#define RD(x)		((x>>7)&31)
#define RS1(x)		((x>>15)&31)
#define RS2(x)		((x>>20)&31)
#define RS3(x)		((x>>27)&31)
#define RM(x)		((x>>12)&7)
#define FMT(x)		((x>>25)&3)
#define FUNCT3(x)	((x>>12)&7)
#define FUNCT7(x)	((x>>25)&127)
#define FUNCT7_S(x)	((x>>26)&63)
#define SHAMT(x)	((x>>20)&31)
#define SHAMT_64(x)	((x>>20)&63)

#define I_IMM(x)	(((x&(1<<31))>>20)|(((x>>25)&63)<<5)|(((x>>21)&15)<<1)|(x>>20&1))
#define S_IMM(x)	(((x&(1<<31))>>20)|(((x>>25)&63)<<5)|(((x>>8)&15)<<1)|(x>>7&1))
#define B_IMM(x)	(((x&(1<<31))>>19)|((x>>7&1)<<11)|(((x>>25)&63)<<5)|(((x>>8)&15)<<1))
#define U_IMM(x)	(x&0xfffff000)
#define J_IMM(x)	(((x&(1<<31))>>11)|(((x>>12)&255)<<12)|(((x>>20)&1)<<11)|(((x>>25)&63)<<5)|(((x>>21)&15)<<1))


#define READ_IREG_8_U(reg_num)	regs.get_8u_reg(reg_num)
#define READ_IREG_16_U(reg_num)	regs.get_16u_reg(reg_num)
#define READ_IREG_32_S(reg_num)	regs.get_32_reg(reg_num)
#define READ_IREG_32_U(reg_num)	regs.get_32u_reg(reg_num)
#define READ_IREG_64_S(reg_num)	regs.get_64_reg(reg_num)
#define READ_IREG_64_U(reg_num)	regs.get_64u_reg(reg_num)

#define READ_FREG_F(reg_num)	regs.get_float(reg_num)
#define READ_FREG_D(reg_num)	regs.get_double(reg_num)

#define WRITE_IREG_8_S(reg_num, value)	regs.write_8_reg(reg_num, value)
#define WRITE_IREG_8_U(reg_num, value)	regs.write_8u_reg(reg_num, value)
#define WRITE_IREG_16_S(reg_num, value)	regs.write_16_reg(reg_num, value)
#define WRITE_IREG_16_U(reg_num, value)	regs.write_16u_reg(reg_num, value)
#define WRITE_IREG_32_S(reg_num, value)	regs.write_32_reg(reg_num, value)
#define WRITE_IREG_32_U(reg_num, value)	regs.write_32u_reg(reg_num, value)
#define WRITE_IREG_64_S(reg_num, value)	regs.write_64_reg(reg_num, value)
#define WRITE_IREG_64_U(reg_num, value)	regs.write_64u_reg(reg_num, value)

#define WRITE_FREG_F(reg_num, value)	regs.write_float(reg_num, value)
#define WRITE_FREG_D(reg_num, value)	regs.write_double(reg_num, value)

#define READ_MEM_8(mem_addr) 	mem.get_8_mem(mem_addr)
#define READ_MEM_16(mem_addr)	mem.get_16_mem(mem_addr)
#define READ_MEM_32(mem_addr)	mem.get_32_mem(mem_addr)
#define READ_MEM_64(mem_addr)	mem.get_64_mem(mem_addr)
#define READ_MEM_F(mem_addr)	mem.get_float(mem_addr)
#define READ_MEM_D(mem_addr)	mem.get_double(mem_addr)
#define WRITE_MEM_8(mem_addr, value)	mem.write_8_mem(mem_addr, value)
#define WRITE_MEM_16(mem_addr, value)	mem.write_16_mem(mem_addr, value)
#define WRITE_MEM_32(mem_addr, value)	mem.write_32_mem(mem_addr, value)
#define WRITE_MEM_64(mem_addr, value)	mem.write_64_mem(mem_addr, value)
#define WRITE_MEM_F(mem_addr, value)	mem.write_float(mem_addr, value)
#define WRITE_MEM_D(mem_addr, value)	mem.write_double(mem_addr, value)

#define COPY_INT_TO_F(rd, value)		regs.write_float_bit(rd, value)
#define COPY_LONG_TO_D(rd, value)		regs.write_double_bit(rd, value)
#define COPY_D_TO_LONG(rd, value)		regs.write_64_bit(rd, value)

#define READ_PC	regs.read_pc()
#define WRITE_PC(value)	regs.set_pc(value)


#define zero 0  //	rrx[0]    zero    Hard-wired zero
#define ra 1    //	rrx[1]    ra      Return address
#define sp 2    //	rrx[2]    sp      Stack pointer
#define gp 3    //	rrx[3]    gp      Global pointer
#define tp 4    //	rrx[4]    tp      Thread pointer
#define t0 5    //	rrx[5]    t0      Temporaries
#define t1 6    //	rrx[6]    t1      ..
#define t2 7    //	rrx[7]    t2      ..
#define fp 8    //	rrx[8]    fp/     Frame pointer
#define s0 8    //            s0      Saved registers
#define s1 9    //	rrx[9]    s1      Saved registers
#define a0 10   //	rrx[10]   a0	  Function arguments& return value
#define a1 11   //	rrx[11]   a1 	  Function arguments& return value
#define a2 12   //	rrx[12]   a2 	  Function arguments
#define a3 13   //	rrx[13]   a3 	  ..
#define a4 14   //	rrx[14]   a4      ..
#define a5 15   //	rrx[15]   a5      ..
#define a6 16   //	rrx[16]   a6	  ..
#define a7 17   //	rrx[17]   a7 	  ..
#define s2 18   //	rrx[18]   s2	  Saved registers
#define s3 19   //	rrx[19]   s3      ..
#define s4 20   //	rrx[20]   s4      ..
#define s5 21   //	rrx[21]   s5      ..
#define s6 22   //	rrx[22]   s6      ..
#define s7 23   //	rrx[23]   s7      ..
#define s8 24   //	rrx[24]   s8      ..
#define s9 25   //	rrx[25]   s9      ..
#define s10 26   //	rrx[26]   s10	  ..
#define s11 27   //	rrx[27]   s11	  ..
#define t3 28   //	rrx[28]   t3      Temporaries
#define t4 29   //	rrx[29]   t4      ..
#define t5 30   //	rrx[30]   t5      ..
#define t6 31   //	rrx[31]   t6      ..
#define err(x)	puts(x);

extern int END;
extern int COUNT;
extern long long instr_num;
extern map<string, int> instr_count;

void ecall(void);
void instruction_execute(int);

class reg
{
	unsigned long long risc64_int[32];
	double risc64_db[32];
	int xlen;
	long long pc;
	int pc_sig;
	
public:
	reg(){
		memset(risc64_db,sizeof(risc64_db),0);
		memset(risc64_int,sizeof(risc64_db),0);
		xlen = 64;
		pc_sig = 0;
		risc64_int[sp] = STACK;
	}
	void printall(){
		for(int i = 0; i < 32; i++)
			printf("%d:%llx   ",i,risc64_int[i]);
		printf("\n");
	}
	void write_64_reg(int number, long long value){
		risc64_int[number] = value;
	}
	void write_64u_reg(int number, unsigned long long value){
		risc64_int[number] = value;
	}
	void write_32_reg(int number, int value){
		risc64_int[number] = value;
	}
	void write_32u_reg(int number, unsigned value){
		risc64_int[number] =  value;
	}
	void write_16_reg(int number, short value){
		risc64_int[number] = value;
	}
	void write_16u_reg(int number, unsigned short value){
		risc64_int[number] = value;
	}
	void write_8_reg(int number, char value){
		risc64_int[number] = value;
	}
	void write_8u_reg(int number, unsigned char value){
		risc64_int[number] = value;
	}
	void write_64_bit(int number,double value){
		risc64_int[number] = *(long long*)(&value);
	}
	void write_32_bit(int number,float value){
		risc64_int[number] = *(int*)(&value);
	}
	unsigned long long get_64u_reg(int number){
		return (unsigned long long)risc64_int[number];
	}
	unsigned int get_32u_reg(int number){
		return (unsigned int)risc64_int[number];
	}
	unsigned short get_16u_reg(int number){
		return (unsigned short)risc64_int[number];
	}
	unsigned char get_8u_reg(int number){
		return (unsigned char)risc64_int[number];
	}
	long long get_64_reg(int number){
		return (long long)risc64_int[number];
	}
	int get_32_reg(int number){
		return (int)risc64_int[number];
	}
	short get_16_reg(int number){
		return (short)risc64_int[number];
	}
	char get_8_reg(int number){
		return (char)risc64_int[number];
	}
	void init_pc(long long pc0){
		pc = pc0;
	}
	void set_pc(long long pc0){
		pc = pc0;
		pc_sig = 1;
	}
	void flush_pc(){
		if(pc_sig == 0){
			pc+=4;
		}
		pc_sig = 0;

		risc64_int[0] = 0;	// reset risc64_int[0]
	}
	long long read_pc(){
		return pc;
	}
	void write_float(int number, float value){
		// risc64_db[number] = value;
		
			float * t = (float*)(risc64_db+number);
			*t = value;
		
	}
	void write_double(int number, double value){
		risc64_db[number] = value;
	}
	float get_float(int number){
		// return (float)risc64_db[number];//direct transfer
		
		unsigned int* temp = (unsigned int*)(risc64_db + number);
		return *(float *)temp; 
		
	}
	unsigned long long get_double_bit(int number){
		return *(unsigned long long*)(risc64_db+number);
	}
	void write_float_bit(int number, int value){
		*(int*)(risc64_db+number) = value;
	}
	void write_double_bit(int number, long long value){
		*(long long*)(risc64_db+number) = value;
	}
	double get_double(int number){
		return risc64_db[number];
	}

};
extern reg regs;
/*********************/
/*********************/
/*********************/
/*********************/
/*the class of memory*/
/*********************/
/*********************/
/*********************/
/*********************/
class memory{
	char mem_space[MEM];
public:
	memory(){
		memset(mem_space,sizeof(mem_space),0);
	}
	//check subscript
	int check_sub(int subscript){
		if( subscript<0 || subscript>MEM-4 )
		{
			printf("MyRiscvSimulator: segmentation fault: access(0x%x) is out of range\r\n", subscript);
			exit(0);
		}
		return 0;
	}

	unsigned char get_8_mem(long long add){
		if(check_sub(add)){
			err("out of range!/n");
			return -1;
		}
		return mem_space[add];
	}
	unsigned short get_16_mem(long long add){
		if(check_sub(add)){
			err("out of range!/n");
			return -1;
		}
		return *(unsigned short*)(mem_space+add);
	}
	unsigned int get_32_mem(long long add){
		if(check_sub(add)){
			err("out of range!/n");
			return -1;
		}
		return *(unsigned int*)(mem_space+add);
	}
	unsigned long long get_64_mem(long long add){
		if(check_sub(add)){
			err("out of range!/n");
			return -1;
		}
		return *(unsigned long long*)(mem_space+add);
	}
	float get_float(long long add){
		if(check_sub(add)){
			err("out of range!/n");
			return -1;
		}
		return *(float*)(mem_space+add);
	}
	double get_double(long long add){
		if(check_sub(add)){
			err("out of range!/n");
			return -1;
		}
		return *(double*)(mem_space+add);
	}


	void write_8_mem(long long add,unsigned char value){
		if(check_sub(add)){
			err("out of range!/n");
			return;
		}
		mem_space[add] = value;
	}
	void write_16_mem(long long add,unsigned short value){
		if(check_sub(add)){
			err("out of range!/n");
			return;
		}
		*(unsigned short*)(mem_space+add) = value;
	}
	void write_32_mem(long long add,unsigned int value){
		if(check_sub(add)){
			err("out of range!/n");
			return;
		}
		*(unsigned int*)(mem_space+add) = value;
	}
	void write_64_mem(long long add,unsigned long long value){
		if(check_sub(add)){
			err("out of range!/n");
			return;
		}
		*(unsigned long long*)(mem_space+add) = value;
	}
	void write_float(long long add, float value){
		if(check_sub(add)){
			err("out of range!/n");
			return;
		}
		*(float*)(mem_space+add) = value;
	}
	void write_double(long long add, double value){
		if(check_sub(add)){
			err("out of range!/n");
			return;
		}
		*(double*)(mem_space+add) = value;
	}

	int get_instruction(long long add){
		if(check_sub(add)){
			err("out of range!/n");
			return -1;
		}
		if(!(add%4)){
			err("address is not aligned to 4!");
			return -1;
		}
		return (int)get_32_mem(add);
	}

	char* get_address(int index){
		return mem_space+index;
	}

};
extern memory mem; 
#else

#endif
