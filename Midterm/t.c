#include "type.h"
#include "string.c"

int color;

PROC proc[NPROC], *running, *freeList, *readyQueue;
int procsize = sizeof(PROC);
#include "kernel.c"
#include "wait.c"
#include "vid.c"
#include "kbd.c"
#include "exceptions.c"
//#include "timer.c"

void timer_handler(int);
void timer_init();

//int kprintf(char *fmt, ...);

void copy_vectors(void) 
{
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

int kprintf(char *fmt, ...);
void timer_handler();

void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;
    //kprintf("IRQ HANDLER CHECK\n");
    // read VIC status register to find out which interrupt
    vicstatus = VIC_STATUS; // VIC_STATUS=0x10140000=status reg
    sicstatus = SIC_STATUS;
    if (vicstatus & 0x10)
       {
          if (*(timer[0].base+TVALUE) == 0)
          {
             timer_handler(0);
          }
       }
    //kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);
    if (vicstatus & 0x80000000){
       if (sicstatus & 0x08){
          kbd_handler();
       }
    }
}

int body();

int main()
{
	color = WHITE;
	fbuf_init();
	kbd_init();

	// enable VIC for IRQ 31
    VIC_INTENABLE |= (1 << 4);
    VIC_INTENABLE |= (1 << 5);
	VIC_INTENABLE |= (1 << 31); // SIC to VIC's IRQ31
	// enable KBD IRQ 
	SIC_ENSET = 1 << 3;  // KBD int=3 on SIC
	SIC_PICENSET = 1 << 3;  // KBD int=3 on SIC

	kprintf("Welcome to WANIX in Arm\n");
    
    timer_init();
    timer_start(0);
    
	init();
    
    kprintf("P0 kfork 4 processess\n");
    kfork((int)body, 1);
    kfork((int)body, 1);
    kfork((int)body, 1);
    kfork((int)body, 1);

    //unlock();
	while (1)
		if (readyQueue)
			tswitch();
}
