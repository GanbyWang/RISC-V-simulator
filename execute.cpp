// v2.0
// Totally rewritten

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <map>
#include <iterator>
#include "riscv.h"
using namespace std;

extern int COUNT;
extern map<string, int> instr_count;
extern long long instr_num;

void instruction_execute(int instruction)
{
	if(COUNT)
	{
		instr_num++;
	}
	string name;

	// The following variables are used to store different fields
	long long opcode = OPCODE(instruction);
	int rd = RD(instruction);
	int rs1 = RS1(instruction);
	int rs2 = RS2(instruction);
	int rs3 = RS3(instruction);
	long long rm = RM(instruction);
	long long shamt = SHAMT(instruction);			// shamt is used in shifts in 32-bit
	long long shamt_64 = SHAMT_64(instruction);		// shamt_64 is used in shifts in 64-bit
	long long u_imm = U_IMM(instruction);
	long long j_imm = J_IMM(instruction);
	long long i_imm = I_IMM(instruction);
	long long s_imm = S_IMM(instruction);
	long long b_imm = B_IMM(instruction);
	long long fmt = FMT(instruction);
	long long funct3 = FUNCT3(instruction);
	long long funct7 = FUNCT7(instruction);
	long long funct7_s = FUNCT7_S(instruction);		// s stands for special, it only apears in shifts in 64-bit

	switch (opcode)
	{
	case 0b0110111:			// LUI
		WRITE_IREG_64_S(rd, u_imm);
		name="LUI";
		break;

	case 0b0010111:			// AUIPC
		WRITE_IREG_64_S(rd, READ_PC + u_imm);
		name="AUIPC";
		break;

	case 0b1101111:			// JAL
		WRITE_IREG_64_S(rd, READ_PC + 4);
		WRITE_PC(READ_PC + j_imm);
		name="JAL";
		break;

	case 0b1100111:			// JALR
		WRITE_IREG_64_S(rd, READ_PC + 4);
		WRITE_PC(READ_IREG_64_S(rs1) + i_imm);
		WRITE_PC(READ_PC - READ_PC % 2);	// in order to set the least bit to 0
		name="JALR";
		break;

	case 0b1100011:			// BRANCH series
		switch (funct3)
		{
		case 0b000:			// BEQ
			if (READ_IREG_64_S(rs1) == READ_IREG_64_S(rs2))
			{
				WRITE_PC(READ_PC + b_imm);
			}
			name="BEQ";
			break;

		case 0b001:			// BNE
			if (READ_IREG_64_S(rs1) != READ_IREG_64_S(rs2))
			{
				WRITE_PC(READ_PC + b_imm);
			}
			name="BNE";
			break;

		case 0b100:			// BLT
			if (READ_IREG_64_S(rs1) < READ_IREG_64_S(rs2))
			{
				WRITE_PC(READ_PC + b_imm);
			}
			name="BLT";
			break;

		case 0b101:			// BGE
			if (READ_IREG_64_S(rs1) >= READ_IREG_64_S(rs2))
			{
				WRITE_PC(READ_PC + b_imm);
			}
			name="BGE";
			break;

		case 0b110:			// BLTU
			if (READ_IREG_64_U(rs1) < READ_IREG_64_U(rs2))
			{
				WRITE_PC(READ_PC + b_imm);
			}
			name="BLTU";
			break;

		case 0b111:			// BGEU
			if (READ_IREG_64_U(rs1) >= READ_IREG_64_U(rs2))
			{
				WRITE_PC(READ_PC + b_imm);
			}
			name="BGEU";
			break;

		default:
			printf("Undefined branch %x\r\n", instruction);
			break;
		}
		break;

	case 0b0000011:			// LOAD series
		switch (funct3)
		{
		case 0b000:			// LB
			WRITE_IREG_8_S(rd, READ_MEM_8(READ_IREG_64_S(rs1) + i_imm));
			name="LB";
			break;

		case 0b001:			// LH
			WRITE_IREG_16_S(rd, READ_MEM_16(READ_IREG_64_S(rs1) + i_imm));
			name="LH";
			break;

		case 0b010:			// LW
			WRITE_IREG_32_S(rd, READ_MEM_32(READ_IREG_64_S(rs1) + i_imm));
			name="LW";
			break;

		case 0b100:			// LBU
			WRITE_IREG_8_U(rd, READ_MEM_8(READ_IREG_64_S(rs1) + i_imm));
			name="LBU";
			break;

		case 0b101:			// LHU
			WRITE_IREG_16_U(rd, READ_MEM_16(READ_IREG_64_S(rs1) + i_imm));
			name="LHU";
			break;

		case 0b110:			// LWU
			WRITE_IREG_32_U(rd, READ_MEM_32(READ_IREG_64_S(rs1) + i_imm));
			name="LWU";
			break;

		case 0b011:			// LD
			WRITE_IREG_64_S(rd, READ_MEM_64(READ_IREG_64_S(rs1) + i_imm));
			name="LD";
			break;

		default:
			printf("Undefined load %x\r\n", instruction);
			break;
		}
		break;

	case 0b0100011:			// STORE series
		switch (funct3)
		{
		case 0b000:			// SB
			WRITE_MEM_8(READ_IREG_64_S(rs1) + s_imm, READ_IREG_8_U(rs2));
			name="SB";
			break;

		case 0b001:			// SH
			WRITE_MEM_16(READ_IREG_64_S(rs1) + s_imm, READ_IREG_16_U(rs2));
			name="SH";
			break;

		case 0b010:			// SW
			WRITE_MEM_32(READ_IREG_64_S(rs1) + s_imm, READ_IREG_32_U(rs2));
			name="SW";
			break;

		case 0b011:			// SD
			WRITE_MEM_64(READ_IREG_64_S(rs1) + s_imm, READ_IREG_64_U(rs2));
			name="SD";
			break;

		default:
			printf("Undefined store %x\r\n", instruction);
			break;
		}
		break;

	case 0b0010011:			// IMM-CAL series
		switch (funct3)
		{
		case 0b000:			// ADDI
			WRITE_IREG_64_S(rd, READ_IREG_64_S(rs1) + i_imm);
			name="ADDI";
			break;

		case 0b010:			// SLTI
			if (READ_IREG_64_S(rs1) < i_imm)
			{
				WRITE_IREG_64_S(rd, 1);
			}
			else
			{
				WRITE_IREG_64_S(rd, 0);
			}
			name="SLTI";
			break;

		case 0b011:			// SLTIU
			if (READ_IREG_64_U(rs1) < (unsigned long long)i_imm)
			{
				WRITE_IREG_64_S(rd, 1);
			}
			else
			{
				WRITE_IREG_64_S(rd, 0);
			}
			name="SLTIU";
			break;

		case 0b100:			// XORI
			WRITE_IREG_64_U(rd, READ_IREG_64_U(rs1) ^ (unsigned long long)i_imm);
			name="XORI";
			break;

		case 0b110:			// ORI
			WRITE_IREG_64_U(rd, READ_IREG_64_U(rs1) | (unsigned long long)i_imm);
			name="ORI";
			break;

		case 0b111:			// ANDI
			WRITE_IREG_64_U(rd, READ_IREG_64_U(rs1) & (unsigned long long)i_imm);
			name="ANDI";
			break;

		case 0b001:			// SLLI
			WRITE_IREG_64_U(rd, READ_IREG_64_U(rs1) << shamt_64);
			name="SLLI";
			break;

		case 0b101:			// SRLI&SRAI
			if (funct7_s == 0)	// SRLI
			{
				WRITE_IREG_64_U(rd, READ_IREG_64_U(rs1) >> shamt_64);
				name="SRLI";
			}
			else				// SRAI
			{
				WRITE_IREG_64_S(rd, READ_IREG_64_S(rs1) >> shamt_64);
				name="SRAI";
			}
			break;

		default:
			printf("Undefined immediate calculation %x\r\n", instruction);
			break;
		}
		break;

	case 0b0110011:			// REG-CAL series
		switch (funct7)
		{
		case 0b0000001:		// MUL&DIV series
			switch (funct3)
			{
			case 0b000:		// MUL
				WRITE_IREG_64_S(rd, READ_IREG_64_S(rs1)*READ_IREG_64_S(rs2));
				name="MUL";
				break;

			case 0b100:		// DIV
				if (READ_IREG_64_S(rs1) == ((long long)1) << 63 && READ_IREG_64_S(rs2) == -1)
				{
					WRITE_IREG_64_S(rd, ((long long)1) << 63);
				}
				else if (READ_IREG_64_S(rs2) == 0)
				{
					WRITE_IREG_64_S(rd, -1);
				}
				else
				{
					WRITE_IREG_64_S(rd, READ_IREG_64_S(rs1) / READ_IREG_64_S(rs2));
				}
				name="DIV";
				break;

			case 0b101:		// DIVU
				if (READ_IREG_64_U(rs2) == 0)
				{
					WRITE_IREG_64_U(rd, ((unsigned long long)1) << 63);
				}
				else
				{
					WRITE_IREG_64_U(rd, READ_IREG_64_U(rs1) / READ_IREG_64_U(rs2));
				}
				name="DIVU";
				break;

			case 0b110:		// REM
				if (READ_IREG_64_S(rs1) == ((long long)1) << 63 && READ_IREG_64_S(rs2) == -1)
				{
					WRITE_IREG_64_S(rd, 0);
				}
				else if (READ_IREG_64_S(rs2) == 0)
				{
					WRITE_IREG_64_S(rd, READ_IREG_64_S(rs1));
				}
				else
				{
					WRITE_IREG_64_S(rd, READ_IREG_64_S(rs1) % READ_IREG_64_S(rs2));
				}
				name="REM";
				break;

			case 0b111:		// REMU
				if (READ_IREG_64_U(rs2) == 0)
				{
					WRITE_IREG_64_U(rd, READ_IREG_64_U(rs1));
				}
				else
				{
					WRITE_IREG_64_U(rd, READ_IREG_64_U(rs1) % READ_IREG_64_U(rs2));
				}
				name="REMU";
				break;

			default:
				printf("Undefined multiplication or divsion %x\r\n", instruction);
				break;
			}
			break;

		default:			// other calculation instructions
			switch (funct3)
			{
			case 0b000:		// ADD&SUB
				if (funct7 == 0)		// ADD
				{
					WRITE_IREG_64_S(rd, READ_IREG_64_S(rs1) + READ_IREG_64_S(rs2));
					name="ADD";
				}
				else					// SUB
				{
					WRITE_IREG_64_S(rd, READ_IREG_64_S(rs1) - READ_IREG_64_S(rs2));
					name="SUB";
				}
				break;

			case 0b001:		// SLL
				WRITE_IREG_64_S(rd, READ_IREG_64_S(rs1) << (READ_IREG_64_S(rs2) & 63));
				name="SLL";
				break;

			case 0b010:		// SLT
				if (READ_IREG_64_S(rs1) < READ_IREG_64_S(rs2))
				{
					WRITE_IREG_64_S(rd, 1);
				}
				else
				{
					WRITE_IREG_64_S(rd, 0);
				}
				name="SLT";
				break;

			case 0b011:		// SLTU
				if (READ_IREG_64_U(rs1) < READ_IREG_64_U(rs2))
				{
					WRITE_IREG_64_S(rd, 1);
				}
				else
				{
					WRITE_IREG_64_S(rd, 0);
				}
				name="SLTU";
				break;

			case 0b100:		// XOR
				WRITE_IREG_64_U(rd, READ_IREG_64_U(rs1) ^ READ_IREG_64_U(rs2));
				name="XOR";
				break;

			case 0b101:		// SRL&SRA
				if (funct7 == 0)		// SRL
				{
					WRITE_IREG_64_U(rd, READ_IREG_64_U(rs1) >> (READ_IREG_64_U(rs2) & 63));
					name="SRL";
				}
				else					// SRA
				{
					WRITE_IREG_64_S(rd, READ_IREG_64_S(rs1) >> (READ_IREG_64_S(rs2) & 63));
					name="SRA";
				}
				break;

			case 0b110:		// OR
				WRITE_IREG_64_U(rd, READ_IREG_64_U(rs1) | READ_IREG_64_U(rs2));
				name="OR";
				break;

			case 0b111:		// AND
				WRITE_IREG_64_U(rd, READ_IREG_64_U(rs1) & READ_IREG_64_U(rs2));
				name="AND";
				break;

			default:
				printf("Undefined register calculations %x\r\n", instruction);
				break;
			}
		}
		break;

	case 0b1110011:
		if (rs2 == 0)		// ECALL
		{
			ecall();
			name="ECALL";
		}
		else
		{
			printf("You need to write EBREAK\r\n");
		}
		break;

	case 0b0011011:			// WORD-IMM series
		switch (funct3)
		{
		case 0b000:			// ADDIW
			WRITE_IREG_32_S(rd, READ_IREG_32_S(rs1) + (int)i_imm);
			name="ADDIW";
			break;

		case 0b001:			// SLLIW
			WRITE_IREG_32_S(rd, READ_IREG_32_S(rs1) << shamt);
			name="SLLIW";
			break;

		case 0b101:			// SRLIW&SRAIW
			if (funct7 == 0)		// SRLIW
			{
				WRITE_IREG_32_S(rd, READ_IREG_32_U(rs1) >> shamt);
				name="SRLIW";
			}
			else					// SRAIW
			{
				WRITE_IREG_32_S(rd, READ_IREG_32_S(rs1) >> shamt);
				name="SRAIW";
			}
			break;

		default:
			printf("Undefined WORD-IMM instruction %x\r\n", instruction);
			break;
		}
		break;

	case 0b0111011:			// REG-CAL-WORD series
		switch (funct7)
		{
		case 1:				// MULW&DIVW series
			switch (funct3)
			{
			case 0b000:		// MULW
				WRITE_IREG_32_S(rd, READ_IREG_32_S(rs1)*READ_IREG_32_S(rs2));
				name="MULW";
				break;

			case 0b100:		// DIVW
				if (READ_IREG_32_S(rs1) == 1 << 31 && READ_IREG_32_S(rs2) == -1)
				{
					WRITE_IREG_32_S(rd, 1 << 31);
				}
				else if (READ_IREG_32_S(rs2) == 0)
				{
					WRITE_IREG_32_S(rd, -1);
				}
				else
				{
					WRITE_IREG_32_S(rd, READ_IREG_32_S(rs1) / READ_IREG_32_S(rs2));
				}
				name="DIVW";
				break;

			case 0b101:		// DIVUW
				if (READ_IREG_32_U(rs2) == 0)
				{
					WRITE_IREG_32_S(rd, ((unsigned long long)1) << 31);
				}
				else
				{
					WRITE_IREG_32_S(rd, READ_IREG_32_U(rs1) / READ_IREG_32_U(rs2));
				}
				name="DIVUW";
				break;

			case 0b110:		// REMW
				if (READ_IREG_32_S(rs1) == 1 << 31 && READ_IREG_32_S(rs2) == -1)
				{
					WRITE_IREG_32_S(rd, 0);
				}
				else if (READ_IREG_32_S(rs2) == 0)
				{
					WRITE_IREG_32_S(rd, READ_IREG_32_S(rs1));
				}
				else
				{
					WRITE_IREG_32_S(rd, READ_IREG_32_S(rs1) % READ_IREG_32_S(rs2));
				}
				name="REMW";
				break;

			case 0b111:		// REMUW
				if (READ_IREG_32_U(rs2) == 0)
				{
					WRITE_IREG_32_S(rd, READ_IREG_32_U(rs1));
				}
				else
				{
					WRITE_IREG_32_S(rd, READ_IREG_32_U(rs1) % READ_IREG_32_U(rs2));
				}
				name="REMUW";
				break;

			default:
				printf("Undefined MULW or DIVW instructions %x\r\n", instruction);
				break;
			}
			break;

		default:			// other REG-CAL-WORD instructions
			switch (funct3)
			{
			case 0b000:		// ADDW&SUBW
				if (funct7 == 0)		// ADDW
				{
					WRITE_IREG_32_S(rd, READ_IREG_32_S(rs1) + READ_IREG_32_S(rs2));
					name="ADDW";
				}
				else					// SUBW
				{
					WRITE_IREG_32_S(rd, READ_IREG_32_S(rs1) - READ_IREG_32_S(rs2));
					name="SUBW";
				}
				break;

			case 0b001:		// SLLW
				WRITE_IREG_32_S(rd, READ_IREG_32_S(rs1) << (READ_IREG_32_S(rs2) & 31));
				name="SLLW";
				break;

			case 0b101:		// SRLW&SRAW
				if (funct7 == 0)		// SRLW
				{
					WRITE_IREG_32_S(rd, READ_IREG_32_U(rs1) >> (READ_IREG_32_U(rs2) & 31));
					name="SRLW";
				}
				else					// SRAW
				{
					WRITE_IREG_32_S(rd, READ_IREG_32_S(rs1) >> (READ_IREG_32_S(rs2) & 31));
					name="SRAW";
				}
				break;

			default:
				printf("Undefined REG-CAL-WORD instructions %x\r\n", instruction);
				break;
			}
			break;
		}
		break;

	case 0b0000111:			// FLOAT-LOAD series
		switch (rm)
		{
		case 0b010:			// FLW
			WRITE_FREG_F(rd, READ_MEM_F(READ_IREG_64_S(rs1) + i_imm));
			name="FLW";
			break;

		case 0b011:			// FLD
			WRITE_FREG_D(rd, READ_MEM_D(READ_IREG_64_S(rs1) + i_imm));
			name="FLD";
			break;

		default:
			printf("undefined FLOAT-LOAD instructions %x\r\n", instruction);
			break;
		}
		break;

	case 0b0100111:			// FLOAT-STORE series
		switch (rm)
		{
		case 0b010:			// FSW
			WRITE_MEM_F(READ_IREG_64_S(rs1) + s_imm, READ_FREG_F(rs2));
			name="FSW";
			break;

		case 0b011:			// FSD
			WRITE_MEM_D(READ_IREG_64_S(rs1) + s_imm, READ_FREG_D(rs2));
			name="FSD";
			break;

		default:
			printf("Undefined FLOAT-STORE instruction %x\r\n", instruction);
			break;
		}
		break;

	case 0b1000011:			// FMADD.S&FMADD.D
		switch (funct7&3)
		{
		case 0b00:			// FMADD.S
			WRITE_FREG_F(rd, READ_FREG_F(rs1)*READ_FREG_F(rs2) + READ_FREG_F(rs3));
			name="FMADD.S";
			break;

		case 0b01:			// FMADD.D
			WRITE_FREG_D(rd, READ_FREG_D(rs1)*READ_FREG_D(rs2) + READ_FREG_D(rs3));
			name="FMADD.D";
			break;

		default:
			printf("Undefined FMADD instruction %x\r\n", instruction);
			break;
		}
		break;

	case 0b1000111:			// FMSUB.S&FMSUB.D
		switch (funct7 & 3)
		{
		case 0b00:			// FMSUB.S
			WRITE_FREG_F(rd, READ_FREG_F(rs1)*READ_FREG_F(rs2) - READ_FREG_F(rs3));
			name="FMSUB.S";
			break;

		case 0b01:			// FMSUB.D
			WRITE_FREG_D(rd, READ_FREG_D(rs1)*READ_FREG_D(rs2) - READ_FREG_D(rs3));
			name="FMSUB.D";
			break;

		default:
			printf("Undefined FMSUB instruction %x\r\n", instruction);
			break;
		}
		break;

	case 0b1001011:			// FNMSUB.S&FNMSUB.D
		switch (funct7 & 3)
		{
		case 0b00:			// FNMSUB.S
			WRITE_FREG_F(rd, -(READ_FREG_F(rs1)*READ_FREG_F(rs2) - READ_FREG_F(rs3)));
			name="FNMSUB.S";
			break;

		case 0b01:			// FNMSUB.D
			WRITE_FREG_D(rd, -(READ_FREG_D(rs1)*READ_FREG_D(rs2) - READ_FREG_D(rs3)));
			name="FNMSUB.D";
			break;

		default:
			printf("Undefined FNMSUB instruction %x\r\n", instruction);
			break;
		}
		break;

	case 0b1001111:			// FNMADD.S&FNMADD.D
		switch (funct7 & 3)
		{
		case 0b00:			// FMADD.S
			WRITE_FREG_F(rd, -(READ_FREG_F(rs1)*READ_FREG_F(rs2) + READ_FREG_F(rs3)));
			name="FMADD.S";
			break;

		case 0b01:			// FMADD.D
			WRITE_FREG_D(rd, -(READ_FREG_D(rs1)*READ_FREG_D(rs2) + READ_FREG_D(rs3)));
			name="FMADD.D";
			break;

		default:
			printf("Undefined FMADD instruction %x\r\n", instruction);
			break;
		}
		break;

	case 0b1010011:			// a huge seires of FLOAT
		switch (funct7)
		{
		case 0b0000000:		// FADD.S
			WRITE_FREG_F(rd, READ_FREG_F(rs1) + READ_FREG_F(rs2));
			name="FADD.S";
			break;

		case 0b0000100:		// FSUB.S
			WRITE_FREG_F(rd, READ_FREG_F(rs1) - READ_FREG_F(rs2));
			name="FSUB.S";
			break;

		case 0b0001000:		// FMUL.S
			WRITE_FREG_F(rd, READ_FREG_F(rs1) * READ_FREG_F(rs2));
			name="FMUL.S";
			break;

		case 0b0001100:		// FDIV.S
			WRITE_FREG_F(rd, READ_FREG_F(rs1) / READ_FREG_F(rs2));
			name="FDIV.S";
			break;

		case 0b0101100:		// FSQRT.S
			WRITE_FREG_F(rd, sqrt(READ_FREG_F(rs1)));
			name="FSQRT.S";
			break;

		case 0b1101000:		// FCVT.S series
			switch (rs2)
			{
			case 0b00000:	// FCVT.S.W
				WRITE_FREG_F(rd, READ_IREG_32_S(rs1));
				name="FCVT.S.W";
				break;

			case 0b00001:	// FCVT.S.WU
				WRITE_FREG_F(rd, READ_IREG_32_U(rs1));
				name="FCVT.S.WU";
				break;

			case 0b00010:	// FCVT.S.L
				WRITE_FREG_F(rd, READ_IREG_64_S(rs1));
				name="FCVT.S.L";
				break;

			case 0b00011:	// FCVT.S.LU
				WRITE_FREG_F(rd, READ_IREG_64_U(rs1));
				name="FCVT.S.LU";
				break;

			default:
				printf("Undefined FCVT.S instruction %x\r\n", instruction);
				break;
			}
			break;

		case 0b1111000:		// FMV.S.X
			COPY_INT_TO_F(rd, READ_IREG_32_S(rs1));
			name="FMV.S.X";
			break;

		case 0b0000001:		// FADD.D
			WRITE_FREG_D(rd, READ_FREG_D(rs1) + READ_FREG_D(rs2));
			name="FADD.D";
			break;

		case 0b0000101:		// FSUB.D
			WRITE_FREG_D(rd, READ_FREG_D(rs1) - READ_FREG_D(rs2));
			name="FSUB.D";
			break;

		case 0b0001001:		// FMUL.D
			WRITE_FREG_D(rd, READ_FREG_D(rs1) * READ_FREG_D(rs2));
			name="FMUL.D";
			break;

		case 0b0001101:		// FDIV.D
			WRITE_FREG_D(rd, READ_FREG_D(rs1) / READ_FREG_D(rs2));
			name="FDIV.D";
			break;

		case 0b0101101:		// FSQRT.D
			WRITE_FREG_D(rd, sqrt(READ_FREG_D(rs1)));
			name="FSQRT.D";
			break;

		case 0b0100000:		// FCVT.S.D
			WRITE_FREG_F(rd, READ_FREG_D(rs1));
			name="FCVT.S.D";
			break;

		case 0b0100001:		// FCVT.D.S
			WRITE_FREG_D(rd, READ_FREG_F(rs1));
			name="FCVT.D.S";
			break;

		case 0b1010001:		// FEQ.D&FLT.D&FLE.D
			switch (rm)
			{
			case 0b010:		// FEQ.D
				if (READ_FREG_D(rs1) == NAN || READ_FREG_D(rs2) == NAN)
				{
					WRITE_IREG_64_S(rd, 0);
				}
				else
				{
					if (READ_FREG_D(rs1) == READ_FREG_D(rs2))
					{
						WRITE_IREG_64_S(rd, 1);
					}
					else
					{
						WRITE_IREG_64_S(rd, 0);
					}
				}
				name="FEQ.D";
				break;

			case 0b001:		// FLT.D
				if (READ_FREG_D(rs1) == NAN || READ_FREG_D(rs2) == NAN)
				{
					WRITE_IREG_64_S(rd, 0);
				}
				else
				{
					if (READ_FREG_D(rs1) < READ_FREG_D(rs2))
					{
						WRITE_IREG_64_S(rd, 1);
					}
					else
					{
						WRITE_IREG_64_S(rd, 0);
					}
				}
				name="FLT.D";
				break;

			case 0b00:		// FLE.D
				if (READ_FREG_D(rs1) == NAN || READ_FREG_D(rs2) == NAN)
				{
					WRITE_IREG_64_S(rd, 0);
				}
				else
				{
					if (READ_FREG_D(rs1) <= READ_FREG_D(rs2))
					{
						WRITE_IREG_64_S(rd, 1);
					}
					else
					{
						WRITE_IREG_64_S(rd, 0);
					}
				}
				name="FLE.D";
				break;

			default:
				printf("Undefined double compare instruction %x\r\n", instruction);
				break;
			}
			break;

		case 0b1100001:		// FCVT.?.D series
			switch (rs2)
			{
			case 0b00000:	// FCVT.W.D
				WRITE_IREG_32_S(rd, READ_FREG_D(rs1));
				name="FCVT.W.D";
				break;

			case 0b00001:	// FCVT.WU.D
				WRITE_IREG_32_U(rd, READ_FREG_D(rs1));
				name="FCVT.WU.D";
				break;

			case 0b00010:	// FCVT.L.D
				WRITE_IREG_64_S(rd, READ_FREG_D(rs1));
				name="FCVT.L.D";
				break;

			case 0b00011:	// FCVT.LU.D
				WRITE_IREG_64_U(rd, READ_FREG_D(rs1));
				name="FCVT.LU.D";
				break;

			default:
				printf("Undefined FCVT.?.D instruction %x\r\n", instruction);
				break;
			}
			break;

		case 0b1101001:		// FCVT.D.? series
			switch (rs2)
			{
			case 0b00000:	// FCVT.D.W
				WRITE_FREG_D(rd, READ_IREG_32_S(rs1));
				name="FCVT.D.W";
				break;

			case 0b00001:	// FCVT.D.WU
				WRITE_FREG_D(rd, READ_IREG_32_U(rs1));
				name="FCVT.D.WU";
				break;

			case 0b00010:	// FCVT.D.L
				WRITE_FREG_D(rd, READ_IREG_64_S(rs1));
				name="FCVT.D.L";
				break;

			case 0b00011:	// FCVT.D.LU
				WRITE_FREG_D(rd, READ_IREG_64_U(rs1));
				name="FCVT.D.LU";
				break;

			default:
				printf("Undefined FCVT.D.? instruction %x\r\n", instruction);
				break;
			}
			break;

		case 0b1110001:		// FMV.X.D
			COPY_D_TO_LONG(rd, READ_FREG_D(rs1));
			name="FMV.X.D";
			break;

		case 0b1111001:		// FMV.D.X
			COPY_LONG_TO_D(rd, READ_IREG_64_S(rs1));
			name="FMV.D.X";
			break;
		
		case 0b0010001:		// FSGNJ
			switch(funct3)
			{
			case 0b000:	// FSGNJ.D
				WRITE_FREG_D(rd, READ_FREG_D(rs1));
				name="FSGNJ.D";
				break;

			case 0b001:	// FSGNJN.D
				WRITE_FREG_D(rd, -READ_FREG_D(rs1));
				name="FSGNJN.D";
				break;				

			default:
				printf("Undefined FSGNJ instruction %x\r\n", instruction);
			}
			break;

		default:
			printf("Undefined FLOAT instruction %x\r\n", instruction);
			break;
		}
		break;

	default:
		printf("Undefined instruction %x\r\n", instruction);
		exit(0);
		break;
	}

	if(COUNT)
	{
		map<string, int>::iterator it;
		if (it != instr_count.end())
		{
			instr_count[name]++;
		}
		else
		{
			instr_count.insert(pair<string, int>(name, 1));
		}
	}
}
