#ifndef midi_asm_h__
#define midi_asm_h__

//for pi32
#define MUL(macc,Rm,Rn)				__builtin_pi32_smul64(Rm,Rn);
#define MAC(macc,Rm,Rn)				__builtin_pi32_smla64(Rm,Rn);
#define MAS(macc,Rm,Rn)				__builtin_pi32_smls64(Rm,Rn);
#define MACDIV(r,macc,d)			__asm__ volatile ("sdivmacc %1; mov %0,maccl;":"=r" (r): "r" (d):"macch","maccl")
#define MRSI(Rm,macc,Rn)			(Rm = __builtin_pi32_sreadmacc(Rn));   //mac read right shift imm
#define MRSR(Rm,macc,Rn)			(Rm = __builtin_pi32_sreadmacc(Rn));   //mac read right shift register
#define MULSI(Ro,macc,Rm,Rn,Rp)		{MUL(macc,Rm,Rn);MRSI(Ro,macc,Rp);}
#define MULRSR(r,macc,a,b,shift)	{MUL(macc,a,b);MRSR(r,macc,shift);}
#define MULRS(r,macc,a,b,shift)		{MUL(macc,a,b);MRSI(r,macc,shift);}
#define MACSR(macc,Rn)				__asm__ volatile ("asrmacc %0;"::"r"(Rn):"macch","maccl")
#define MACSL(macc,Rn)				__asm__ volatile ("lslmacc %0;"::"r"(Rn):"macch","maccl")
#define VSHL(r,a,b)					r = b > 0 ? (a << b) : a >> (-b)
#define MACGET(h,l,macc)			__asm__ volatile ("mov %0,macch; mov %1,maccl;":"=r" (h), "=r" (l))


#endif // midi_dec_h__



