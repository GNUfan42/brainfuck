#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
const char *demo=
#include "demo.h"
;
//#define T char
typedef T type_t;
//Please use type_t to handle with values relates to VM,
//and use size_t to handle values relates to host.
#define SIZE 64*1024
struct Register{
	const char *rom;
	size_t pc;//Program Counter
	size_t dp;//Data Pointer
	type_t *ram;
	size_t size;
	char *err_msg;
};//ram MUST be initialized to 0!
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
		r->pc=i+1;
	}else{
		r->err_msg="Could not find the respective bracket.";
	}
}
size_t filesize(int fd){
	struct stat statbuf;
	if(fstat(fd, &statbuf)==-1){
		return SIZE_MAX;
	}
	return statbuf.st_size;
}
char *map_file(char *filename, size_t *unmap_info){
	int fd;
	void *p=NULL;
	size_t size;
	if((fd=open(filename, O_RDONLY))<0){
		goto ret;
	}
	size=filesize(fd);
	if(p=mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0)){
		if(unmap_info){
			*unmap_info=size;
		}
	}
	close(fd);
ret:	return p;
}

enum State {HALTED, USE_INTERNAL, LOAD_FILE, INIT, RUNNING, ERROR}
main(enum State s, char **argv){
	type_t memory[SIZE]={0};
	struct Register r;
	size_t unmap_info=0;
	while(s){
		switch(s){
			case USE_INTERNAL:
				r.rom=demo;
				s=INIT;
				break;
			case LOAD_FILE:
				if(r.rom==0x1337){
					r.err_msg=(char*)r.ram;
					snprintf(r.err_msg, r.size*sizeof(type_t)/sizeof(char), "Could not map the file %s: errno: %d.", argv[1], errno);
					s=ERROR;
					break;
				}else if(r.rom=map_file(argv[1], &unmap_info)){
					s=INIT;
					break;
				}//This fallthrough is necessary
			case INIT:
				r.pc=0;
				r.dp=0;
				r.ram=memory;
				r.size=SIZE;
				r.err_msg=NULL;
				if(s==INIT){
					s=RUNNING;
				}else{
					r.rom=0x1337;
				}
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
				if(r.dp==-1||r.dp>=r.size){
					r.err_msg="Memory access is out of bounds.";
				}
				if(!r.err_msg){
					r.pc++;
					break;
				}
				s=ERROR;
			case ERROR:
				fprintf(stderr, "Error: %s\n", r.err_msg);
				fprintf(stderr, "pc: %p\ndp: %p\nmem_size: %p\n", r.pc, r.dp, r.size);
				s=HALTED;
				break;
		}
	}
	//Remember to cleanup here
	if(!unmap_info){
		munmap(r.rom, unmap_info);
	}
	return s;
}
/*TODO:
1. Add boundary checks				OK
2. Add unimpleneted routines			OK
3. Use mmap for flexible memory allocation	--
*/
