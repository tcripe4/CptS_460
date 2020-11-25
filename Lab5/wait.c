
int ksleep(int event)
{
	int SR = int_off();
	running->event = event;
	running->status = SLEEP;	
	tswitch();					// switch process
	int_on(SR);					// restore original CPSR
}

// wakes up ALL processes, if any, that are sleeping on an event
int kwakeup(int event)
{

	int i; PROC *p;
	int SR = int_off();
	for (i = 1; i < NPROC; i++) // not applicable to P0
	{
		p = &proc[i];
		if (p->status == SLEEP && p->event == event)
		{
			p->event = 0;				// cancel PROC's event
			p->status = READY;			// change status to READY
			enqueue(&readyQueue, p);	// enter p into readyQueue
		}
	}
	int_on(SR);
}

// ch 4.15 in Design and Implementation
int kexit(int exitValue)
{
	int i, wakeupP1 = 0;
	PROC *p;
	if (running->pid == 1)
	{ // nproc = number of active PROCs
		printf("other procs still exist, P1 can't die yet\n");
		return -1;
	}
	/* send children (dead or alive) to P1's orphanage */
	for (i = 1; i < NPROC; i++)
	{
		p = &proc[i];
		if (p->status != FREE && p->ppid == running->pid)
		{
			p->ppid = 1;
			p->parent = &proc[1];
			wakeupP1++;
		}
	}
	// record exitvalue and become a zombie
	running->exitCode = exitValue;
	running->status = ZOMBIE;
	// wakeup parent and also P1 if necessary
	kwakeup(running->parent);
	if (wakeupP1)
	{
		kwakeup(&proc[1]);
	}
	tswitch();
}

// wait for ZOMBIE child
// ch 4.15 in Design and Implementation
int kwait(int *status)
{
    PROC *p; int i, hasChild = 0;
    while (1)								// search PROCs for a child
    {
        for (i = 1; i < NPROC; i++)			// exclude P0
        {
            p = &proc[i];
            if (p->status != FREE && p->ppid == running->pid)
            {
                hasChild = 1;				// has child flag
                if (p->status == ZOMBIE)	// lay child to rest
                {
                    *status = p->exitCode;	// collect its exit code
                    p->status = FREE;		// free its proc
                    put_proc(&freeList, p); // to freelist
                    return(p->pid);			// return its pid
                }
            }
        }
        if (!hasChild)
        {
            return -1;	// no child, return error
        }
        ksleep(running); // still has kids alive: sleep on PROC address
    }
}
