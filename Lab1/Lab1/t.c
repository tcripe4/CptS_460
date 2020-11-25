/*******************************************************
*                  @t.c file                          *
*******************************************************/
Travis Cripe


typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

DIR *dp;
u16 ino;
u32 *up;
INODE *ip;
GD *gp;

int prints(char *s)
{
    while(*s)
      {
        putc(*s++);
      }
}

int gets(char *s)
{
    while((*s = getc()) != '\r')
      {
        putc(*s++);
      }
    *s = 0;
}

u16 NSEC = 2;

char buf1[BLK], buf2[BLK];

u16 getblk(u16 blk, char *buf)
{
    readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);

    // readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}


// got this from 360 code
u16 search(INODE *ip, char *name)
{
    char *cp, temp[256];
    for (int i = 0; i < 12; i++) {
        get_block(dev, ip->i_block[i], buf2);
        cp = buf2;
        dp = (DIR*)cp;
        //printf("  ino   rlen  nlen  name\n");

        while (cp < buf2 + BLK) {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;
            //printf("%4d  %4d  %4d    %s\n",
                //dp->inode, dp->rec_len, dp->name_len, temp);
            if (strcmp(temp, name) == 0) {
                return (u16)dp->inode - 1;
            }
            cp += dp->rec_len;
            dp = (DIR*)cp;
        }
    }
    error();
}

main()
{
    int blk;
    char *name[2];
    name[0] = "boot";
    name[1] = "mtx";
    
    getblk(2, buf1);
    gp = (GD*)buf1;
    blk = gp->bg_inode_table;
    
    get(blk,buf1);
    ip = (INODE*)buf1 + 1;
    
    for (int i = 0; i < 2; i++)
    {
        ino = search(ip, names[i]);
        getblk(blk + (ino /8), buf1);
        ip = (INODE*)buf1 + (ino % 8);
    }
    
    if ((u16)ip->i_block[12]){
        getblk((u16)ip->i_block[12], buf2);
    }

    setes(0x1000);  // MTX loading segment = 0x1000


//3. load 12 DIRECT blocks of INODE into memory
   for (i=0; i<12; i++){
      getblk((u16)ip->i_block[i], 0);
      putc('*');
      inces();
   }

//4. load INDIRECT blocks, if any, into memory
   if (ip->i_block[12]){
     up = (u32 *)buf2;      
     while(*up){
        getblk((u16)*up, 0); putc('.');
        inces();
        up++;
     }
  }
  prints("go?"); getc();
}  
