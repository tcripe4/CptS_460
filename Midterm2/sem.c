#define BUFSIZE 8

char buf[BUFSIZE];
int head, tail;
SEMAPHORE empty, full, pmutex, cmutex;


void sem_init()
{
    head = 0;
    tail = 0;
    empty.value = BUFSIZE;
    full.value = 0;
    pmutex.value = 1;
    cmutex.value = 1;
}

int P(SEMAPHORE *s)
{
    int SR = int_off();
    s->value--;
    if (s->value < 0)
    {
        block(s);
    }
    s->spinlock = 0;
    int_on(SR);
}

int V(SEMAPHORE *s)
{
    int SR = int_off();
    s->value++;
    if (s->value <= 0)
    {
        signal(s);
    }
    s->spinlock = 0;
    int_on(SR);
}

int signal(SEMAPHORE *s)
{
    PROC *proc = dequeue(&s->queue);
    proc->status = READY;
    enqueue(&readyQueue, proc);
}

int block(SEMAPHORE *s)
{
    running->status = BLOCK;
    enqueue(&s->queue, running);
    tswitch();
}

int Producer()
{
    char line[64], pro_item;
    int i = 0;
    kprintf("Enter item to produce: ");
    kgets(line);
    kprintf("\n");
    
    while(1)
    {
        pro_item = line[i++];
        P(&empty);
        P(&pmutex);
        buf[head++] = pro_item;
        head %= BUFSIZE;
        V(&pmutex);
        V(&full);
    }
    kprintf("\n");
}

int Consumer()
{
    int consume = 0;
    char item;
    kprintf("TEST FOR CONSUME: ");
    while(1)
    {
        kprintf("CONSUME TEST: ");
        P(&full);
        P(&cmutex);
        item = buf[tail++];
        kprintf("%c \n", item);
        //kputc(item);
        tail %= BUFSIZE;
        V(&cmutex);
        V(&empty);
    }
}
