#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <map>
#include <iterator>
#include "riscv.h"
#include <vector>
#include <algorithm>
using namespace std;
#define PAIR pair<string,int>
#define RUN_MODE    0
#define DEBUG_MODE  1
#define PACE_MODE   2
#define TRACE_MODE  3
#define COUNT_MODE  4
struct CmpByValue {  
  bool operator()(const PAIR& lhs, const PAIR& rhs) {  
    return lhs.second > rhs.second;  
  }  
}; 
/*  Global Variables  */
memory mem;	// virtual memory
reg regs;	// register file
int END;	// flag for elf-program's exit
int COUNT;	// mark whether MODE == COUNT_MODE
map<string, int> instr_count;
long long instr_num;

/*  Local Variables  */
int MODE = 0;		// value can be RUN_MODE, DEBUG_MODE, PACE_MODE, TRACE_MODE or COUNT_MODE
FILE* fp_elf = NULL;	// file pointer towards elf-file
FILE* pc_trace = NULL;	// file to record PC trace (only in trace mode)
unsigned RUN = 0;		// count for runs (only in debug mode)

char i_reg_name[32][5] = {	"zero", "ra  ", "sp  ", "gp  ", 
				"tp  ", "t0  ", "t1  ", "t2  ", 
				"s0  ", "s1  ", "a0  ", "a1  ", 
				"a2  ", "a3  ", "a4  ", "a5  ", 
				"a6  ", "a7  ", "s2  ", "s3  ", 
				"s4  ", "s5  ", "s6  ", "s7  ", 
				"s8  ", "s9  ", "s10 ", "s11 ", 
				"t3  ", "t4  ", "t5  ", "t6  "	};

char f_reg_name[32][5] = {	"f0  ", "f1  ", "f2  ", "f3  ", 
				"f4  ", "f5  ", "f6  ", "f7  ", 
				"f8  ", "f9  ", "f10 ", "f11 ", 
				"f12 ", "f13 ", "f14 ", "f15 ", 
				"f16 ", "f17 ", "f18 ", "f19 ", 
				"f20 ", "f21 ", "f22 ", "f23 ", 
				"f24 ", "f25 ", "f26 ", "f27 ", 
				"f28 ", "f29 ", "f30 ", "f31 "  };

/*  Local Functions  */
int load_fromelf();
int excv_program();
int load_program(long long phoff, int phsize, int phnum);
// int load_section(long long shoff, int shsize, int shnum);
int mode_debuger();
int debug_help();
int mode_pace();

int main(int argc, char* argv[])
{
	if( argc<=2 )	// number of arguments is too small (at least 3)
	{
		/* printf("riscv-simulator: fatal error: no input option or file\r\n");
		// printf("riscv-simulator: invalid option -- 'y'\r\n");
		printf("Usage: ./riscv-simulator <option> <file>\r\n");
		printf("  Load and execute program from object <file>.\r\n");
		printf("  One of the following options must be given:\r\n");
		printf("    -r, --run mode     Run the whole program without pause\r\n");
		printf("    -d, --debug mode   Execute an instruction each time,\r\n");
		printf("                       wait for user's command\r\n");
		printf("    -p, --pace mode    Execute an instruction each time,\r\n");
		printf("                       print current PC, instruction and register file content,\r\n");
		printf("                       wait for 'Enter' to continue\r\n");
		printf("    -t, --trace mode   Run the whole program without pause\r\n");
		printf("                       print PC trace into file "pc_trace"\r\n");
		printf("    -c, --count mode   Count for use of each instruction\r\n"); */
		exit(0);
	}

	if( argv[1][0]!='-' )	// incorrect option format
	{
		/* printf("riscv-simulator: fatal error: no input option or file\r\n");
		// printf("riscv-simulator: invalid option -- 'y'\r\n");
		printf("Usage: ./riscv-simulator <option> <file>\r\n");
		printf("  Load and execute program from object <file>.\r\n");
		printf("  One of the following options must be given:\r\n");
		printf("    -r, --run mode     Run the whole program without pause\r\n");
		printf("    -d, --debug mode   Execute an instruction each time,\r\n");
		printf("                       wait for user's command\r\n");
		printf("    -p, --pace mode    Execute an instruction each time,\r\n");
		printf("                       print current PC, instruction and register file content,\r\n");
		printf("                       wait for 'Enter' to continue\r\n");
		printf("    -t, --trace mode   Run the whole program without pause\r\n");
		printf("                       print PC trace into file "pc_trace"\r\n");
		printf("    -c, --count mode   Count for use of each instruction\r\n"); */
		exit(0);
	}

	switch( argv[1][1] )
	{
		case 'r':
			MODE = RUN_MODE; 
			break;
		case 'd':
			MODE = DEBUG_MODE;
			break;
		case 'p':
			MODE = PACE_MODE;
			break;
		case 't':
			MODE = TRACE_MODE;
			pc_trace = fopen("pc_trace", "w");
			break;
		case 'c':
			MODE = COUNT_MODE;
			COUNT = 1;
			break;
		default:	// invalid option input
			/* printf("riscv-simulator: fatal error: no input option or file\r\n");
			// printf("riscv-simulator: invalid option -- 'y'\r\n");
			printf("Usage: ./riscv-simulator <option> <file>\r\n");
			printf("  Load and execute program from object <file>.\r\n");
			printf("  One of the following options must be given:\r\n");
			printf("    -r, --run mode     Run the whole program without pause\r\n");
			printf("    -d, --debug mode   Execute an instruction each time,\r\n");
			printf("                       wait for user's command\r\n");
			printf("    -p, --pace mode    Execute an instruction each time,\r\n");
			printf("                       print current PC, instruction and register file content,\r\n");
			printf("                       wait for 'Enter' to continue\r\n");
			printf("    -t, --trace mode   Run the whole program without pause\r\n");
			printf("                       print PC trace into file "pc_trace"\r\n");
			printf("    -c, --count mode   Count for use of each instruction\r\n"); */
			exit(0);
	}

	if( !(fp_elf = fopen(argv[2], "r")) )	// elf-file not found
	{
		printf("riscv-simulator: error: %s: No such file or directory\r\n", argv[2]);
		exit(0);
	}

	load_fromelf();		// load program from elf-file to virtual memory
	excv_program();		// execute program in virtual memory

	return 0;
}

int load_fromelf()
{
	char buf_ehdr[70] = {0};
	Elf64_Ehdr* p_ehdr = (Elf64_Ehdr*)buf_ehdr;
	
	if( !fread(buf_ehdr, 1, 64, fp_elf) )	// read elf-header
	{
		printf("riscv-simulator: file format error\r\n");
		exit(0);
	}

	/*  TEST:  check for the former 64 bytes  */
	/* unsigned* num_str_ehdr = (unsigned*)buf_ehdr;
	for(int i=0; i<16; i++)
	{
		printf("%x\r\n", num_str_ehdr[i]);
	} */

	if( !load_program( (long long)p_ehdr->e_phoff, (int)p_ehdr->e_phentsize, (int)p_ehdr->e_phnum ) )	// load from elf-file
	{
		printf("riscv-simulator: file segment error\r\n");
		exit(0);
	}

	/*  load the same content with load_program()  */
	/* if( !load_section( (long long)p_ehdr->e_shoff, (int)p_ehdr->e_shentsize, (int)p_ehdr->e_shnum ) )	// load from elf-file
	{
		printf("riscv-simulator: file section error\r\n");
		exit(0);
	} */

	if( fclose(fp_elf)==EOF )	// close elf-file
	{
		printf("riscv-simulator: file close error\r\n");
		exit(0);
	}

	regs.init_pc(p_ehdr->e_entry);	// init pc as the entry of program

	return 0;
}

int load_program(long long phoff, int phsize, int phnum)
{
	int i = 0;
	char buf_phdr[70] = {0};
	Elf64_Phdr* p_phdr = (Elf64_Phdr*)buf_phdr;

	if( !fp_elf )
	{
		return 0;
	}

	for(i=0; i<phnum; i++)
	{
		if( fseek(fp_elf, phoff+i*phsize, SEEK_SET) )	// locate at the start of a new program-header
		{
			return 0;
		}
		if( !fread(buf_phdr, 1, phsize, fp_elf) )	// read a new program-header
		{
			return 0;
		}

		if( p_phdr->p_type == PT_LOAD )		// if this segment should be load
		{
			if( fseek(fp_elf, p_phdr->p_offset, SEEK_SET) )		// locate at the start of a new segment
			{
				return 0;
			}
			if( !fread(mem.get_address(p_phdr->p_vaddr), 1, p_phdr->p_filesz, fp_elf) )	// load a new segment
			{
				return 0;
			}
		}
	}

	return 1;
}

/* int load_section(long long shoff, int shsize, int shnum)
{
	int i = 0;
	char buf_shdr[70] = {0};
	Elf64_Shdr* p_shdr = (Elf64_Shdr*)buf_shdr;

	if( !fp_elf )
	{
		return 0;
	}

	for(i=0; i<shnum; i++)
	{
		if( fseek(fp_elf, shoff+i*shsize, SEEK_SET) )	// locate at the start of a new section-header
		{
			return 0;
		}
		if( !fread(buf_shdr, 1, shsize, fp_elf) )	// read a new section-header
		{
			return 0;
		}

		if( p_shdr->sh_addr && p_shdr->sh_type!=SHT_NOBITS )	// if this section should be load
		{
			if( fseek(fp_elf, p_shdr->sh_offset, SEEK_SET) )	// locate at the start of a new section
			{
				return 0;
			}
			if( !fread(mem.get_address(p_shdr->sh_addr), 1, p_shdr->sh_size, fp_elf) )	// load a new section
			{
				return 0;
			}
		}
	}

	return 1;
} */

int excv_program()
{
	/*  TEST:  check for memory bytes  */
	/* for( ; regs.read_pc()<=0x131ec; regs.read_pc()+=4)
	{
		printf( "   0x%llx:       %08x\r\n", regs.read_pc(), *(int*)mem.get_address( regs.read_pc() ) );
	} */

	/*  TEST:  limit instruction turns  */
	/* int turn = 0;
	for( ; turn<100000 !END; turn++) */

	int turn = 0;
	while( !END )
	{
		if( regs.read_pc()>(long long)(MEM-4) || regs.read_pc()<0 )	// ensure PC is within range
		{
			printf("riscv-simulator: segmentation fault: PC(0x%llx) is out of range\r\n", regs.read_pc());
			exit(0);
		}

		switch(MODE)
		{
			case RUN_MODE:
				break;

			case DEBUG_MODE:
				mode_debuger();
				break;

			case PACE_MODE:
				mode_pace();
				break;

			case TRACE_MODE:
				if(pc_trace==NULL)
				{
					printf("riscv-simulator: fatal error: trace mode error\r\n");
					exit(0);
				}
				fprintf(pc_trace, "%llx\r\n", regs.read_pc());
				break;

			case COUNT_MODE:
				break;

			default:
				printf("riscv-simulator: fatal error: exceptional mode\r\n");
				exit(0);
		}				

		instruction_execute( mem.get_32_mem( regs.read_pc() ) );
		regs.flush_pc();	// update PC
	}

	
	if(COUNT)
	{
		printf("%lld instructions has been excuted in this run\n", instr_num);
		printf("Top 10 instructions are listed:\n");
		vector<PAIR> name_score_vec(instr_count.begin(), instr_count.end());
		sort(name_score_vec.begin(), name_score_vec.end(), CmpByValue());
		for (int i = 0; i != 10; ++i){
			printf("No.%d: %s %dtimes\n", i+1, name_score_vec[i].first.c_str(), name_score_vec[i].second);
		}
/*		for(int i=1; i<=10; i++)
		{
			map<string, int>::iterator maxi;
			map<string, int>::iterator it;
			maxi=instr_count.begin();
			for(it=instr_count.begin(); it!=instr_count.end(); it++)
			{
				if(maxi->second<it->second);
				{
					maxi=it;
				}
			}
			printf("No.%d: %s %dtimes\n", i, maxi->first.c_str(), maxi->second);
			instr_count[maxi->first]=0;
		}*/
	}

	return 0;
}

int mode_debuger()
{
	int i, j;
	int go = 0;
	char c;
	char buf[105];

	if(RUN>0)
	{
		RUN--;
		printf("\r\n");
		printf("PC: 0x%08x  Instruction: 0x%08x\r\n", (int)regs.read_pc(), *(int*)( mem.get_address( regs.read_pc() ) ));
		return 0;
	}

	while(!go)
	{
		int cnt = 0;
		char opt = -1;
		int optnum = -1;
		int opthex = -1;
		int subhex = -1;

		printf("\r\n");
		printf("PC: 0x%08x  Instruction: 0x%08x\r\n", (int)regs.read_pc(), *(int*)( mem.get_address( regs.read_pc() ) ));
		printf("(debug mode) ");

		while( cnt<100 && (c=getchar())!='\n' )
		{
			buf[cnt] = c;
			cnt++;
		}
		buf[cnt] = 0;
			
		while( c!='\n' )
		{
			c = getchar();
		}

		sscanf(buf, "%c %d", &opt, &optnum);
		sscanf(buf, "%c %x %x", &opt, &opthex, &subhex);

		switch(opt)
		{
			case 'h':
				debug_help();
				break;

			case 'q':
				exit(0);
				break;

			case 'i':
				if(optnum<0 || optnum>31)
				{
					printf("ireg:\r\n");
					for(i=0; i<8; i++)
					{
						for(j=0; j<4; j++)
						{
							printf( "%s: 0x%016llx  ", i_reg_name[i*4+j], regs.get_64_reg(i*4+j) );
						}
						printf("\r\n");
					}
				}
				else
				{
					printf( "%s: 0x%016llx\r\n", i_reg_name[optnum], regs.get_64_reg(optnum) );
				}
				break;

			case 'f':
				if(optnum<0 || optnum>31)
				{
					printf("freg:\r\n");
					for(i=0; i<8; i++)
					{
						for(j=0; j<4; j++)
						{
							printf( "%s: 0x%016llx  ", f_reg_name[i*4+j], regs.get_double_bit(i*4+j) );
						}
						printf("\r\n");
					}
				}
				else
				{
					printf( "%s: 0x%016llx\r\n", f_reg_name[optnum], regs.get_double_bit(optnum) );
				}
				break;

			case 'm':
				if( opthex<0 || opthex>MEM-4 )
				{
					printf("0x%x is out of memory range (0-0x%x)\r\n", opthex, MEM-4);
				}
				else if( subhex<=opthex || subhex>MEM-4 )
				{
					printf( "memory 0x%x: 0x%x\r\n", opthex, mem.get_32_mem(opthex) );
				}
				else
				{
					for(i=opthex; i<=subhex; i+=4)
					{
						for(j=0; j<4; j++)
						{
							printf("0x%x: 0x%x  ", i+j, (unsigned)mem.get_8_mem(i+j));
						}
						printf("\r\n");
					}					
				}
				break;

			case 'r':
				go = 1;
				if(optnum<=0)
				{
					MODE = RUN_MODE;
				}
				else
				{
					RUN = optnum-1;
				}
				break;

			case 'p':
				go = 1;
				break;

			default:
				printf("input 'h' to see help\r\n");
		}		
	}

	return 0;
}

int debug_help()
{
	printf("Usage: <option> (<argv1>) (<argv2>)\r\n");
	printf("  <option>:\r\n");
	printf("    -h, --help     See help\r\n");
	printf("    -q, --quit     Quit program\r\n");
	printf("    -i, --ireg     See all iregs with no arguments\r\n");
	printf("                   See one ireg with the first argument (ireg number)\r\n");
	printf("    -f, --freg     See all fregs with no arguments\r\n");
	printf("                   See one freg with one argument (freg number)\r\n");
	printf("    -m, --memory   See one word in memory with one argument (address)\r\n");
	printf("                   See some bytes in memory with two arguments (range)\r\n");
	printf("    -r, --run      Run without pause with no arguments\r\n");
	printf("                   Run given number of instructions with one argument (instruction numbers)\r\n");
	printf("    -p, --pace     Run current instruction\r\n");
}

int mode_pace()
{
	char c;
	int i, j;
	
	printf("PC: 0x%08x  Instruction: 0x%08x\r\n", (int)regs.read_pc(), *(int*)( mem.get_address( regs.read_pc() ) ));

	printf("ireg:\r\n");
	for(i=0; i<8; i++)
	{
		for(j=0; j<4; j++)
		{
			printf( "%s: 0x%016llx  ", i_reg_name[i*4+j], regs.get_64_reg(i*4+j) );
		}
		printf("\r\n");
	}

	printf("freg:\r\n");
	for(i=0; i<8; i++)
	{
		for(j=0; j<4; j++)
		{
			printf( "%s: 0x%016llx  ", f_reg_name[i*4+j], regs.get_double_bit(i*4+j) );
		}
		printf("\r\n");
	}
	printf("\r\n");

	while( (c=getchar())!='\n' && c != EOF );

	return 0;
}




