#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"


SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  assert(freesrc == 0);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
	//assert(0);//debug
	//用libc中的文件操作打开文件, 并获取文件大小
	FILE * file_point = fopen(filename, "r");
    	if (!file_point) return NULL;
    	//assert(0);//debug
    	fseek(file_point, 0L, SEEK_END);
    	long read_len = ftell(file_point);
    	rewind(file_point);
    	//申请一段大小为size的内存区间buf	
    	uint8_t * img_buf = (uint8_t *)malloc(read_len * sizeof(uint8_t));
    	//将整个文件读取到buf中
    	assert(fread(img_buf, 1, read_len, file_point) == read_len);
	//将buf和size作为参数, 调用STBIMG_LoadFromMemory(), 它会返回一个SDL_Surface结构的指针
    	SDL_Surface * ret = STBIMG_LoadFromMemory(img_buf, read_len);
    	assert(ret != NULL);
	//关闭文件, 释放申请的内存
    	fclose(file_point);
    	free(img_buf);
	//返回SDL_Surface结构指针
	//assert(0);//debug
    	return ret;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
