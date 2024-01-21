#include <memory.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {

	pf += nr_page * PGSIZE;
	
	return pf;
	
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
	//Nanos-lite的pg_alloc(). pg_alloc()的参数是分配空间的字节数, 
	//但我们保证AM通过回调函数调用pg_alloc()时申请的空间总是页面大小的整数倍, 
	//因此可以通过调用new_page()来实现pg_alloc(). 此外pg_alloc()还需要对分配的页面清零.
	
	int p_num = n / PGSIZE;
  	assert(p_num * PGSIZE == n);//根据保证（）
  	void *page_end = new_page(p_num);
  	void *page_start = page_end - n;
  	
  	
  	memset(page_start, 0, n);//清零
  	
  	return page_start;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
