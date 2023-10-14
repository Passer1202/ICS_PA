/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  int val;
  
  char expr[200];

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */



void scan_watchpoint()
{
	if(head!=NULL)
	{
		for(WP* h=head;h!=NULL;h=h->next)
		{
			bool success;
			word_t value=expr(h->expr,&success);
			if(h->val!=value)
			{
				printf("Watchpoint %d:\"%s\":oldvlaue=%u,newvalue=%u\n",h->NO,h->expr,h->val,value);
			h->val=value;
			}
         	}
         }
}

void sdb_watchpoint_display(){
	
	if(head==NULL){printf("No watchpoint\n");return;}	
	scan_watchpoint();
}

	
static void free_wp(WP*wp)
{
	WP* h=head;
	if(h == wp)head=NULL;
	else
	{
		while(h!=NULL&&h->next!=wp)h=h->next;
		assert(h!=NULL);
		h->next=wp->next;
	}
	wp->next=free_;
	free_=wp;
}

static WP* new_wp()
{
	if(free_==NULL)
	{Log("too much watchpoint");
		assert(0);//空的了
	}	  //}
	WP* p=free_;
	free_=free_->next;
	p->next=head;
	head=p;
	return p;

}

void delete_watchpoint(int no)
{
        if(no>=NR_WP)assert(0);
        WP* wp=&wp_pool[no];
        free_wp(wp);
        printf("delete watchpoint%d:%s\n",wp->NO,wp->expr);
}


void set_watchpoint(char *args,word_t ans)
{
	WP* wp=new_wp();
	strcpy(wp->expr,args);
	wp->val=ans;
	printf("Watchpoint %d:%s\n",wp->NO,wp->expr);
}
