/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
 
#define DR   0x00  // data register: for read/write chars
#define FR   0x18  // flag register: TxEmpty, RxFull, etc
 
#define RXFE 0x10  // receiving
#define TXFF 0x20  // transmitting
#define TXFE 0x80
 
typedef volatile struct uart
{
    char *base; // base address; as char *
    int n;      // uart number 0-3
}UART;
 
UART uart[4];

char *ctable = "0123456789ABCDEF";
 
// UART inititalization function
int uart_init()
{
    int i; UART *up;
 
    for (i = 0; i < 4; i++) //uart0 to uart2 are adjacent
    {
        up = &uart[i];
        up->base = (char *)(0x101F1000 + i * 0x1000);
        up->n = i;
    }
    //uart[3].base = (char *)(0x10009000); // uart3 at 0x10009000
}
 
// It loops until the UART flag register is no longer RXFE, indicating there is a
// char in the data register. Then it reads the data register, which clears the RXFE bit and sets the RXFE
// bit in FR, and returns the char
int ugetc(UART *up)
{
    while (*(up->base + FR) & RXFE); // loop if UFR is REFE
    return *(up->base + DR);         // return a char in UDR
}
 
int uputc(UART *up, char c)
{
    while (*(up->base + FR) & TXFF); // loop if UFR is TXFF
    *(up->base + DR) = c;            // write char to data register
}
 
int ugets(UART *up, char *s)
{
    while ((*s = (char)ugetc(up)) != '\r')
    {
        uputc(up, *s);
        s++;
    }
    *s = 0;
}
 
int uprints(UART *up, char *s)
{
    while (*s)
    {
        uputc(up, *s++);
    }
}

int ugetx(UART *up, int x)
{
    char c;
    if (x)
    {
        c = ctable[x % 16];
        ugetx(up, x / 16);
    }
    uputc(up, c);
}

int uprintx(UART *up, int x)
{
    uputc(up, '0');
    uputc(up, 'x');
    if (x == 0)
    {
        uputc(up, '0');
    }
    else
    {
        ugetx(up, x);
    }
    uputc(up, ' ');
}

int ugetu(UART *up, int x)
{
    char c;
    if (x)
    {
        c = ctable[x % 10];
        ugetu(up, x / 10);
    }
    uputc(up, c);
}

int uprintu(UART *up, int x)
{
    if (x == 0)
    {
        uputc(up, '0');
    }
    else
    {
        ugetu(up, x);
    }
    uputc(up, ' ');
}

int uprinti(UART *up, int x)
{
    if (x < 0)
    {
        uputc(up, '-');
        x = -x;
    }
    uprintu(up, x);
}
 
ufprintf(UART *up, char *fmt, ...)
{
    int *ip;
    char *cp;
    cp = fmt;
    ip = (int *)&fmt + 1;
 
    while (*cp)
    {
        if (*cp != '%')
        {
            uputc(up, *cp);
            if (*cp == '\n')
                uputc(up, '\r');
            cp++;
            continue;
        }
        cp++;
        switch (*cp)
        {
            case 'c': uputc(up, (char)*ip); break;
            case 's': uprints(up, (char *)*ip); break;
            case 'd': uprinti(up, *ip); break;
            case 'u': uprintu(up, *ip); break;
            case 'x': uprintx(up, *ip); break;
        }
        cp++;
        ip++;
    }
}
