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

#include <isa.h>
#include <memory/vaddr.h>
#include <memory/paddr.h>

#define VA_OFFSET(addr) (addr & 0x00000FFF)
#define VPN_L(addr) 	((addr>>12) & 0x000003FF)
#define VPN_H(addr)	((addr>>22) & 0x000003FF)

#define PTE_V(x)	(x & 0x1)		//第1位
#define PTE_R(x)	((x>>1) & 0x1)   	//第2位
#define PTE_W(x)	((x>>2) & 0x1)		//第3位
#define PTE_X(x)	((x>>3) & 0x1)		//第4位
#define PTE_PPN(x)	((x>>12) & 0xFFFFF)	//高20位

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {

	uint32_t satp=cpu.csr.satp;
	
	vaddr_t dir_b= satp<<12 ;
	
	uint32_t va_off=VA_OFFSET(vaddr);
	uint32_t vpn_l=VPN_L(vaddr);
	uint32_t vpn_h=VPN_H(vaddr);
	
	vaddr_t dir_t=dir_b + vpn_h*4;
	
	word_t tx_d=paddr_read(dir_t,4);
	
	assert(PTE_V(tx_d)!=0);
	
	vaddr_t table_b=PTE_PPN(tx_d);
	table_b<<=12;
	
	vaddr_t table_t=table_b+vpn_l*4;
	
	word_t tx_t=paddr_read(table_t,4);
	
	assert(PTE_V(tx_t)!=0);
	
	switch(type){
		case MEM_TYPE_IFETCH:assert(PTE_X(tx_t)!=0);break;
		case MEM_TYPE_READ:assert(PTE_R(tx_t)!=0);break;
		case MEM_TYPE_WRITE: assert(PTE_W(tx_t)!=0);break;
		default:assert(0);
	}
	
	paddr_t ppn=PTE_PPN(tx_t);
	ppn<<=12;
	
	paddr_t paddr=ppn|va_off;

	assert(paddr==vaddr);
  	
  	return paddr;
}
