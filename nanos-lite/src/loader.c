#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif


//未进行ISA类型检查，后续或补

size_t ramdisk_read(void *buf, size_t offset, size_t len);
int fs_open(const char *pathname, int flags, int mode);
size_t fs_lseek(int fd,size_t offset,int whence);
size_t fs_read(int fd, void *buf, size_t len);
int fs_close(int fd);

//Ehdr----header
//Phdr----segement

static uintptr_t loader(PCB *pcb, const char *filename) {
	
	int fd = fs_open(filename,0,0);
	
	//检查忽略掉的调用
	assert(fd>=3);
	
	Elf_Ehdr ehdr;
	
	//ramdisk_read(&ehdr,0,sizeof(Elf_Ehdr));
	assert(fs_read(fd,&ehdr,sizeof(Elf_Ehdr))==sizeof(Elf_Ehdr));
	
	//check the magic number
	assert(*(uint32_t *)ehdr.e_ident == 0x464c457f);
	
	//printf("%d\n",ehdr.e_entry);
	
	//读取段信息
	Elf_Phdr phdr[ehdr.e_phnum];
	
	fs_lseek(fd, ehdr.e_phoff, 0);
	//ramdisk_read(phdr, ehdr.e_phoff, sizeof(Elf_Phdr)*ehdr.e_phnum);
	assert(fs_read(fd,&phdr,sizeof(Elf_Phdr)*ehdr.e_phnum)==sizeof(Elf_Phdr)*ehdr.e_phnum);
	
	
	//处理段信息
	for(int i=0;i<ehdr.e_phnum;i++){
	
		if(phdr[i].p_type!=PT_LOAD)continue;//检测是否是需要加载的段
		//加载对应段到主存
		assert(phdr[i].p_type==PT_LOAD);
		//ramdisk_read((void*)phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_memsz);
		
		fs_lseek(fd, phdr[i].p_offset, 0);
		assert(fs_read(fd,(void*)phdr[i].p_vaddr,phdr[i].p_memsz)==phdr[i].p_memsz);
		//将[VirtAddr + FileSiz, VirtAddr + MemSiz)对应的物理区间清零
		memset((void*)(phdr[i].p_vaddr+phdr[i].p_filesz), 0, phdr[i].p_memsz - phdr[i].p_filesz);
		

	}
	
	assert(fs_close(fd) == 0);
	
  return ehdr.e_entry;
}

void context_kload(PCB *pcb, void (*entry)(void *), void *arg) {
  Area kcontext_stack;
  kcontext_stack.start = pcb->stack;
  kcontext_stack.end = pcb->stack + STACK_SIZE;
  pcb->cp = kcontext(kcontext_stack, entry, arg);
}

uintptr_t* good_bound(uintptr_t* s){
	return (uintptr_t*)((uintptr_t)s & ~(sizeof(uintptr_t) - 1));
}

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[])  {
 //assert(0);
    Area pcb_stack;
    
    pcb_stack.start = pcb->stack;
    pcb_stack.end = pcb->stack + STACK_SIZE;
 
    uintptr_t entry = loader(pcb, filename);
 
    int argc = 0,envc = 0;
    while (argv[argc++] != NULL);
    argc--;
    while (envp[envc++] != NULL) ;
    envc--;
 
   
    uintptr_t* u_s = (uintptr_t*)heap.end;
    
    uintptr_t* p_heap = (uintptr_t*)heap.end;
 
    
    int index=0;
    
    while(index<argc){
    	size_t size = strlen(argv[index]) + 1;
    	u_s-=size;
    	char* p_us=(char*)u_s;
    	for(int i=0;i<size;i++)
    		*(p_us+i)=*(argv[index]+i);
    	index++;
    }
 
   index=0;
   while(index<envc){
    	size_t size = strlen(envp[index]) + 1;
    	u_s-=size;
    	char* p_us=(char*)u_s;
    	for(int i=0;i<size;i++)
    		*(p_us+i)=*(envp[index]+i);
    	index++;
    }
 
    u_s=good_bound(u_s);
 
    u_s =  u_s - (argc + envc + 3);  
 
    u_s[0] = argc;
 
    index=0;
    
    while(index<argc){
    	size_t size = strlen(argv[index]) + 1;
    	p_heap-=size;
    	u_s[index + 1]=(uintptr_t)p_heap;
    	index++;
    }
    u_s[index + 1]=0;
    
    index=0;
    while(index<envc){
    	size_t size = strlen(envp[index]) + 1;
    	p_heap-=size;
    	u_s[argc + 1 + index + 1]=(uintptr_t)p_heap;
    	index++;
    }
    u_s[argc + 1 + index + 1]=0;
    
    printf("%s\n",u_s[0]);
    pcb->cp = ucontext(NULL, pcb_stack, (void*)entry);
   assert(0);
    pcb->cp->GPRx = (uintptr_t)u_s;
}

void naive_uload(PCB *pcb, const char *filename) {

  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %d", entry);
 // assert(0);
  ((void(*)())entry) ();
 // assert(0);
}

