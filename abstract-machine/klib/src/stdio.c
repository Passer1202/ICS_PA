#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  //panic("Not implemented");
  char* start=out;
  va_list ap;
  va_start(ap,fmt);
  while(*fmt!='\0'){
    if(*fmt=='%'){
      fmt++;
      
    }
    else{
      *(out++)=*(fmt++);//maybe wrong
    }
    
  }
  *out='\0';

  va_end(ap);

  return (out-start);
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
