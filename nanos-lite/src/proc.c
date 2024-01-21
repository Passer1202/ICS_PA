#include <proc.h>

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void (*entry)(void *), void *arg);
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

//size_t get_ramdisk_size();

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void init_proc() {
  
  //context_kload(&pcb[0], hello_fun, "Begin");
  char *argv[] = {"exec-test",NULL};
  char *envp[] = {NULL};
  context_uload(&pcb[0], "exec-test", argv, envp);
  
  switch_boot_pcb();
  
  Log("Initializing processes...");

  // load program here
  //printf("%d",get_ramdisk_size());
  //PA4.1注释掉，
  //const char filename[] = "/bin/nterm";
  //naive_uload(NULL, filename);
}

Context* schedule(Context *prev) {
	// save the context pointer
	current->cp = prev;

	// switch between pcb[0] and pcb[1]
	current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

	// then return the new context
	return current->cp;
}
