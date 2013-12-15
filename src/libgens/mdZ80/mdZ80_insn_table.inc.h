/***************************************************************************
 * mdZ80: Gens Z80 Emulator                                                *
 * mdZ80_insn_table.c: Z80 instruction table.                              *
 *                                                                         *
 * Copyright (c) 1999-2002 by Stéphane Dallongeville                       *
 * Copyright (c) 2003-2004 by Stéphane Akhoun                              *
 * Copyright (c) 2008-2013 by David Korth                                  *
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published by the   *
 * Free Software Foundation; either version 2 of the License, or (at your  *
 * option) any later version.                                              *
 *                                                                         *
 * This program is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License along *
 * with this program; if not, write to the Free Software Foundation, Inc., *
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.           *
 ***************************************************************************/

/* WARNING: This file should ONLY be included by mdZ80_exec.c::mdZ80_exec()! */
#if !defined(__MDZ80_IN_EXEC) || __MDZ80_IN_EXEC != 19840519
#error *** ERROR: mdZ80_insn_table.inc.h should ONLY be included by mdZ80_exec.c!
#endif

// Z80 instruction table: Single-byte instructions
static void *z80_insn_table[] = {
	&&Z80I_NOP, &&Z80I_LD_BC_NN, &&Z80I_LD_mBC_A, &&Z80I_INC_BC,		// 00-03
	&&Z80I_INC_B, &&Z80I_DEC_B, &&Z80I_LD_B_N, &&Z80I_RLCA,			// 04-07
	&&Z80I_EX_AF_AF2, &&Z80I_ADD_HL_BC, &&Z80I_LD_A_mBC, &&Z80I_DEC_BC,	// 08-0B
	&&Z80I_INC_C, &&Z80I_DEC_C, &&Z80I_LD_C_N, &&Z80I_RRCA,			// 0C-0F
	&&Z80I_DJNZ, &&Z80I_LD_DE_NN, &&Z80I_LD_mDE_A, &&Z80I_INC_DE,		// 10-13
	&&Z80I_INC_D, &&Z80I_DEC_D, &&Z80I_LD_D_N, &&Z80I_RLA,			// 14-17
	&&Z80I_JR_N, &&Z80I_ADD_HL_DE, &&Z80I_LD_A_mDE, &&Z80I_DEC_DE,		// 18-1B
	&&Z80I_INC_E, &&Z80I_DEC_E, &&Z80I_LD_E_N, &&Z80I_RRA,			// 1C-1F
	&&Z80I_JRNZ_N, &&Z80I_LD_HL_NN, &&Z80I_LD_mNN_HL, &&Z80I_INC_HL,	// 20-23
	&&Z80I_INC_H, &&Z80I_DEC_H, &&Z80I_LD_H_N, &&Z80I_DAA,			// 24-27
	&&Z80I_JRZ_N, &&Z80I_ADD_HL_HL, &&Z80I_LD_HL_mNN, &&Z80I_DEC_HL,	// 28-2B
	&&Z80I_INC_L, &&Z80I_DEC_L, &&Z80I_LD_L_N, &&Z80I_CPL,			// 2C-2F
	&&Z80I_JRNC_N, &&Z80I_LD_SP_NN, &&Z80I_LD_mNN_A, &&Z80I_INC_SP,		// 30-33
	&&Z80I_INC_mHL, &&Z80I_DEC_mHL, &&Z80I_LD_mHL_N, &&Z80I_SCF,		// 34-37
	&&Z80I_JRC_N, &&Z80I_ADD_HL_SP, &&Z80I_LD_A_mNN, &&Z80I_DEC_SP,		// 38-3B
	&&Z80I_INC_A, &&Z80I_DEC_A, &&Z80I_LD_A_N, &&Z80I_CCF,			// 3C-3F
	&&Z80I_LD_B_B, &&Z80I_LD_B_C, &&Z80I_LD_B_D, &&Z80I_LD_B_E,		// 40-43
	&&Z80I_LD_B_H, &&Z80I_LD_B_L, &&Z80I_LD_B_mHL, &&Z80I_LD_B_A,		// 44-47
	&&Z80I_LD_C_B, &&Z80I_LD_C_C, &&Z80I_LD_C_D, &&Z80I_LD_C_E,		// 48-4B
	&&Z80I_LD_C_H, &&Z80I_LD_C_L, &&Z80I_LD_C_mHL, &&Z80I_LD_C_A,		// 4C-4F
	&&Z80I_LD_D_B, &&Z80I_LD_D_C, &&Z80I_LD_D_D, &&Z80I_LD_D_E,		// 50-53
	&&Z80I_LD_D_H, &&Z80I_LD_D_L, &&Z80I_LD_D_mHL, &&Z80I_LD_D_A,		// 54-57
	&&Z80I_LD_E_B, &&Z80I_LD_E_C, &&Z80I_LD_E_D, &&Z80I_LD_E_E,		// 58-5B
	&&Z80I_LD_E_H, &&Z80I_LD_E_L, &&Z80I_LD_E_mHL, &&Z80I_LD_E_A,		// 5C-5F
	&&Z80I_LD_H_B, &&Z80I_LD_H_C, &&Z80I_LD_H_D, &&Z80I_LD_H_E,		// 60-63
	&&Z80I_LD_H_H, &&Z80I_LD_H_L, &&Z80I_LD_H_mHL, &&Z80I_LD_H_A,		// 64-67
	&&Z80I_LD_L_B, &&Z80I_LD_L_C, &&Z80I_LD_L_D, &&Z80I_LD_L_E,		// 68-6B
	&&Z80I_LD_L_H, &&Z80I_LD_L_L, &&Z80I_LD_L_mHL, &&Z80I_LD_L_A,		// 6C-6F
	&&Z80I_LD_mHL_B, &&Z80I_LD_mHL_C, &&Z80I_LD_mHL_D, &&Z80I_LD_mHL_E,	// 70-73
	&&Z80I_LD_mHL_H, &&Z80I_LD_mHL_L, &&Z80I_HALT, &&Z80I_LD_mHL_A,		// 74-77
	&&Z80I_LD_A_B, &&Z80I_LD_A_C, &&Z80I_LD_A_D, &&Z80I_LD_A_E,		// 78-7B
	&&Z80I_LD_A_H, &&Z80I_LD_A_L, &&Z80I_LD_A_mHL, &&Z80I_LD_A_A,		// 7C-7F
	&&Z80I_ADD_B, &&Z80I_ADD_C, &&Z80I_ADD_D, &&Z80I_ADD_E,			// 80-83
	&&Z80I_ADD_H, &&Z80I_ADD_L, &&Z80I_ADD_mHL, &&Z80I_ADD_A,		// 84-87
	&&Z80I_ADC_B, &&Z80I_ADC_C, &&Z80I_ADC_D, &&Z80I_ADC_E,			// 88-8B
	&&Z80I_ADC_H, &&Z80I_ADC_L, &&Z80I_ADC_mHL, &&Z80I_ADC_A,		// 8C-8F
	&&Z80I_SUB_B, &&Z80I_SUB_C, &&Z80I_SUB_D, &&Z80I_SUB_E,			// 90-83
	&&Z80I_SUB_H, &&Z80I_SUB_L, &&Z80I_SUB_mHL, &&Z80I_SUB_A,		// 94-87
	&&Z80I_SBC_B, &&Z80I_SBC_C, &&Z80I_SBC_D, &&Z80I_SBC_E,			// 98-8B
	&&Z80I_SBC_H, &&Z80I_SBC_L, &&Z80I_SBC_mHL, &&Z80I_SBC_A,		// 9C-8F
	&&Z80I_AND_B, &&Z80I_AND_C, &&Z80I_AND_D, &&Z80I_AND_E,			// A0-A3
	&&Z80I_AND_H, &&Z80I_AND_L, &&Z80I_AND_mHL, &&Z80I_AND_A,		// A4-A7
	&&Z80I_XOR_B, &&Z80I_XOR_C, &&Z80I_XOR_D, &&Z80I_XOR_E,			// A8-AB
	&&Z80I_XOR_H, &&Z80I_XOR_L, &&Z80I_XOR_mHL, &&Z80I_XOR_A,		// AC-AF
	&&Z80I_OR_B, &&Z80I_OR_C, &&Z80I_OR_D, &&Z80I_OR_E,			// B0-B3
	&&Z80I_OR_H, &&Z80I_OR_L, &&Z80I_OR_mHL, &&Z80I_OR_A,			// B4-B7
	&&Z80I_CP_B, &&Z80I_CP_C, &&Z80I_CP_D, &&Z80I_CP_E,			// B8-BB
	&&Z80I_CP_H, &&Z80I_CP_L, &&Z80I_CP_mHL, &&Z80I_CP_A,			// BC-BF
	&&Z80I_RETNZ, &&Z80I_POP_BC, &&Z80I_JPNZ_NN, &&Z80I_JP_NN,		// C0-C3
	&&Z80I_CALLNZ_NN, &&Z80I_PUSH_BC, &&Z80I_ADD_N, &&Z80I_RST,		// C4-C7
	&&Z80I_RETZ, &&Z80I_RET, &&Z80I_JPZ_NN, &&PREFIX_CB,			// C8-CB
	&&Z80I_CALLZ_NN, &&Z80I_CALL_NN, &&Z80I_ADC_N, &&Z80I_RST,		// CC-CF
	&&Z80I_RETNC, &&Z80I_POP_DE, &&Z80I_JPNC_NN, &&Z80I_OUT_mN,		// D0-D3
	&&Z80I_CALLNC_NN, &&Z80I_PUSH_DE, &&Z80I_SUB_N, &&Z80I_RST,		// D4-D7
	&&Z80I_RETC, &&Z80I_EXX, &&Z80I_JPC_NN, &&Z80I_IN_mN,			// D8-DB
	&&Z80I_CALLC_NN, &&PREFIX_DD, &&Z80I_SBC_N, &&Z80I_RST,			// DC-DF
	&&Z80I_RETNP, &&Z80I_POP_HL, &&Z80I_JPNP_NN, &&Z80I_EX_mSP_HL,		// E0-E3
	&&Z80I_CALLNP_NN, &&Z80I_PUSH_HL, &&Z80I_AND_N, &&Z80I_RST,		// E4-E7
	&&Z80I_RETP, &&Z80I_JP_HL, &&Z80I_JPP_NN, &&Z80I_EX_DE_HL,		// E8-EB
	&&Z80I_CALLP_NN, &&PREFIX_ED, &&Z80I_XOR_N, &&Z80I_RST,			// EC-EF
	&&Z80I_RETNS, &&Z80I_POP_AF, &&Z80I_JPNS_NN, &&Z80I_DI,			// F0-F3
	&&Z80I_CALLNS_NN, &&Z80I_PUSH_AF, &&Z80I_OR_N, &&Z80I_RST,		// F4-F7
	&&Z80I_RETS, &&Z80I_LD_SP_HL, &&Z80I_JPS_NN, &&Z80I_EI,			// F8-FB
	&&Z80I_CALLS_NN, &&PREFIX_FD, &&Z80I_CP_N, &&Z80I_RST			// FC-FF
};

// Z80 instruction table: CB prefix
static void *z80_insn_table_CB[] = {
	&&Z80I_RLC_B, &&Z80I_RLC_C, &&Z80I_RLC_D, &&Z80I_RLC_E,			// 00-03
	&&Z80I_RLC_H, &&Z80I_RLC_L, &&Z80I_RLC_mHL, &&Z80I_RLC_A,		// 04-07
	&&Z80I_RRC_B, &&Z80I_RRC_C, &&Z80I_RRC_D, &&Z80I_RRC_E,			// 08-0B
	&&Z80I_RRC_H, &&Z80I_RRC_L, &&Z80I_RRC_mHL, &&Z80I_RRC_A,		// 0C-0F
	&&Z80I_RL_B, &&Z80I_RL_C, &&Z80I_RL_D, &&Z80I_RL_E,			// 10-13
	&&Z80I_RL_H, &&Z80I_RL_L, &&Z80I_RL_mHL, &&Z80I_RL_A,			// 14-17
	&&Z80I_RR_B, &&Z80I_RR_C, &&Z80I_RR_D, &&Z80I_RR_E,			// 18-1B
	&&Z80I_RR_H, &&Z80I_RR_L, &&Z80I_RR_mHL, &&Z80I_RR_A,			// 1C-1F
	&&Z80I_SLA_B, &&Z80I_SLA_C, &&Z80I_SLA_D, &&Z80I_SLA_E,			// 20-23
	&&Z80I_SLA_H, &&Z80I_SLA_L, &&Z80I_SLA_mHL, &&Z80I_SLA_A,		// 24-27
	&&Z80I_SRA_B, &&Z80I_SRA_C, &&Z80I_SRA_D, &&Z80I_SRA_E,			// 28-2B
	&&Z80I_SRA_H, &&Z80I_SRA_L, &&Z80I_SRA_mHL, &&Z80I_SRA_A,		// 2C-2F
	&&Z80I_SLL_B, &&Z80I_SLL_C, &&Z80I_SLL_D, &&Z80I_SLL_E,			// 30-33
	&&Z80I_SLL_H, &&Z80I_SLL_L, &&Z80I_SLL_mHL, &&Z80I_SLL_A,		// 34-37
	&&Z80I_SRL_B, &&Z80I_SRL_C, &&Z80I_SRL_D, &&Z80I_SRL_E,			// 38-3B
	&&Z80I_SRL_H, &&Z80I_SRL_L, &&Z80I_SRL_mHL, &&Z80I_SRL_A,		// 3C-3F
	&&Z80I_BIT0_B, &&Z80I_BIT0_C, &&Z80I_BIT0_D, &&Z80I_BIT0_E,		// 40-43
	&&Z80I_BIT0_H, &&Z80I_BIT0_L, &&Z80I_BIT0_mHL, &&Z80I_BIT0_A,		// 44-47
	&&Z80I_BIT1_B, &&Z80I_BIT1_C, &&Z80I_BIT1_D, &&Z80I_BIT1_E,		// 48-4B
	&&Z80I_BIT1_H, &&Z80I_BIT1_L, &&Z80I_BIT1_mHL, &&Z80I_BIT1_A,		// 4C-4F
	&&Z80I_BIT2_B, &&Z80I_BIT2_C, &&Z80I_BIT2_D, &&Z80I_BIT2_E,		// 50-53
	&&Z80I_BIT2_H, &&Z80I_BIT2_L, &&Z80I_BIT2_mHL, &&Z80I_BIT2_A,		// 54-57
	&&Z80I_BIT3_B, &&Z80I_BIT3_C, &&Z80I_BIT3_D, &&Z80I_BIT3_E,		// 58-5B
	&&Z80I_BIT3_H, &&Z80I_BIT3_L, &&Z80I_BIT3_mHL, &&Z80I_BIT3_A,		// 5C-5F
	&&Z80I_BIT4_B, &&Z80I_BIT4_C, &&Z80I_BIT4_D, &&Z80I_BIT4_E,		// 60-63
	&&Z80I_BIT4_H, &&Z80I_BIT4_L, &&Z80I_BIT4_mHL, &&Z80I_BIT4_A,		// 64-67
	&&Z80I_BIT5_B, &&Z80I_BIT5_C, &&Z80I_BIT5_D, &&Z80I_BIT5_E,		// 68-6B
	&&Z80I_BIT5_H, &&Z80I_BIT5_L, &&Z80I_BIT5_mHL, &&Z80I_BIT5_A,		// 6C-6F
	&&Z80I_BIT6_B, &&Z80I_BIT6_C, &&Z80I_BIT6_D, &&Z80I_BIT6_E,		// 70-73
	&&Z80I_BIT6_H, &&Z80I_BIT6_L, &&Z80I_BIT6_mHL, &&Z80I_BIT6_A,		// 74-77
	&&Z80I_BIT7_B, &&Z80I_BIT7_C, &&Z80I_BIT7_D, &&Z80I_BIT7_E,		// 78-7B
	&&Z80I_BIT7_H, &&Z80I_BIT7_L, &&Z80I_BIT7_mHL, &&Z80I_BIT7_A,		// 7C-7F
	&&Z80I_RES0_B, &&Z80I_RES0_C, &&Z80I_RES0_D, &&Z80I_RES0_E,		// 80-83
	&&Z80I_RES0_H, &&Z80I_RES0_L, &&Z80I_RES0_mHL, &&Z80I_RES0_A,		// 84-87
	&&Z80I_RES1_B, &&Z80I_RES1_C, &&Z80I_RES1_D, &&Z80I_RES1_E,		// 88-8B
	&&Z80I_RES1_H, &&Z80I_RES1_L, &&Z80I_RES1_mHL, &&Z80I_RES1_A,		// 8C-8F
	&&Z80I_RES2_B, &&Z80I_RES2_C, &&Z80I_RES2_D, &&Z80I_RES2_E,		// 90-93
	&&Z80I_RES2_H, &&Z80I_RES2_L, &&Z80I_RES2_mHL, &&Z80I_RES2_A,		// 94-97
	&&Z80I_RES3_B, &&Z80I_RES3_C, &&Z80I_RES3_D, &&Z80I_RES3_E,		// 98-9B
	&&Z80I_RES3_H, &&Z80I_RES3_L, &&Z80I_RES3_mHL, &&Z80I_RES3_A,		// 9C-9F
	&&Z80I_RES4_B, &&Z80I_RES4_C, &&Z80I_RES4_D, &&Z80I_RES4_E,		// A0-A3
	&&Z80I_RES4_H, &&Z80I_RES4_L, &&Z80I_RES4_mHL, &&Z80I_RES4_A,		// A4-A7
	&&Z80I_RES5_B, &&Z80I_RES5_C, &&Z80I_RES5_D, &&Z80I_RES5_E,		// A8-AB
	&&Z80I_RES5_H, &&Z80I_RES5_L, &&Z80I_RES5_mHL, &&Z80I_RES5_A,		// AC-AF
	&&Z80I_RES6_B, &&Z80I_RES6_C, &&Z80I_RES6_D, &&Z80I_RES6_E,		// B0-B3
	&&Z80I_RES6_H, &&Z80I_RES6_L, &&Z80I_RES6_mHL, &&Z80I_RES6_A,		// B4-B7
	&&Z80I_RES7_B, &&Z80I_RES7_C, &&Z80I_RES7_D, &&Z80I_RES7_E,		// B8-BB
	&&Z80I_RES7_H, &&Z80I_RES7_L, &&Z80I_RES7_mHL, &&Z80I_RES7_A,		// BC-BF
	&&Z80I_SET0_B, &&Z80I_SET0_C, &&Z80I_SET0_D, &&Z80I_SET0_E,		// C0-C3
	&&Z80I_SET0_H, &&Z80I_SET0_L, &&Z80I_SET0_mHL, &&Z80I_SET0_A,		// C4-C7
	&&Z80I_SET1_B, &&Z80I_SET1_C, &&Z80I_SET1_D, &&Z80I_SET1_E,		// C8-CB
	&&Z80I_SET1_H, &&Z80I_SET1_L, &&Z80I_SET1_mHL, &&Z80I_SET1_A,		// CC-CF
	&&Z80I_SET2_B, &&Z80I_SET2_C, &&Z80I_SET2_D, &&Z80I_SET2_E,		// D0-D3
	&&Z80I_SET2_H, &&Z80I_SET2_L, &&Z80I_SET2_mHL, &&Z80I_SET2_A,		// D4-D7
	&&Z80I_SET3_B, &&Z80I_SET3_C, &&Z80I_SET3_D, &&Z80I_SET3_E,		// D8-DB
	&&Z80I_SET3_H, &&Z80I_SET3_L, &&Z80I_SET3_mHL, &&Z80I_SET3_A,		// DC-DF
	&&Z80I_SET4_B, &&Z80I_SET4_C, &&Z80I_SET4_D, &&Z80I_SET4_E,		// E0-E3
	&&Z80I_SET4_H, &&Z80I_SET4_L, &&Z80I_SET4_mHL, &&Z80I_SET4_A,		// E4-E7
	&&Z80I_SET5_B, &&Z80I_SET5_C, &&Z80I_SET5_D, &&Z80I_SET5_E,		// E8-EB
	&&Z80I_SET5_H, &&Z80I_SET5_L, &&Z80I_SET5_mHL, &&Z80I_SET5_A,		// EC-EF
	&&Z80I_SET6_B, &&Z80I_SET6_C, &&Z80I_SET6_D, &&Z80I_SET6_E,		// F0-F3
	&&Z80I_SET6_H, &&Z80I_SET6_L, &&Z80I_SET6_mHL, &&Z80I_SET6_A,		// F4-F7
	&&Z80I_SET7_B, &&Z80I_SET7_C, &&Z80I_SET7_D, &&Z80I_SET7_E,		// F8-FB
	&&Z80I_SET7_H, &&Z80I_SET7_L, &&Z80I_SET7_mHL, &&Z80I_SET7_A		// FC-FF
};

// Z80 instruction table: DD prefix
static void *z80_insn_table_DD[] = {
	&&Z80I_NOP, &&Z80I_LD_BC_NN, &&Z80I_LD_mBC_A, &&Z80I_INC_BC,		// 00-03
	&&Z80I_INC_B, &&Z80I_DEC_B, &&Z80I_LD_B_N, &&Z80I_RLCA,			// 04-07
	&&Z80I_EX_AF_AF2, &&Z80I_ADD_IX_BC, &&Z80I_LD_A_mBC, &&Z80I_DEC_BC,	// 08-0B
	&&Z80I_INC_C, &&Z80I_DEC_C, &&Z80I_LD_C_N, &&Z80I_RRCA,			// 0C-0F
	&&Z80I_DJNZ, &&Z80I_LD_DE_NN, &&Z80I_LD_mDE_A, &&Z80I_INC_DE,		// 10-13
	&&Z80I_INC_D, &&Z80I_DEC_D, &&Z80I_LD_D_N, &&Z80I_RLA,			// 14-17
	&&Z80I_JR_N, &&Z80I_ADD_IX_DE, &&Z80I_LD_A_mDE, &&Z80I_DEC_DE,		// 18-1B
	&&Z80I_DEC_E, &&Z80I_INC_E, &&Z80I_LD_E_N, &&Z80I_RRA,			// 1C-1F
	&&Z80I_JRNZ_N, &&Z80I_LD_IX_NN, &&Z80I_LD_mNN_IX, &&Z80I_INC_IX,	// 20-23
	&&Z80I_INC_IXh, &&Z80I_DEC_IXh, &&Z80I_LD_IXh_N, &&Z80I_DAA,		// 24-27
	&&Z80I_JRZ_N, &&Z80I_ADD_IX_IX, &&Z80I_LD_IX_mNN, &&Z80I_DEC_IX,	// 28-2B
	&&Z80I_INC_IXl, &&Z80I_DEC_IXl, &&Z80I_LD_IXl_N, &&Z80I_CPL,		// 2C-2F
	&&Z80I_JRNC_N, &&Z80I_LD_SP_NN, &&Z80I_LD_mNN_A, &&Z80I_INC_SP,		// 30-33
	&&Z80I_INC_mIXd, &&Z80I_DEC_mIXd, &&Z80I_LD_mIXd_N, &&Z80I_SCF,		// 34-37
	&&Z80I_JRC_N, &&Z80I_ADD_IX_SP, &&Z80I_LD_A_mNN, &&Z80I_DEC_SP,		// 38-3B
	&&Z80I_INC_A, &&Z80I_DEC_A, &&Z80I_LD_A_N, &&Z80I_CCF,			// 3C-3F
	&&Z80I_LD_B_B, &&Z80I_LD_B_C, &&Z80I_LD_B_D, &&Z80I_LD_B_E,		// 40-43
	&&Z80I_LD_B_IXh, &&Z80I_LD_B_IXl, &&Z80I_LD_B_mIXd, &&Z80I_LD_B_A,	// 44-47
	&&Z80I_LD_C_B, &&Z80I_LD_C_C, &&Z80I_LD_C_D, &&Z80I_LD_C_E,		// 48-4B
	&&Z80I_LD_C_IXh, &&Z80I_LD_C_IXl, &&Z80I_LD_C_mIXd, &&Z80I_LD_C_A,	// 4C-4F
	&&Z80I_LD_D_B, &&Z80I_LD_D_C, &&Z80I_LD_D_D, &&Z80I_LD_D_E,		// 50-53
	&&Z80I_LD_D_IXh, &&Z80I_LD_D_IXl, &&Z80I_LD_D_mIXd, &&Z80I_LD_D_A,	// 54-57
	&&Z80I_LD_E_B, &&Z80I_LD_E_C, &&Z80I_LD_E_D, &&Z80I_LD_E_E,		// 58-5B
	&&Z80I_LD_E_IXh, &&Z80I_LD_E_IXl, &&Z80I_LD_E_mIXd, &&Z80I_LD_E_A,	// 5C-5F
	&&Z80I_LD_IXh_B, &&Z80I_LD_IXh_C, &&Z80I_LD_IXh_D, &&Z80I_LD_IXh_E,	// 60-63
	&&Z80I_LD_IXh_IXh, &&Z80I_LD_IXh_IXl, &&Z80I_LD_H_mIXd, &&Z80I_LD_IXh_A,// 64-67
	&&Z80I_LD_IXl_B, &&Z80I_LD_IXl_C, &&Z80I_LD_IXl_D, &&Z80I_LD_IXl_E,	// 68-6B
	&&Z80I_LD_IXl_IXh, &&Z80I_LD_IXl_IXl, &&Z80I_LD_L_mIXd, &&Z80I_LD_IXl_A,// 6C-6F
	&&Z80I_LD_mIXd_B, &&Z80I_LD_mIXd_C, &&Z80I_LD_mIXd_D, &&Z80I_LD_mIXd_E,	// 70-73
	&&Z80I_LD_mIXd_H, &&Z80I_LD_mIXd_L, &&Z80I_HALT, &&Z80I_LD_mIXd_A,	// 74-77
	&&Z80I_LD_A_B, &&Z80I_LD_A_C, &&Z80I_LD_A_D, &&Z80I_LD_A_E,		// 78-7B
	&&Z80I_LD_A_IXh, &&Z80I_LD_A_IXl, &&Z80I_LD_A_mIXd, &&Z80I_LD_A_A,	// 7C-7F
	&&Z80I_ADD_B, &&Z80I_ADD_C, &&Z80I_ADD_D, &&Z80I_ADD_E,			// 80-83
	&&Z80I_ADD_IXh, &&Z80I_ADD_IXl, &&Z80I_ADD_mIXd, &&Z80I_ADD_A,		// 84-87
	&&Z80I_ADC_B, &&Z80I_ADC_C, &&Z80I_ADC_D, &&Z80I_ADC_E,			// 88-8B
	&&Z80I_ADC_IXh, &&Z80I_ADC_IXl, &&Z80I_ADC_mIXd, &&Z80I_ADC_A,		// 8C-8F
	&&Z80I_SUB_B, &&Z80I_SUB_C, &&Z80I_SUB_D, &&Z80I_SUB_E,			// 90-83
	&&Z80I_SUB_IXh, &&Z80I_SUB_IXl, &&Z80I_SUB_mIXd, &&Z80I_SUB_A,		// 94-87
	&&Z80I_SBC_B, &&Z80I_SBC_C, &&Z80I_SBC_D, &&Z80I_SBC_E,			// 98-8B
	&&Z80I_SBC_IXh, &&Z80I_SBC_IXl, &&Z80I_SBC_mIXd, &&Z80I_SBC_A,		// 9C-8F
	&&Z80I_AND_B, &&Z80I_AND_C, &&Z80I_AND_D, &&Z80I_AND_E,			// A0-A3
	&&Z80I_AND_IXh, &&Z80I_AND_IXl, &&Z80I_AND_mIXd, &&Z80I_AND_A,		// A4-A7
	&&Z80I_XOR_B, &&Z80I_XOR_C, &&Z80I_XOR_D, &&Z80I_XOR_E,			// A8-AB
	&&Z80I_XOR_IXh, &&Z80I_XOR_IXl, &&Z80I_XOR_mIXd, &&Z80I_XOR_A,		// AC-AF
	&&Z80I_OR_B, &&Z80I_OR_C, &&Z80I_OR_D, &&Z80I_OR_E,			// B0-B3
	&&Z80I_OR_IXh, &&Z80I_OR_IXl, &&Z80I_OR_mIXd, &&Z80I_OR_A,		// B4-B7
	&&Z80I_CP_B, &&Z80I_CP_C, &&Z80I_CP_D, &&Z80I_CP_E,			// B8-BB
	&&Z80I_CP_IXh, &&Z80I_CP_IXl, &&Z80I_CP_mIXd, &&Z80I_CP_A,		// BC-BF
	&&Z80I_RETNZ, &&Z80I_POP_BC, &&Z80I_JPNZ_NN, &&Z80I_JP_NN,		// C0-C3
	&&Z80I_CALLNZ_NN, &&Z80I_PUSH_BC, &&Z80I_ADD_N, &&Z80I_RST,		// C4-C7
	&&Z80I_RETZ, &&Z80I_RET, &&Z80I_JPZ_NN, &&PREFIX_DDCB,			// C8-CB
	&&Z80I_CALLZ_NN, &&Z80I_CALL_NN, &&Z80I_ADC_N, &&Z80I_RST,		// CC-CF
	&&Z80I_RETNC, &&Z80I_POP_DE, &&Z80I_JPNC_NN, &&Z80I_OUT_mN,		// D0-D3
	&&Z80I_CALLNC_NN, &&Z80I_PUSH_DE, &&Z80I_SUB_N, &&Z80I_RST,		// D4-D7
	&&Z80I_RETC, &&Z80I_EXX, &&Z80I_JPC_NN, &&Z80I_IN_mN,			// D8-DB
	&&Z80I_CALLC_NN, &&PREFIX_DD, &&Z80I_SBC_N, &&Z80I_RST,			// DC-DF
	&&Z80I_RETNP, &&Z80I_POP_IX, &&Z80I_JPNP_NN, &&Z80I_EX_mSP_IX,		// E0-E3
	&&Z80I_CALLNP_NN, &&Z80I_PUSH_IX, &&Z80I_AND_N, &&Z80I_RST,		// E4-E7
	&&Z80I_RETP, &&Z80I_JP_IX, &&Z80I_JPP_NN, &&Z80I_EX_DE_HL,		// E8-EB
	&&Z80I_CALLP_NN, &&PREFIX_ED, &&Z80I_XOR_N, &&Z80I_RST,			// EC-EF
	&&Z80I_RETNS, &&Z80I_POP_AF, &&Z80I_JPNS_NN, &&Z80I_DI,			// F0-F3
	&&Z80I_CALLNS_NN, &&Z80I_PUSH_AF, &&Z80I_OR_N, &&Z80I_RST,		// F4-F7
	&&Z80I_RETS, &&Z80I_LD_SP_IX, &&Z80I_JPS_NN, &&Z80I_EI,			// F8-FB
	&&Z80I_CALLNS_NN, &&PREFIX_FD, &&Z80I_CP_N, &&Z80I_RST			// FC-FF
};

// Z80 instruction table: ED prefix
static void *z80_insn_table_ED[] = {
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 00-03
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 04-07
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 08-0B
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 0C-0F
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 10-13
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 14-17
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 18-1B
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 1C-1F
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 20-23
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 24-27
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 28-2B
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 2C-2F
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 30-33
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 34-37
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 38-3B
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 3C-3F
	&&Z80I_IN_B_mBC, &&Z80I_OUT_mBC_B, &&Z80I_SBC_HL_BC, &&Z80I_LD_mNN_BC,	// 40-43
	&&Z80I_NEG, &&Z80I_RETN, &&Z80I_IM0, &&Z80I_LD_I_A,			// 44-47
	&&Z80I_IN_C_mBC, &&Z80I_OUT_mBC_C, &&Z80I_ADC_HL_BC, &&Z80I_LD_BC_mNN,	// 48-4B
	&&Z80I_NEG, &&Z80I_RETI, &&Z80I_IM0, &&Z80I_LD_R_A,			// 4C-4F
	&&Z80I_IN_D_mBC, &&Z80I_OUT_mBC_D, &&Z80I_SBC_HL_DE, &&Z80I_LD_mNN_DE,	// 50-53
	&&Z80I_NEG, &&Z80I_RETN, &&Z80I_IM1, &&Z80I_LD_A_I,			// 54-57
	&&Z80I_IN_E_mBC, &&Z80I_OUT_mBC_E, &&Z80I_ADC_HL_DE, &&Z80I_LD_DE_mNN,	// 58-5B
	&&Z80I_NEG, &&Z80I_RETN, &&Z80I_IM2, &&Z80I_LD_A_R,			// 5C-5F
	&&Z80I_IN_H_mBC, &&Z80I_OUT_mBC_H, &&Z80I_SBC_HL_HL, &&Z80I_LD2_mNN_HL,	// 60-63
	&&Z80I_NEG, &&Z80I_RETN, &&Z80I_IM0, &&Z80I_RRD,			// 64-67
	&&Z80I_IN_L_mBC, &&Z80I_OUT_mBC_L, &&Z80I_ADC_HL_HL, &&Z80I_LD2_HL_mNN,	// 68-6B
	&&Z80I_NEG, &&Z80I_RETN, &&Z80I_IM0, &&Z80I_RLD,			// 6C-6F
	&&Z80I_IN_F_mBC, &&Z80I_OUT_mBC_0, &&Z80I_SBC_HL_SP, &&Z80I_LD_mNN_SP,	// 70-73
	&&Z80I_NEG, &&Z80I_RETN, &&Z80I_IM1, &&Z80I_NOP,			// 74-77
	&&Z80I_IN_A_mBC, &&Z80I_OUT_mBC_A, &&Z80I_ADC_HL_SP, &&Z80I_LD_SP_mNN,	// 78-7B
	&&Z80I_NEG, &&Z80I_RETN, &&Z80I_IM2, &&Z80I_NOP,			// 7C-7F
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 80-83
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 84-87
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 88-8B
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 8C-8F
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 90-93
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 94-97
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 98-9B
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// 9C-9F
	&&Z80I_LDI, &&Z80I_CPI, &&Z80I_INI, &&Z80I_OUTI,			// A0-A3
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// A4-A7
	&&Z80I_LDD, &&Z80I_CPD, &&Z80I_IND, &&Z80I_OUTD,			// A8-AB
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// AC-AF
	&&Z80I_LDIR, &&Z80I_CPIR, &&Z80I_INIR, &&Z80I_OTIR,			// B0-B3
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// B4-B7
	&&Z80I_LDDR, &&Z80I_CPDR, &&Z80I_INDR, &&Z80I_OTDR,			// B8-BB
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// BC-BF
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// C0-C3
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// C4-C7
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// C8-CB
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// CC-CF
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// D0-D3
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// D4-D7
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// D8-DB
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// DC-DF
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// E0-E3
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// E4-E7
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// E8-EB
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// EC-EF
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// F0-F3
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// F4-F7
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP,				// F8-FB
	&&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP, &&Z80I_NOP				// FC-FF
};

// Z80 instruction table: FD prefix
static void *z80_insn_table_FD[] = {
	&&Z80I_NOP, &&Z80I_LD_BC_NN, &&Z80I_LD_mBC_A, &&Z80I_INC_BC,		// 00-03
	&&Z80I_INC_B, &&Z80I_DEC_B, &&Z80I_LD_B_N, &&Z80I_RLCA,			// 04-07
	&&Z80I_EX_AF_AF2, &&Z80I_ADD_IY_BC, &&Z80I_LD_A_mBC, &&Z80I_DEC_BC,	// 08-0B
	&&Z80I_INC_C, &&Z80I_DEC_C, &&Z80I_LD_C_N, &&Z80I_RRCA,			// 0C-0F
	&&Z80I_DJNZ, &&Z80I_LD_DE_NN, &&Z80I_LD_mDE_A, &&Z80I_INC_DE,		// 10-13
	&&Z80I_INC_D, &&Z80I_DEC_D, &&Z80I_LD_D_N, &&Z80I_RLA,			// 14-17
	&&Z80I_JR_N, &&Z80I_ADD_IY_DE, &&Z80I_LD_A_mDE, &&Z80I_DEC_DE,		// 18-1B
	&&Z80I_DEC_E, &&Z80I_INC_E, &&Z80I_LD_E_N, &&Z80I_RRA,			// 1C-1F
	&&Z80I_JRNZ_N, &&Z80I_LD_IY_NN, &&Z80I_LD_mNN_IY, &&Z80I_INC_IY,	// 20-23
	&&Z80I_INC_IYh, &&Z80I_DEC_IYh, &&Z80I_LD_IYh_N, &&Z80I_DAA,		// 24-27
	&&Z80I_JRZ_N, &&Z80I_ADD_IY_IY, &&Z80I_LD_IY_mNN, &&Z80I_DEC_IY,	// 28-2B
	&&Z80I_INC_IYl, &&Z80I_DEC_IYl, &&Z80I_LD_IYl_N, &&Z80I_CPL,		// 2C-2F
	&&Z80I_JRNC_N, &&Z80I_LD_SP_NN, &&Z80I_LD_mNN_A, &&Z80I_INC_SP,		// 30-33
	&&Z80I_INC_mIYd, &&Z80I_DEC_mIYd, &&Z80I_LD_mIYd_N, &&Z80I_SCF,		// 34-37
	&&Z80I_JRC_N, &&Z80I_ADD_IY_SP, &&Z80I_LD_A_mNN, &&Z80I_DEC_SP,		// 38-3B
	&&Z80I_INC_A, &&Z80I_DEC_A, &&Z80I_LD_A_N, &&Z80I_CCF,			// 3C-3F
	&&Z80I_LD_B_B, &&Z80I_LD_B_C, &&Z80I_LD_B_D, &&Z80I_LD_B_E,		// 40-43
	&&Z80I_LD_B_IYh, &&Z80I_LD_B_IYl, &&Z80I_LD_B_mIYd, &&Z80I_LD_B_A,	// 44-47
	&&Z80I_LD_C_B, &&Z80I_LD_C_C, &&Z80I_LD_C_D, &&Z80I_LD_C_E,		// 48-4B
	&&Z80I_LD_C_IYh, &&Z80I_LD_C_IYl, &&Z80I_LD_C_mIYd, &&Z80I_LD_C_A,	// 4C-4F
	&&Z80I_LD_D_B, &&Z80I_LD_D_C, &&Z80I_LD_D_D, &&Z80I_LD_D_E,		// 50-53
	&&Z80I_LD_D_IYh, &&Z80I_LD_D_IYl, &&Z80I_LD_D_mIYd, &&Z80I_LD_D_A,	// 54-57
	&&Z80I_LD_E_B, &&Z80I_LD_E_C, &&Z80I_LD_E_D, &&Z80I_LD_E_E,		// 58-5B
	&&Z80I_LD_E_IYh, &&Z80I_LD_E_IYl, &&Z80I_LD_E_mIYd, &&Z80I_LD_E_A,	// 5C-5F
	&&Z80I_LD_IYh_B, &&Z80I_LD_IYh_C, &&Z80I_LD_IYh_D, &&Z80I_LD_IYh_E,	// 60-63
	&&Z80I_LD_IYh_IYh, &&Z80I_LD_IYh_IYl, &&Z80I_LD_H_mIYd, &&Z80I_LD_IYh_A,// 64-67
	&&Z80I_LD_IYl_B, &&Z80I_LD_IYl_C, &&Z80I_LD_IYl_D, &&Z80I_LD_IYl_E,	// 68-6B
	&&Z80I_LD_IYl_IYh, &&Z80I_LD_IYl_IYl, &&Z80I_LD_L_mIYd, &&Z80I_LD_IYl_A,// 6C-6F
	&&Z80I_LD_mIYd_B, &&Z80I_LD_mIYd_C, &&Z80I_LD_mIYd_D, &&Z80I_LD_mIYd_E,	// 70-73
	&&Z80I_LD_mIYd_H, &&Z80I_LD_mIYd_L, &&Z80I_HALT, &&Z80I_LD_mIYd_A,	// 74-77
	&&Z80I_LD_A_B, &&Z80I_LD_A_C, &&Z80I_LD_A_D, &&Z80I_LD_A_E,		// 78-7B
	&&Z80I_LD_A_IYh, &&Z80I_LD_A_IYl, &&Z80I_LD_A_mIYd, &&Z80I_LD_A_A,	// 7C-7F
	&&Z80I_ADD_B, &&Z80I_ADD_C, &&Z80I_ADD_D, &&Z80I_ADD_E,			// 80-83
	&&Z80I_ADD_IYh, &&Z80I_ADD_IYl, &&Z80I_ADD_mIYd, &&Z80I_ADD_A,		// 84-87
	&&Z80I_ADC_B, &&Z80I_ADC_C, &&Z80I_ADC_D, &&Z80I_ADC_E,			// 88-8B
	&&Z80I_ADC_IYh, &&Z80I_ADC_IYl, &&Z80I_ADC_mIYd, &&Z80I_ADC_A,		// 8C-8F
	&&Z80I_SUB_B, &&Z80I_SUB_C, &&Z80I_SUB_D, &&Z80I_SUB_E,			// 90-83
	&&Z80I_SUB_IYh, &&Z80I_SUB_IYl, &&Z80I_SUB_mIYd, &&Z80I_SUB_A,		// 94-87
	&&Z80I_SBC_B, &&Z80I_SBC_C, &&Z80I_SBC_D, &&Z80I_SBC_E,			// 98-8B
	&&Z80I_SBC_IYh, &&Z80I_SBC_IYl, &&Z80I_SBC_mIYd, &&Z80I_SBC_A,		// 9C-8F
	&&Z80I_AND_B, &&Z80I_AND_C, &&Z80I_AND_D, &&Z80I_AND_E,			// A0-A3
	&&Z80I_AND_IYh, &&Z80I_AND_IYl, &&Z80I_AND_mIYd, &&Z80I_AND_A,		// A4-A7
	&&Z80I_XOR_B, &&Z80I_XOR_C, &&Z80I_XOR_D, &&Z80I_XOR_E,			// A8-AB
	&&Z80I_XOR_IYh, &&Z80I_XOR_IYl, &&Z80I_XOR_mIYd, &&Z80I_XOR_A,		// AC-AF
	&&Z80I_OR_B, &&Z80I_OR_C, &&Z80I_OR_D, &&Z80I_OR_E,			// B0-B3
	&&Z80I_OR_IYh, &&Z80I_OR_IYl, &&Z80I_OR_mIYd, &&Z80I_OR_A,		// B4-B7
	&&Z80I_CP_B, &&Z80I_CP_C, &&Z80I_CP_D, &&Z80I_CP_E,			// B8-BB
	&&Z80I_CP_IYh, &&Z80I_CP_IYl, &&Z80I_CP_mIYd, &&Z80I_CP_A,		// BC-BF
	&&Z80I_RETNZ, &&Z80I_POP_BC, &&Z80I_JPNZ_NN, &&Z80I_JP_NN,		// C0-C3
	&&Z80I_CALLNZ_NN, &&Z80I_PUSH_BC, &&Z80I_ADD_N, &&Z80I_RST,		// C4-C7
	&&Z80I_RETZ, &&Z80I_RET, &&Z80I_JPZ_NN, &&PREFIX_FDCB,			// C8-CB
	&&Z80I_CALLZ_NN, &&Z80I_CALL_NN, &&Z80I_ADC_N, &&Z80I_RST,		// CC-CF
	&&Z80I_RETNC, &&Z80I_POP_DE, &&Z80I_JPNC_NN, &&Z80I_OUT_mN,		// D0-D3
	&&Z80I_CALLNC_NN, &&Z80I_PUSH_DE, &&Z80I_SUB_N, &&Z80I_RST,		// D4-D7
	&&Z80I_RETC, &&Z80I_EXX, &&Z80I_JPC_NN, &&Z80I_IN_mN,			// D8-DB
	&&Z80I_CALLC_NN, &&PREFIX_DD, &&Z80I_SBC_N, &&Z80I_RST,			// DC-DF
	&&Z80I_RETNP, &&Z80I_POP_IY, &&Z80I_JPNP_NN, &&Z80I_EX_mSP_IY,		// E0-E3
	&&Z80I_CALLNP_NN, &&Z80I_PUSH_IY, &&Z80I_AND_N, &&Z80I_RST,		// E4-E7
	&&Z80I_RETP, &&Z80I_JP_IY, &&Z80I_JPP_NN, &&Z80I_EX_DE_HL,		// E8-EB
	&&Z80I_CALLP_NN, &&PREFIX_ED, &&Z80I_XOR_N, &&Z80I_RST,			// EC-EF
	&&Z80I_RETNS, &&Z80I_POP_AF, &&Z80I_JPNS_NN, &&Z80I_DI,			// F0-F3
	&&Z80I_CALLNS_NN, &&Z80I_PUSH_AF, &&Z80I_OR_N, &&Z80I_RST,		// F4-F7
	&&Z80I_RETS, &&Z80I_LD_SP_IY, &&Z80I_JPS_NN, &&Z80I_EI,			// F8-FB
	&&Z80I_CALLNS_NN, &&PREFIX_FD, &&Z80I_CP_N, &&Z80I_RST			// FC-FF
};

// Z80 instruction table: DDCB prefix
static void *z80_insn_table_DDCB[] = {
	&&Z80I_RLC_mIXd_B, &&Z80I_RLC_mIXd_C, &&Z80I_RLC_mIXd_D, &&Z80I_RLC_mIXd_E,	// 00-03
	&&Z80I_RLC_mIXd_H, &&Z80I_RLC_mIXd_L, &&Z80I_RLC_mIXd, &&Z80I_RLC_mIXd_A,	// 04-07
	&&Z80I_RRC_mIXd_B, &&Z80I_RRC_mIXd_C, &&Z80I_RRC_mIXd_D, &&Z80I_RRC_mIXd_E,	// 08-0B
	&&Z80I_RRC_mIXd_H, &&Z80I_RRC_mIXd_L, &&Z80I_RRC_mIXd, &&Z80I_RRC_mIXd_A,	// 0C-0F
	&&Z80I_RL_mIXd_B, &&Z80I_RL_mIXd_C, &&Z80I_RL_mIXd_D, &&Z80I_RL_mIXd_E,		// 10-13
	&&Z80I_RL_mIXd_H, &&Z80I_RL_mIXd_L, &&Z80I_RL_mIXd, &&Z80I_RL_mIXd_A,		// 14-17
	&&Z80I_RR_mIXd_B, &&Z80I_RR_mIXd_C, &&Z80I_RR_mIXd_D, &&Z80I_RR_mIXd_E,		// 18-1B
	&&Z80I_RR_mIXd_H, &&Z80I_RR_mIXd_L, &&Z80I_RR_mIXd, &&Z80I_RR_mIXd_A,		// 1C-1F
	&&Z80I_SLA_mIXd_B, &&Z80I_SLA_mIXd_C, &&Z80I_SLA_mIXd_D, &&Z80I_SLA_mIXd_E,	// 20-23
	&&Z80I_SLA_mIXd_H, &&Z80I_SLA_mIXd_L, &&Z80I_SLA_mIXd, &&Z80I_SLA_mIXd_A,	// 24-27
	&&Z80I_SRA_mIXd_B, &&Z80I_SRA_mIXd_C, &&Z80I_SRA_mIXd_D, &&Z80I_SRA_mIXd_E,	// 28-2B
	&&Z80I_SRA_mIXd_H, &&Z80I_SRA_mIXd_L, &&Z80I_SRA_mIXd, &&Z80I_SRA_mIXd_A,	// 2C-2F
	&&Z80I_SLL_mIXd_B, &&Z80I_SLL_mIXd_C, &&Z80I_SLL_mIXd_D, &&Z80I_SLL_mIXd_E,	// 30-33
	&&Z80I_SLL_mIXd_H, &&Z80I_SLL_mIXd_L, &&Z80I_SLL_mIXd, &&Z80I_SLL_mIXd_A,	// 34-37
	&&Z80I_SRL_mIXd_B, &&Z80I_SRL_mIXd_C, &&Z80I_SRL_mIXd_D, &&Z80I_SRL_mIXd_E,	// 38-3B
	&&Z80I_SRL_mIXd_H, &&Z80I_SRL_mIXd_L, &&Z80I_SRL_mIXd, &&Z80I_SRL_mIXd_A,	// 3C-3F
	&&Z80I_BIT0_B, &&Z80I_BIT0_C, &&Z80I_BIT0_D, &&Z80I_BIT0_E,			// 40-43
	&&Z80I_BIT0_H, &&Z80I_BIT0_L, &&Z80I_BIT0_mIXd, &&Z80I_BIT0_A,			// 44-47
	&&Z80I_BIT1_B, &&Z80I_BIT1_C, &&Z80I_BIT1_D, &&Z80I_BIT1_E,			// 48-4B
	&&Z80I_BIT1_H, &&Z80I_BIT1_L, &&Z80I_BIT1_mIXd, &&Z80I_BIT1_A,			// 4C-4F
	&&Z80I_BIT2_B, &&Z80I_BIT2_C, &&Z80I_BIT2_D, &&Z80I_BIT2_E,			// 50-53
	&&Z80I_BIT2_H, &&Z80I_BIT2_L, &&Z80I_BIT2_mIXd, &&Z80I_BIT2_A,			// 54-57
	&&Z80I_BIT3_B, &&Z80I_BIT3_C, &&Z80I_BIT3_D, &&Z80I_BIT3_E,			// 58-5B
	&&Z80I_BIT3_H, &&Z80I_BIT3_L, &&Z80I_BIT3_mIXd, &&Z80I_BIT3_A,			// 5C-5F
	&&Z80I_BIT4_B, &&Z80I_BIT4_C, &&Z80I_BIT4_D, &&Z80I_BIT4_E,			// 60-63
	&&Z80I_BIT4_H, &&Z80I_BIT4_L, &&Z80I_BIT4_mIXd, &&Z80I_BIT4_A,			// 64-67
	&&Z80I_BIT5_B, &&Z80I_BIT5_C, &&Z80I_BIT5_D, &&Z80I_BIT5_E,			// 68-6B
	&&Z80I_BIT5_H, &&Z80I_BIT5_L, &&Z80I_BIT5_mIXd, &&Z80I_BIT5_A,			// 6C-6F
	&&Z80I_BIT6_B, &&Z80I_BIT6_C, &&Z80I_BIT6_D, &&Z80I_BIT6_E,			// 70-73
	&&Z80I_BIT6_H, &&Z80I_BIT6_L, &&Z80I_BIT6_mIXd, &&Z80I_BIT6_A,			// 74-77
	&&Z80I_BIT7_B, &&Z80I_BIT7_C, &&Z80I_BIT7_D, &&Z80I_BIT7_E,			// 78-7B
	&&Z80I_BIT7_H, &&Z80I_BIT7_L, &&Z80I_BIT7_mIXd, &&Z80I_BIT7_A,			// 7C-7F
	&&Z80I_RES0_mIXd_B, &&Z80I_RES0_mIXd_C, &&Z80I_RES0_mIXd_D, &&Z80I_RES0_mIXd_E,	// 80-83
	&&Z80I_RES0_mIXd_H, &&Z80I_RES0_mIXd_L, &&Z80I_RES0_mIXd, &&Z80I_RES0_mIXd_A,	// 84-87
	&&Z80I_RES1_mIXd_B, &&Z80I_RES1_mIXd_C, &&Z80I_RES1_mIXd_D, &&Z80I_RES1_mIXd_E,	// 88-8B
	&&Z80I_RES1_mIXd_H, &&Z80I_RES1_mIXd_L, &&Z80I_RES1_mIXd, &&Z80I_RES1_mIXd_A,	// 8C-8F
	&&Z80I_RES2_mIXd_B, &&Z80I_RES2_mIXd_C, &&Z80I_RES2_mIXd_D, &&Z80I_RES2_mIXd_E,	// 90-93
	&&Z80I_RES2_mIXd_H, &&Z80I_RES2_mIXd_L, &&Z80I_RES2_mIXd, &&Z80I_RES2_mIXd_A,	// 94-97
	&&Z80I_RES3_mIXd_B, &&Z80I_RES3_mIXd_C, &&Z80I_RES3_mIXd_D, &&Z80I_RES3_mIXd_E,	// 98-9B
	&&Z80I_RES3_mIXd_H, &&Z80I_RES3_mIXd_L, &&Z80I_RES3_mIXd, &&Z80I_RES3_mIXd_A,	// 9C-9F
	&&Z80I_RES4_mIXd_B, &&Z80I_RES4_mIXd_C, &&Z80I_RES4_mIXd_D, &&Z80I_RES4_mIXd_E,	// A0-A3
	&&Z80I_RES4_mIXd_H, &&Z80I_RES4_mIXd_L, &&Z80I_RES4_mIXd, &&Z80I_RES4_mIXd_A,	// A4-A7
	&&Z80I_RES5_mIXd_B, &&Z80I_RES5_mIXd_C, &&Z80I_RES5_mIXd_D, &&Z80I_RES5_mIXd_E,	// A8-AB
	&&Z80I_RES5_mIXd_H, &&Z80I_RES5_mIXd_L, &&Z80I_RES5_mIXd, &&Z80I_RES5_mIXd_A,	// AC-AF
	&&Z80I_RES6_mIXd_B, &&Z80I_RES6_mIXd_C, &&Z80I_RES6_mIXd_D, &&Z80I_RES6_mIXd_E,	// B0-B3
	&&Z80I_RES6_mIXd_H, &&Z80I_RES6_mIXd_L, &&Z80I_RES6_mIXd, &&Z80I_RES6_mIXd_A,	// B4-B7
	&&Z80I_RES7_mIXd_B, &&Z80I_RES7_mIXd_C, &&Z80I_RES7_mIXd_D, &&Z80I_RES7_mIXd_E,	// B8-BB
	&&Z80I_RES7_mIXd_H, &&Z80I_RES7_mIXd_L, &&Z80I_RES7_mIXd, &&Z80I_RES7_mIXd_A,	// BC-BF
	&&Z80I_SET0_mIXd_B, &&Z80I_SET0_mIXd_C, &&Z80I_SET0_mIXd_D, &&Z80I_SET0_mIXd_E,	// C0-C3
	&&Z80I_SET0_mIXd_H, &&Z80I_SET0_mIXd_L, &&Z80I_SET0_mIXd, &&Z80I_SET0_mIXd_A,	// C4-C7
	&&Z80I_SET1_mIXd_B, &&Z80I_SET1_mIXd_C, &&Z80I_SET1_mIXd_D, &&Z80I_SET1_mIXd_E,	// C8-CB
	&&Z80I_SET1_mIXd_H, &&Z80I_SET1_mIXd_L, &&Z80I_SET1_mIXd, &&Z80I_SET1_mIXd_A,	// CC-CF
	&&Z80I_SET2_mIXd_B, &&Z80I_SET2_mIXd_C, &&Z80I_SET2_mIXd_D, &&Z80I_SET2_mIXd_E,	// D0-D3
	&&Z80I_SET2_mIXd_H, &&Z80I_SET2_mIXd_L, &&Z80I_SET2_mIXd, &&Z80I_SET2_mIXd_A,	// D4-D7
	&&Z80I_SET3_mIXd_B, &&Z80I_SET3_mIXd_C, &&Z80I_SET3_mIXd_D, &&Z80I_SET3_mIXd_E,	// D8-DB
	&&Z80I_SET3_mIXd_H, &&Z80I_SET3_mIXd_L, &&Z80I_SET3_mIXd, &&Z80I_SET3_mIXd_A,	// DC-DF
	&&Z80I_SET4_mIXd_B, &&Z80I_SET4_mIXd_C, &&Z80I_SET4_mIXd_D, &&Z80I_SET4_mIXd_E,	// E0-E3
	&&Z80I_SET4_mIXd_H, &&Z80I_SET4_mIXd_L, &&Z80I_SET4_mIXd, &&Z80I_SET4_mIXd_A,	// E4-E7
	&&Z80I_SET5_mIXd_B, &&Z80I_SET5_mIXd_C, &&Z80I_SET5_mIXd_D, &&Z80I_SET5_mIXd_E,	// E8-EB
	&&Z80I_SET5_mIXd_H, &&Z80I_SET5_mIXd_L, &&Z80I_SET5_mIXd, &&Z80I_SET5_mIXd_A,	// EC-EF
	&&Z80I_SET6_mIXd_B, &&Z80I_SET6_mIXd_C, &&Z80I_SET6_mIXd_D, &&Z80I_SET6_mIXd_E,	// F0-F3
	&&Z80I_SET6_mIXd_H, &&Z80I_SET6_mIXd_L, &&Z80I_SET6_mIXd, &&Z80I_SET6_mIXd_A,	// F4-F7
	&&Z80I_SET7_mIXd_B, &&Z80I_SET7_mIXd_C, &&Z80I_SET7_mIXd_D, &&Z80I_SET7_mIXd_E,	// F8-FB
	&&Z80I_SET7_mIXd_H, &&Z80I_SET7_mIXd_L, &&Z80I_SET7_mIXd, &&Z80I_SET7_mIXd_A	// FC-FF
};

// Z80 instruction table: FDCB prefix
static void *z80_insn_table_FDCB[] = {
	&&Z80I_RLC_mIYd_B, &&Z80I_RLC_mIYd_C, &&Z80I_RLC_mIYd_D, &&Z80I_RLC_mIYd_E,	// 00-03
	&&Z80I_RLC_mIYd_H, &&Z80I_RLC_mIYd_L, &&Z80I_RLC_mIYd, &&Z80I_RLC_mIYd_A,	// 04-07
	&&Z80I_RRC_mIYd_B, &&Z80I_RRC_mIYd_C, &&Z80I_RRC_mIYd_D, &&Z80I_RRC_mIYd_E,	// 08-0B
	&&Z80I_RRC_mIYd_H, &&Z80I_RRC_mIYd_L, &&Z80I_RRC_mIYd, &&Z80I_RRC_mIYd_A,	// 0C-0F
	&&Z80I_RL_mIYd_B, &&Z80I_RL_mIYd_C, &&Z80I_RL_mIYd_D, &&Z80I_RL_mIYd_E,		// 10-13
	&&Z80I_RL_mIYd_H, &&Z80I_RL_mIYd_L, &&Z80I_RL_mIYd, &&Z80I_RL_mIYd_A,		// 14-17
	&&Z80I_RR_mIYd_B, &&Z80I_RR_mIYd_C, &&Z80I_RR_mIYd_D, &&Z80I_RR_mIYd_E,		// 18-1B
	&&Z80I_RR_mIYd_H, &&Z80I_RR_mIYd_L, &&Z80I_RR_mIYd, &&Z80I_RR_mIYd_A,		// 1C-1F
	&&Z80I_SLA_mIYd_B, &&Z80I_SLA_mIYd_C, &&Z80I_SLA_mIYd_D, &&Z80I_SLA_mIYd_E,	// 20-23
	&&Z80I_SLA_mIYd_H, &&Z80I_SLA_mIYd_L, &&Z80I_SLA_mIYd, &&Z80I_SLA_mIYd_A,	// 24-27
	&&Z80I_SRA_mIYd_B, &&Z80I_SRA_mIYd_C, &&Z80I_SRA_mIYd_D, &&Z80I_SRA_mIYd_E,	// 28-2B
	&&Z80I_SRA_mIYd_H, &&Z80I_SRA_mIYd_L, &&Z80I_SRA_mIYd, &&Z80I_SRA_mIYd_A,	// 2C-2F
	&&Z80I_SLL_mIYd_B, &&Z80I_SLL_mIYd_C, &&Z80I_SLL_mIYd_D, &&Z80I_SLL_mIYd_E,	// 30-33
	&&Z80I_SLL_mIYd_H, &&Z80I_SLL_mIYd_L, &&Z80I_SLL_mIYd, &&Z80I_SLL_mIYd_A,	// 34-37
	&&Z80I_SRL_mIYd_B, &&Z80I_SRL_mIYd_C, &&Z80I_SRL_mIYd_D, &&Z80I_SRL_mIYd_E,	// 38-3B
	&&Z80I_SRL_mIYd_H, &&Z80I_SRL_mIYd_L, &&Z80I_SRL_mIYd, &&Z80I_SRL_mIYd_A,	// 3C-3F
	&&Z80I_BIT0_B, &&Z80I_BIT0_C, &&Z80I_BIT0_D, &&Z80I_BIT0_E,			// 40-43
	&&Z80I_BIT0_H, &&Z80I_BIT0_L, &&Z80I_BIT0_mIYd, &&Z80I_BIT0_A,			// 44-47
	&&Z80I_BIT1_B, &&Z80I_BIT1_C, &&Z80I_BIT1_D, &&Z80I_BIT1_E,			// 48-4B
	&&Z80I_BIT1_H, &&Z80I_BIT1_L, &&Z80I_BIT1_mIYd, &&Z80I_BIT1_A,			// 4C-4F
	&&Z80I_BIT2_B, &&Z80I_BIT2_C, &&Z80I_BIT2_D, &&Z80I_BIT2_E,			// 50-53
	&&Z80I_BIT2_H, &&Z80I_BIT2_L, &&Z80I_BIT2_mIYd, &&Z80I_BIT2_A,			// 54-57
	&&Z80I_BIT3_B, &&Z80I_BIT3_C, &&Z80I_BIT3_D, &&Z80I_BIT3_E,			// 58-5B
	&&Z80I_BIT3_H, &&Z80I_BIT3_L, &&Z80I_BIT3_mIYd, &&Z80I_BIT3_A,			// 5C-5F
	&&Z80I_BIT4_B, &&Z80I_BIT4_C, &&Z80I_BIT4_D, &&Z80I_BIT4_E,			// 60-63
	&&Z80I_BIT4_H, &&Z80I_BIT4_L, &&Z80I_BIT4_mIYd, &&Z80I_BIT4_A,			// 64-67
	&&Z80I_BIT5_B, &&Z80I_BIT5_C, &&Z80I_BIT5_D, &&Z80I_BIT5_E,			// 68-6B
	&&Z80I_BIT5_H, &&Z80I_BIT5_L, &&Z80I_BIT5_mIYd, &&Z80I_BIT5_A,			// 6C-6F
	&&Z80I_BIT6_B, &&Z80I_BIT6_C, &&Z80I_BIT6_D, &&Z80I_BIT6_E,			// 70-73
	&&Z80I_BIT6_H, &&Z80I_BIT6_L, &&Z80I_BIT6_mIYd, &&Z80I_BIT6_A,			// 74-77
	&&Z80I_BIT7_B, &&Z80I_BIT7_C, &&Z80I_BIT7_D, &&Z80I_BIT7_E,			// 78-7B
	&&Z80I_BIT7_H, &&Z80I_BIT7_L, &&Z80I_BIT7_mIYd, &&Z80I_BIT7_A,			// 7C-7F
	&&Z80I_RES0_mIYd_B, &&Z80I_RES0_mIYd_C, &&Z80I_RES0_mIYd_D, &&Z80I_RES0_mIYd_E,	// 80-83
	&&Z80I_RES0_mIYd_H, &&Z80I_RES0_mIYd_L, &&Z80I_RES0_mIYd, &&Z80I_RES0_mIYd_A,	// 84-87
	&&Z80I_RES1_mIYd_B, &&Z80I_RES1_mIYd_C, &&Z80I_RES1_mIYd_D, &&Z80I_RES1_mIYd_E,	// 88-8B
	&&Z80I_RES1_mIYd_H, &&Z80I_RES1_mIYd_L, &&Z80I_RES1_mIYd, &&Z80I_RES1_mIYd_A,	// 8C-8F
	&&Z80I_RES2_mIYd_B, &&Z80I_RES2_mIYd_C, &&Z80I_RES2_mIYd_D, &&Z80I_RES2_mIYd_E,	// 90-93
	&&Z80I_RES2_mIYd_H, &&Z80I_RES2_mIYd_L, &&Z80I_RES2_mIYd, &&Z80I_RES2_mIYd_A,	// 94-97
	&&Z80I_RES3_mIYd_B, &&Z80I_RES3_mIYd_C, &&Z80I_RES3_mIYd_D, &&Z80I_RES3_mIYd_E,	// 98-9B
	&&Z80I_RES3_mIYd_H, &&Z80I_RES3_mIYd_L, &&Z80I_RES3_mIYd, &&Z80I_RES3_mIYd_A,	// 9C-9F
	&&Z80I_RES4_mIYd_B, &&Z80I_RES4_mIYd_C, &&Z80I_RES4_mIYd_D, &&Z80I_RES4_mIYd_E,	// A0-A3
	&&Z80I_RES4_mIYd_H, &&Z80I_RES4_mIYd_L, &&Z80I_RES4_mIYd, &&Z80I_RES4_mIYd_A,	// A4-A7
	&&Z80I_RES5_mIYd_B, &&Z80I_RES5_mIYd_C, &&Z80I_RES5_mIYd_D, &&Z80I_RES5_mIYd_E,	// A8-AB
	&&Z80I_RES5_mIYd_H, &&Z80I_RES5_mIYd_L, &&Z80I_RES5_mIYd, &&Z80I_RES5_mIYd_A,	// AC-AF
	&&Z80I_RES6_mIYd_B, &&Z80I_RES6_mIYd_C, &&Z80I_RES6_mIYd_D, &&Z80I_RES6_mIYd_E,	// B0-B3
	&&Z80I_RES6_mIYd_H, &&Z80I_RES6_mIYd_L, &&Z80I_RES6_mIYd, &&Z80I_RES6_mIYd_A,	// B4-B7
	&&Z80I_RES7_mIYd_B, &&Z80I_RES7_mIYd_C, &&Z80I_RES7_mIYd_D, &&Z80I_RES7_mIYd_E,	// B8-BB
	&&Z80I_RES7_mIYd_H, &&Z80I_RES7_mIYd_L, &&Z80I_RES7_mIYd, &&Z80I_RES7_mIYd_A,	// BC-BF
	&&Z80I_SET0_mIYd_B, &&Z80I_SET0_mIYd_C, &&Z80I_SET0_mIYd_D, &&Z80I_SET0_mIYd_E,	// C0-C3
	&&Z80I_SET0_mIYd_H, &&Z80I_SET0_mIYd_L, &&Z80I_SET0_mIYd, &&Z80I_SET0_mIYd_A,	// C4-C7
	&&Z80I_SET1_mIYd_B, &&Z80I_SET1_mIYd_C, &&Z80I_SET1_mIYd_D, &&Z80I_SET1_mIYd_E,	// C8-CB
	&&Z80I_SET1_mIYd_H, &&Z80I_SET1_mIYd_L, &&Z80I_SET1_mIYd, &&Z80I_SET1_mIYd_A,	// CC-CF
	&&Z80I_SET2_mIYd_B, &&Z80I_SET2_mIYd_C, &&Z80I_SET2_mIYd_D, &&Z80I_SET2_mIYd_E,	// D0-D3
	&&Z80I_SET2_mIYd_H, &&Z80I_SET2_mIYd_L, &&Z80I_SET2_mIYd, &&Z80I_SET2_mIYd_A,	// D4-D7
	&&Z80I_SET3_mIYd_B, &&Z80I_SET3_mIYd_C, &&Z80I_SET3_mIYd_D, &&Z80I_SET3_mIYd_E,	// D8-DB
	&&Z80I_SET3_mIYd_H, &&Z80I_SET3_mIYd_L, &&Z80I_SET3_mIYd, &&Z80I_SET3_mIYd_A,	// DC-DF
	&&Z80I_SET4_mIYd_B, &&Z80I_SET4_mIYd_C, &&Z80I_SET4_mIYd_D, &&Z80I_SET4_mIYd_E,	// E0-E3
	&&Z80I_SET4_mIYd_H, &&Z80I_SET4_mIYd_L, &&Z80I_SET4_mIYd, &&Z80I_SET4_mIYd_A,	// E4-E7
	&&Z80I_SET5_mIYd_B, &&Z80I_SET5_mIYd_C, &&Z80I_SET5_mIYd_D, &&Z80I_SET5_mIYd_E,	// E8-EB
	&&Z80I_SET5_mIYd_H, &&Z80I_SET5_mIYd_L, &&Z80I_SET5_mIYd, &&Z80I_SET5_mIYd_A,	// EC-EF
	&&Z80I_SET6_mIYd_B, &&Z80I_SET6_mIYd_C, &&Z80I_SET6_mIYd_D, &&Z80I_SET6_mIYd_E,	// F0-F3
	&&Z80I_SET6_mIYd_H, &&Z80I_SET6_mIYd_L, &&Z80I_SET6_mIYd, &&Z80I_SET6_mIYd_A,	// F4-F7
	&&Z80I_SET7_mIYd_B, &&Z80I_SET7_mIYd_C, &&Z80I_SET7_mIYd_D, &&Z80I_SET7_mIYd_E,	// F8-FB
	&&Z80I_SET7_mIYd_H, &&Z80I_SET7_mIYd_L, &&Z80I_SET7_mIYd, &&Z80I_SET7_mIYd_A,	// FC-FF
};