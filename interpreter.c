#include <stdio.h>
#include <stdlib.h>
const char *demo=
#include "demo.h"
;
#define T char
typedef T type_t;
//Please use type_t to handle with values relates to VM,
//and use size_t to handle values relates to host.
const size_t size=64*1024;
struct Register{
	const char *rom;
	size_t pc;//Program Counter
	size_t dp;//Data Pointer
	type_t *ram;
	size_t size;
	char *err_msg;
};
void match_bracket(struct Register *r, char mode){
	size_t i=r->pc;
	size_t count=0;
	switch(mode){
		case '[':
			do{
				if(r->rom[i]=='['){
					count++;
				}else if(r->rom[i]==']'){
					count--;
				}
			}while(r->rom[i++]&&count);
			break;
		case ']':
			do{
				if(r->rom[i]=='['){
					count--;
				}else if(r->rom[i]==']'){
					count++;
				}
			}while(--i!=-1&&count);
			break;
	}
	if(!count){
		r->pc=i;
	}else{
		r->err_msg="Could not find the respective bracket.";
	}
}
enum State {HALTED, USE_INTERNAL, LOAD_FILE, INIT, RUNNING, ERROR}
main(enum State s, char **argv){
	type_t memory[size];
	struct Register r;
	while(s){
		switch(s){
			case USE_INTERNAL:
				r.rom=demo;
				s=INIT;
				break;
			case LOAD_FILE:
				break;
			case INIT:
				r.pc=0;
				r.dp=0;
				r.ram=memory;
				r.size=size;
				r.err_msg=NULL;
				s=RUNNING;
				break;
			case RUNNING:
				switch(r.rom[r.pc]){
					case '>':
						r.dp++;
						break;
					case '<':
						r.dp--;
						break;
					case '+':
						r.ram[r.dp]++;
						break;
					case '-':
						r.ram[r.dp]--;
						break;
					case '.':
						putchar(r.ram[r.dp]);
						break;
					case ',':
						r.ram[r.dp]=getchar();
						break;
					case '[':
						if(!r.ram[r.dp]){
							match_bracket(&r, '[');
						}
						break;
					case ']':
						if(r.ram[r.dp]){
							match_bracket(&r, ']');
						}
						break;
					case '\0':
						s=HALTED;
					default:
				}
				if(!r.err_msg){
					r.pc++;
					break;
				}
			case ERROR:
				break;
		}
	}
	return s;
}
/*TODO:
1. Add boundary checks
2. Add unimpleneted routines
3. Use mmap for memory allocation
*/
