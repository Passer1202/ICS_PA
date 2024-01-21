#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

//以下为实现分页机制添加的宏

//页内偏移，[11:0]
#define VA_OFFSET(addr) (addr & 0x00000FFF)
//一级页号，[21:12]
#define VPN_L(addr)     ((addr >> 12) & 0x000003FF)
//二级页号，[31:22]
#define VPN_H(addr)     ((addr >> 22) & 0x000003FF)
//物理页偏移，[11:0]
#define PA_OFFSET(addr) (addr & 0x00000FFF)
//物理页号，[31:12]
#define PPN(addr)	((addr >> 12) & 0x000FFFFF)

#define PTE_PPN 0xFFFFF000

//以上为实现分页机制添加的宏

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}

void map(AddrSpace *as, void *va, void *pa, int prot) {

	uintptr_t my_va = (uintptr_t) va;
  	uintptr_t my_pa = (uintptr_t) pa;
  	
  	assert(VA_OFFSET(my_va)==0);
  	assert(PA_OFFSET(my_pa)==0);
  	
  	//Todo:提取虚拟地址的二级页号和一级页号，以及物理地址的物理页号
  	uint32_t ppn=PPN(my_pa);
  	uint32_t vpn_l=VPN_L(my_va);
  	uint32_t vpn_h=VPN_H(my_va);
  	
  	//Todo:获取地址空间的页表基址和一级页表的目标位置
  	PTE * page_dir_base = (PTE *) as->ptr;//页表基址
  	PTE * page_table_p = page_dir_base + vpn_h;//一级页表的位置
  	
  	//Todo:如果一级页表中的页表项的地址(二级页表的基地址)为空，创建并填写页表项
  	
  	if(*page_table_p!=0){
  		
  		PTE* page_table=(PTE*)((*page_table_p)&PTE_PPN);
  		
  		PTE* table_index=page_table+vpn_l;
  		
  		*table_index=(ppn<<12)|PTE_V|PTE_R|PTE_W|PTE_X;
  	
  	}
  	else{
  		// Todo:pgalloc_usr分配一页物理内存
  		PTE* new_page_table=(PTE*)pgalloc_usr(PGSIZE);
  		//Todo:将这个基地址填写到一级页表的页表项中，同时设置 PTE_V 表示这个页表项是有效的。
  		*page_table_p = ((PTE) new_page_table) | PTE_V;
  		//Todo:计算二级页表位置
  		PTE* table_index=new_page_table+vpn_l;
  		
  		*table_index=(ppn<<12)|PTE_V|PTE_R|PTE_W|PTE_X;
  	
  	}
  	
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *p_ucontext = (Context *)(kstack.end-sizeof(Context));
  p_ucontext->mepc=(uintptr_t) entry;
  return p_ucontext;
}
