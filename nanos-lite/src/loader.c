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

//Ehdr----header
//Phdr----segement

static uintptr_t loader(PCB *pcb, const char *filename) {
	
	Elf_Ehdr ehdr;
	ramdisk_read(&ehdr,0,sizeof(Elf_Ehdr));
	//check the magic number
	assert(*(uint32_t *)ehdr.e_ident == 0x464c457f);
	
	//printf("%d\n",ehdr.e_entry);
	
	//读取段信息
	Elf_Phdr phdr[ehdr.e_phnum];
	ramdisk_read(phdr, ehdr.e_phoff, sizeof(Elf_Phdr)*ehdr.e_phnum);
	
	//处理段信息
	for(int i=0;i<ehdr.e_phnum;i++){
		if(phdr[i].p_type!=0)continue;//检测是否是需要加载的段
		//加载对应段到主存
		assert(phdr[i].p_type==PT_LOAD);
		ramdisk_read((void*)phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_memsz);
		//将[VirtAddr + FileSiz, VirtAddr + MemSiz)对应的物理区间清零
		memset((void*)(phdr[i].p_vaddr+phdr[i].p_filesz), 0, phdr[i].p_memsz - phdr[i].p_filesz);


	}
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %d", entry);
 // assert(0);
  ((void(*)())entry) ();
 // assert(0);
}

