#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)


int vsprintf(char *out, const char *fmt, va_list ap) {
  //panic("Not implemented");
  char* start=out;
  
  //va_start(ap,fmt);
  while(*fmt!='\0'){
    if(*fmt=='%'){
      switch(*++fmt){
        case'd':{
          int32_t num=va_arg(ap,int);
      
         if(num<0){
          *out++='-';
          num=-2147483647;
          num=-num;
          }
         if(num==0){*out++='0';}
          else{
		  int cnt=0;
		  char src[32];
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
  
  return (out-start);

}

int printf(const char *fmt, ...) {
  //panic("Not implemented");
  char out[8192]={'\0'};;//buffer,可能不够大？
  va_list ap;
  va_start(ap,fmt);
  
  int ret=vsprintf(out,fmt,ap);
  //if(ret>=100000||ret<0)ret=-1;
  va_end(ap);
  int index_printf=0;
  while(out[index_printf]!='\0'){
  	//putch(' ');
  	putch(out[index_printf++]);
  	//putch(' ');
  }
  return ret;
}


int sprintf(char *out, const char *fmt, ...) {
  //panic("Not implemented");
  int ans=0;
  
  va_list ap;
  va_start(ap,fmt);
  
  ans=vsprintf(out,fmt,ap);
  
  va_end(ap);
  
  return ans;
  
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
