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
      switch(*++fmt){
        case'd':{
          int num=va_arg(ap,int);
          if(num<0){*out++='-';num=(-num);}
          else if(num==0){*out++='0';}
          else{
		  int cnt=0;
		  char src[15];
		  while(num!=0){
		    src[cnt++]=(num%10+'0');
		    num/=10;
		  }
		  cnt--;
		  while(cnt>=0){
		    *out++=src[cnt--];
		  }
          }
          break;
          }
        
        case 's':{
          char* str=va_arg(ap,char*);
          while(*str!='\0'){
            *out++=*str++;
          }  
          break;
          }
      }
      fmt++;
    }
    else{
      *out++=*fmt++;//maybe wrong
    }
    
  }
  *out='\0';

  va_end(ap);

  return (out-start-1);
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
