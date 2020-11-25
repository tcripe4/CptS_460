#include "keymap2"

#define LSHIFT 0x12
#define RSHIFT 0x59
#define ENTER  0x5A
#define LCTRL  0x14
#define RCTRL  0xE014

#define ESC    0x76
#define F1     0x05
#define F2     0x06
#define F3     0x04
#define F4     0x0C

#define KCNTL 0x00
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK  0x0C
#define KISTA 0x10

typedef volatile struct kbd{
  char *base;
  char buf[128];
  int head, tail, data, room;
}KBD;

volatile KBD kbd;

#define KSTAT 0x04
#define KDATA 0x08

int lshift;
int lcontrol;

int kbd_init()
{
  char scode;
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x10; // bit4=Enable bit0=INT on
  *(kp->base + KCLK)  = 8;
  kp->head = kp->tail = 0;
  kp->data = 0; kp->room = 128;
    
    lshift = 0;
    lcontrol = 0;
}

// kbd_handler1() for scan code set 1
void kbd_handler()
{
  u8 scode, c;
  KBD *kp = &kbd;
  scode = *(kp->base + KDATA);

  printf("scan code = %x ", scode);
  
  if (scode & 0x80)
  {
      if (scode == LSHIFT || scode == RSHIFT)
      {
          if (lshift == 0)
          {
              lshift = 1;
          }
          else
          {
              lshift = 0;
          }
      }
      if (scode == LCTRL)
      {
          lcontrol = 0;
      }
     return;
  }
    
    
  if (scode == LSHIFT || scode == RSHIFT)
  {
      if (lshift == 0)
      {
          lshift = 1;
      }
      else
      {
          lshift = 0;
      }
      return;
  }
    
    if (scode == LCTRL)
    {
        lcontrol = 1;
        return;
    }
    
    if (lshift)
    {
        c = utab[scode];
    }
    else
    {
        c = ltab[scode];
    }
    
    if (lcontrol && ltab[scode] == 'c')
    {
        printf("Control-c Key\n");
        c = '\n';
    }
    
    if (lcontrol && ltab[scode] == 'd')
    {
        c = 0x4;
    }
    
    
  //printf("kbd interrupt: c=%x %c\n", c, c);

  kp->buf[kp->head++] = c;
  kp->head %= 128;
  kp->data++; kp->room--;
}

int kgetc()
{
  char c;
  KBD *kp = &kbd;

  unlock();
  while(kp->data == 0);

  lock();
  c = kp->buf[kp->tail++];
  kp->tail %= 128;
  kp->data--; kp->room++;
  unlock();
  return c;
}

