#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static void reverse(char *s, int len) {
  char *end = s + len - 1;
  char tmp;
  while (s < end) {
    tmp = *s;
    *s = *end;
    *end = tmp;
    s++;
    end--;
  }
}

static void itoa(int n, char *s) {
  int i = strlen(s), sign = n, bit;
  if (sign < 0) {n = -n;s[i++]='-';s[i]='\0';}
    char st[100];
    char*p=st;
    char*q=st;
  do {
    bit=n%10;
    *p='0'+bit;
    p++;
    n/=10;
  } while (n > 0);
  *p = '\0';
  reverse(q, p-q);
  strcat(s,q);
}

int printf(const char *fmt, ...) {
  char buffer[8192]={'\0'};
  va_list aptr;
  int number=0;
  va_start(aptr,fmt);
  number=vsprintf(buffer,fmt,aptr);
  va_end(aptr);
  for(int i=0;buffer[i]!='\0';i++){
    putch(buffer[i]);
  }
  return number;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *start = out;
  for (; *fmt != '\0'; fmt++) 
  {
    if (*fmt != '%') {
      out=start+strlen(start);
      *out = *fmt;
    } 
    else {
      switch (*(++fmt)) {
      case 'd': itoa(va_arg(ap, int), start); break;
      case 'c': char c = va_arg(ap, int);*(start+strlen(start))=c; *(start+strlen(start))='\0';break;
      case 's': char *s = va_arg(ap, char*); strcat(start, s); break;
      }
    }
   }
    *(start+strlen(start))='\0';
    return strlen(start);
}
int sprintf(char *out, const char *fmt, ...) {
  memset(out,'\0',strlen(out));
  va_list pArgs;
  va_start(pArgs, fmt);
  char *start = out;
  for (; *fmt != '\0'; fmt++) 
  {
    if (*fmt != '%') {
      out=start+strlen(start);
      *out = *fmt;
    } 
    else {
      switch (*(++fmt)) {
      case 'd': itoa(va_arg(pArgs, int), start); break;
      case 'c': char c = va_arg(pArgs, int); *(start+strlen(start))=c; *(start+strlen(start))='\0';break;
      case 's': char *s = va_arg(pArgs, char*); strcat(start, s); break;
      }
    }
  }
  *(start+strlen(start))='\0';
  va_end(pArgs);

  return strlen(start);
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
panic("Not implemented1");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented2");
}

#endif
