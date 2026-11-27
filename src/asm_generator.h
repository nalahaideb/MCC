#ifndef ASM_GENERATOR_H
#define ASM_GENERATOR_H
#include <vector>
#include <string>
#include <unordered_map>

class ASM_generator
{
 public:
	ASM_generator();
	
	inline void run();
	inline void read_file(const std::string f);
	inline void to_file(const std::string f);
	
 private:
	
	//IR line handlers + frame handling
	//most of these functions are similar to ones lowering C to IR
	
	//FUNCTION f(t1, t2)
	inline std::string func(const std::string s);
	
	//END -> fn_f_ret label
	inline std::string end(const std::string s);
	
	//t1 := CALL f(t2, t3) -> args into a0.., jal, a0 into dest slot
	inline std::string call(const std::string s);
	
	//t1 := - t2
	inline std::string neg(const std::string s);
	
	//temp -> stack offset, t3 -> "-20(s0)"
	inline std::string offset(const std::string t);
	
	//highest temp found in frame sets offset distance from s0 on stack
	//8 + 4 * N, N is the highest temp num found in function 
	//t1 = 8 + 4 * 1 = 12, round up to 16 for RISC convention on sp alignment
	inline unsigned int frame(const size_t i);
	
	//operand -> "li" for a literal
	//operand -> "lw" from offset for a temp
	inline std::string atom(const std::string s);

	//creates stub label and jump to enter main before other instructions are read
	void enter();
	void exit();
	
	inline std::vector< std::string > lower(const std::string s);
	inline std::string assign(const std::string s);
	inline std::string jump(const std::string s);
	inline std::string label(const std::string s);
	
	inline std::string opr(const std::string s);
	inline std::string binop(const std::string s);
	//IF t1 relop t2 THEN l1 ELSE l2 -> branch on relop, j to else label
	inline std::string boolop(const std::string s);
	
	inline std::string load(const std::string s);
	inline std::string la(const std::string s);
	inline std::string li(const std::string s);
	inline std::string lui(const std::string s);
	inline std::string ecall(const std::string s);
	inline std::string ret(const std::string s);
	inline std::string jal(const std::string s);
	inline std::string jr(const std::string s);
	inline std::string mv(const std::string s);

	inline std::string add(const std::string s);
	inline std::string addi(const std::string s);
	inline std::string sub(const std::string s);
	inline std::string mul(const std::string s);
	inline std::string div(const std::string s);
	inline std::string rem(const std::string s);
		
	inline std::string beq(const std::string s);
	inline std::string bgt(const std::string s);
	inline std::string bge(const std::string s);
	inline std::string bgte(const std::string s);
	inline std::string bgtz(const std::string s);
	inline std::string blt(const std::string s);
	inline std::string ble(const std::string s);
	inline std::string blte(const std::string s);
	inline std::string bltz(const std::string s);
	
	inline std::string slt(const std::string s);
	
	inline std::string lw(const std::string s);
	inline std::string sw(const std::string s);
		
	//t1 := CALL read()
	inline void read(const std::string s);
	
	//t2 := CALL print(t1)
	//also prints a newline
	inline void print(const std::string s);

	//stores a label and its memory location
	std::unordered_map< std::string, unsigned int > data;

	std::vector< std::string > output;
	unsigned int position = 1;
	std::vector< std::string > file_content;

	//current functions name and frame size
	std::string fname;
	unsigned int fsize = 0;
	};
#endif
