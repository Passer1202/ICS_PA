#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
 uintptr_t *p_args = args;
  int argc = *p_args;
  p_args += 1;
  char **argv = (char **)p_args;
  //for (int i = 0; i < argc; ++i) printf("argv[%d]: %s\n", i, argv[i]);
  p_args += (argc + 1);
  char **envp = (char **)p_args;
  environ = envp;
  exit(main(argc, argv, envp));
  assert(0);
}
