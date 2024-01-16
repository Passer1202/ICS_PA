#include <fs.h>


typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);


typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

typedef struct{
	size_t fd;//文件描述符
	size_t fd_offset;//偏移量记录
}FD_OF_info;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, DEV_EVENTS,PROC_DISPINFO,FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [DEV_EVENTS]= {"/dev/events", 0, 0, events_read, invalid_write},
  [PROC_DISPINFO] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
  [FD_FB] = {"/dev/fb", 0, 0, invalid_read, fb_write},
#include "files.h"
};

static FD_OF_info file_o_table[LENGTH(file_table)];//保存已打开文件信息;

static size_t file_o_total=0;//现在打开的文件的索引

static int get_index(size_t fd){
	
	int ans=0;
	while(ans<file_o_total){
		if(file_o_table[ans].fd==fd)return ans;
		ans++;
	}
	
	return -1;//表示对应文件描述符的文件未打开
}

int fs_open(const char *pathname, int flags, int mode){
	//忽略了flags和mode参数
	//忽略了 stdin,stdout,stderr的打开
	int index=0;
	while(index<LENGTH(file_table)){
		Finfo file_find=file_table[index];
		if(strcmp(pathname,file_find.name)==0){
		
			if(index>=3){
				file_o_table[file_o_total].fd=index;
				file_o_table[file_o_total++].fd_offset=0;
				
			}
			else Log("File to be ignored:stdin, stdout or stderr!");
			
			return index;
		}
		index++;
	
	}
	
	panic("should not reach here");

}

size_t fs_read(int fd, void *buf, size_t len){
	//同fs_open忽略了对stdin,stdout,stderr的读操作
	
	ReadFn file_read;
	file_read=file_table[fd].read;
	if(file_read!=NULL){
		size_t ret=file_read(buf,0,len);
		return ret;
	}
	
	if(fd>=3){
		int index=get_index(fd);
		if(index==-1){
			Log("File is not opened!");
			return -1;
		}
		size_t file_size=file_table[fd].size;
  		size_t disk_offset=file_table[fd].disk_offset;
  		size_t file_offset=file_o_table[index].fd_offset;
  		
  		if(file_offset>file_size){
  			Log("offset is out of the bound");
  			return 0;//即不进行读操作
  		}
  		
  		else{
	  		size_t r_len=(file_offset+len<=file_size)?len:(file_size-file_offset);
	  		
	  		file_o_table[index].fd_offset+=r_len;
	  		
	  		ramdisk_read(buf,disk_offset+file_offset,r_len);
	  		
	  		return r_len;
  		}
		
	}
	else Log("File to be ignored:stdin, stdout or stderr!");
	return 0;

}



size_t sys_write(int fd, void *buf, size_t cnt){
    if(fd!=1&&fd!=2)return -1;//调用失败
    char* out=buf;
    size_t num=cnt;
    while(num--){
    	putch(*out++);
    }
    
    return cnt;
}



size_t fs_write(int fd, const void *buf, size_t len){

	//对stdin,stdout,stderr进行识别
	
	WriteFn file_write;
	file_write=file_table[fd].write;
	if(file_write!=NULL){
		size_t ret=file_write(buf,0,len);
		return ret;
	}
	
	
	switch(fd){
	case 0:
		Log("File to be ignored:stdin!");return 0;
	case 1:case 2: 
		return sys_write(fd, (char*)buf, len);
	}
	
	int index=get_index(fd);
	if(index==-1){
		Log("File is not opened!");	
		return -1;
	}
	
	size_t file_size=file_table[fd].size;
  	size_t disk_offset=file_table[fd].disk_offset;
  	size_t file_offset=file_o_table[index].fd_offset;
  	
  	if(file_offset>file_size){
  		Log("offset is out of the bound");
  		return 0;//即不进行读操作
  	}
  	else{
		size_t w_len=(file_offset+len<=file_size)?len:(file_size-file_offset);
	  		
	  	file_o_table[index].fd_offset+=w_len;
	  	
	  	
	  		
	  	ramdisk_write(buf,disk_offset+file_offset,w_len);
	  		
	  	return w_len;
  	}
  	
	
}

size_t fs_lseek(int fd,size_t offset,int whence){

	//同fs_open忽略了对stdin,stdout,stderr的读操作
	//whence from fs.h
	if(fd>=3){
		int index=get_index(fd);
		if(index==-1){
			Log("File is not opened!");
			return -1;
		}
		
		size_t offset_new=-1;
		size_t file_size=file_table[fd].size;
  		size_t file_offset=file_o_table[index].fd_offset;
  		
  		
  		switch(whence){
  			case SEEK_SET:
  				offset_new=(offset<=file_size)?offset:file_size;
  				break;
  			case SEEK_CUR:
  				offset_new=(offset+file_offset<=file_size)?offset+file_offset:file_size;
  				break;
  			case SEEK_END:
  				offset_new=(offset+file_size<=file_size)?offset+file_size:file_size;
  				break;
  			default:
  				Log("Unknown whence %d", whence);
  				return -1;
  		
  		}
  		
  		assert(offset_new>=0);
  		
  		file_o_table[index].fd_offset=offset_new;
  		
  		return offset_new;
	}
	else Log("File to be ignored:stdin, stdout or stderr!");
	return 0;
}


int fs_close(int fd){

	//同fs_open忽略了对stdin,stdout,stderr的读操作
	if(fd>=3){
		int index=get_index(fd);
		
		if(index==-1){
			Log("File is not opened!");
			return -1;
		}
		
		for(int i=index;i<file_o_total-1;i++){
			file_o_table[i]=file_o_table[i+1];
		}
		file_o_total--;
		assert(file_o_total>=0);
		return 0;
	}
	
	else Log("File to be ignored:stdin, stdout or stderr!");
	return 0;
	
}


void init_fs() {
  // TODO: initialize the size of /dev/fb
  AM_GPU_CONFIG_T my_event=io_read(AM_GPU_CONFIG);
  
  int wid=my_event.width;
  int hei=my_event.height;
  file_table[FD_FB].size=wid*hei*sizeof(uint32_t);
  
}
