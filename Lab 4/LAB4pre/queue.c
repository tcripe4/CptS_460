/************************  queue.c file ************************
1. Write your C code for the functions specified below
2. Compile queue.c into .o file by
      arm-none-eabi-gcc -c -mcpu=arm926ej-s queue.c -o queue.o
3. Use it in the LINK step by
      arm-none-eabi-ld -T t.ld ts.o t.o   queue.o   -o t.elf
****************************************************************/

#include "type.h"
extern PROC *freeList, *readyQueue, *running;

// write YOUR C cdoe for the following functions

PROC *get_proc(PROC **list)  // called as get_proc(&freeList)
{
  // return a free PROC from freeList
    PROC *p = *list;
    *list = p->next;
    return p;
}
   
int put_proc(PROC **list, PROC *p)   // called as put_proc(&feeList, p)
{
  // enter PROC p to end of freeList
    p->next = *list;
    *list = p;
}

int enqueue(PROC **queue, PROC *p)   // called as enqueue(&readyQueue, p)
{
    PROC *x = *queue;
    if (x == 0 || p->priority > x->priority)
    {
        *queue = p;
        p->next = x;
    }
    else
    {
        while (x->next && p->priority <= x->next->priority)
        {
            x = x->next;
        }
        p->next = x->next;
        x->next = p;
    }
}

PROC *dequeue(PROC **queue)         // called as dequeue(&readyQueue)
{
    PROC *p = *queue;
    if (p)
    {
        *queue = (*queue)->next;
    }
    return p;
}

int printList(char *listname, PROC *list)
{
    printf("%s = ", listname);
    PROC *p = list;
    while (p)
    {
        printf("[%d, %d]", p->pid, p->priority);
        p = p->next;
    }
    printf("NULL\n");
}

