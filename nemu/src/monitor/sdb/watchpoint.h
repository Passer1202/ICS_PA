

#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>

#define NR_WP 32
typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  int val;

  char expr[200];

} WP;
 WP wp_pool[NR_WP];
 WP *head ;
 WP *free_;

void init_wp_pool();

void sdb_watchpoint_display();

void scan_watchpoint();

void free_wp(WP* wp);

WP* new_wp();

void delete_watchpoint(int no);

void set_watchpoint(char *args,word_t ans);
#endif
