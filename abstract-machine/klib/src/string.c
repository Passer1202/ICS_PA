#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t cnt=0;
  while(*s++!='\0')
   cnt++;
  return cnt;
}

char *strcpy(char *dst, const char *src) {
  //panic("Not implemented");
  assert(dst!=NULL&&src!=NULL);
  char* ret=dst;
  while(*src!='\0'){
  *dst++=*src++;
  }
  *dst='\0';
  return ret;
}

char *strncpy(char *dst, const char *src, size_t n) {
  panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
  //panic("Not implemented");
  char* ret=dst;
  while(*dst!='\0')
  {
  	dst++;
  }
  while(*src!='\0'){
  	*dst++=*src++;
  }
  *dst='\0';
  
  return ret;
}

int strcmp(const char *s1, const char *s2) {
  //panic("Not implemented");
  while(*s1!='\0'&&*s2!='\0'){
	  if(*s1>*s2){
	  	return 1;
	  }
	  else if(*s1<*s2){
	  	return -1;
	  }
	  else{
		  s1++;
		  s2++;
	  }
  }
  if(*s1=='\0'&&*s2=='\0')return 0;
  else if(*s1=='\0'&&*s2!='\0')return -1;
  else return 1;
  
}

int strncmp(const char *s1, const char *s2, size_t n) {

  assert(0);
  for(int i=0;i<n;i++)
  {
  	if(*s1>*s2){
  		return 1;
  	}
  	else if(*s1<*s2){
  		return -1;
  	}
  	else{
  		s1++;
  		s2++;
  	}
  }
  return 0;
}

void *memset(void *s, int c, size_t n) {
  //panic("Not implemented");
  char* r=s;
  while(n--){
  	*r++ = c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  //panic("Not implemented");
  unsigned char*pout=(unsigned char*)out;
  unsigned char*pin=(unsigned char*)in;
  while(n--){
  *pout=*pin;
  pout++;
  pin++;
  }
  
  return out;
  
}

int memcmp(const void *s1, const void *s2, size_t n) {
  //panic("Not implemented");
  const unsigned char* a = s1;
  const unsigned char* b = s2;
  while(n--){
  	if(*a-*b!=0){return *a-*b;}
  	else {a++;b++;}
  }
  return 0;
}

#endif
