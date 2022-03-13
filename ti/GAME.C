#include "dsk1.grf1rf"
#include "dsk1.earefs/c"
#asm
         COPY "DSK2.ACESHAPES"
#endasm
 
/*Super Sled Acer, V1.0 */
 
 
int flag,temp,jump;
int k,s,a,x,y;
int hell=1,speed,sp2;
int mp1,mp2,mp3,md1,md2,md3,mv1,mv2,mv3;
char pname[20];
int dist,r,c;
int et[15],lev,lives;
int fail,mc,jnum;
 
#include "dsk1.random;c"
#include "dsk1.cutils/c"
 
init()
{
screen(16);
chrdef(97,"191c4f1fbe2daf2f0000c0e030b1fffeaf37b91e5f1f4f0f");
chrdef(100,"fff18000e0f8fcfe2f0f27071707270fffffffffffffffff");
chrdef(103,"0f0f1f1f1f3f3f3efefcf0c0801c1e0e3e3e1e1f1f0f0703");
chrdef(106,"0e0f0f1ffffefcf8");
chrdef(107,"cc33cc33cc33cc33");
chrdef(108,"bafebafeba1010104488112244881122002040fe40200000");
#asm
       LI R0,2944
       LI R1,SLEDDN
       LI R2,448
       BLWP @VMBW
#endasm
}
 
title()
{ grf1();
  say(10,10,"ab");
  say(11,10,"cd SEAHORSE");
  say(12,10,"ef SOFTWARE");
  say(13,10,"gh PRESENTS");
  say(14,10,"ij");
  for (a=0;a<25000;a++);
  ldpic("SLEDACER");
  k=0;
  while (k!=18)
   { k=key(1,&s);
     if (k!=18) k=key(2,&s); }
  grf1();
  init();
}
 
center(r,x) int r; char x[];
{ int a;
  a=0;
  while (x[a++]!=0);
  a=a-2;
  say(r,16-a/2,x);
}
 
main()
{ intpic();
  init();
  title();
while (hell)
{ init();
  start();
  game();
}}
 
start()
{ grf1();
  say(1,1,"WELCOME, SUPER SLED ACER");
  say(2,1,"YOUR MISSION IS TO MAKE IT");
  say(3,1,"TO THE BOTTOM OF THE HILL");
  say(5,1,"BEWARE OF ALL OBSTACLES, AND");
  say(6,1,"SEE YOU AT THE BOTTOM!");
  say(12,1,"(---IF YOU'RE LUCKY!---)");
  say(20,1,"PRESS FIRE BUTTON TO BEGIN");
  k=0; jnum=1;
  while(k!=18)
  { jnum=1; k=key(jnum,&s);
    if (k!=18) {jnum=2; k=key(jnum,&s); }
  }
  grf1();
  spmag(3);
  sprite(1,112,2,100,128);
  for (x=2; x<15; x++)
    et[x]=0;
}
 
sndon()
{ #asm
       LI R0,>E400
       MOVB R0,@>8400
       LI R0,>FA00
       MOVB R0,@>8400
#endasm
}
 
crsnd()
{#asm
       LI R0,>E600
       MOVB R0,@>8400
#endasm
}
 
sndoff()
{#asm
       LI R0,>FF00
       MOVB R0,@>8400
#endasm
}
 
game()
{ dist=10000;
  speed=0;
  sp2=0;
  lev=1;
  lives=3;
  temp=0;
  jump=0;
  fail=0;
  if ((k=key(0,&s))==65) dist=100;
stlev();
while (dist-->0)
{ if (jump==0)
  ctrl();
  enemy();
  if (jump==0)
    check();
  else {sndoff(); temp=0; jump--;}
  pause(1);
}
if (dist==-2)
       gamovr();
else   gamwin();
}
 
ctrl()
{ a=joyst(jnum,&x,&y);
  spposn(1,&r,&c);
  if (x==-4)
  {    sppat(1,116);
       if (c>speed+1) c=c-speed;
  }
  if (x==4)
  {    sppat(1,120);
       if (c<254-speed) c=c+speed;
  }
  if (x==0)
       sppat(1,112);
  if (speed<8) {sp2++; if (sp2==10) {speed++; sp2=0;}}
  k=key(jnum,&s);
  if (fail==0)
  if (k==18)
  {    sp2=sp2-2;
       if (sp2==6) temp++;
       if (sp2<1)
       { speed--; sp2=8; }
       if (speed<5) speed=5;
       if (temp==8) {say(2,11,"FAILURE!"); fail=100;}
  }
  else
  { if (sp2==8) temp--;
    if (sp2==4) temp--;
  }
  if (fail!=0) {fail--; if (fail==0) temp=0;}
  if (temp!=8) {hchar(2,11,32,8); if (temp>0) hchar(2,11,109,temp);}
 
  if (jump==0)
    sndon();
  else sndoff();
  sploct(1,r,c);
}
 
pause(t) int t;
{for (y=0; y<t; y++)
  for (x=0; x<125; x++); }
 
enemy()
{
if (rnd(5)>2) enout();
for (a=2;a<15;a++)
{ spposn(a,&r,&c);
if (et[a]!=0)
{ r=r-speed;
  if (et[a]==1) c=c+4;
  if (et[a]==4) c=c+sgn(128-c);
  if (et[a]==5) r=r-6;
  if (et[a]==6) r=r-1;
  if (et[a]==7) r=r-3;
  if (et[a]==9) c=c-6;
  if (et[a]==10) r=r+12;
  flag=1;
  if (r<0) {spdel(a); flag=0;}
  if (r>192){spdel(a); flag=0;}
  if (c<0){spdel(a); flag=0;}
  if (c>255){spdel(a); flag=0;}
  if (flag) sploct(a,r,c);
  if (flag==0) et[a]=0;
}}
}
 
sgn(x) int x;
{ int y;
  y=0;
  if (x>0) y=1;
  if (x<0) y=-1;
  return y;
}
 
enout()
{ x=1;
  while (et[++x]!=0);
  if (x<15)
  { y=rnd(10)+1;
    switch(y) {
 
    case 1: if (dist<8600)
           {sprite(x,124,2,190,1);
            et[x]=y;}
            break;
    case 2: sprite(x,128,13,190,rnd(256));
            et[x]=y;
            break;
    case 3: if (dist<9300) {sprite(x,132,2,190,rnd(256));
            et[x]=y;}
            break;
    case 4: if (dist<7700)
            { sprite(x,140,2,190,rnd(256));
              et[x]=y; }
            break;
    case 5: if (dist<1700)
            { sprite(x,144,5,190,rnd(256));
              et[x]=y; }
            break;
    case 6: if (dist<4400)
            { sprite(x,148,2,190,rnd(256));
              et[x]=y; }
            break;
    case 7: s=1;
            while (et[++s]!=6);
            if (s<15)
            { spposn(s,&r,&c);
              sprite(x,152,2,r-3,c);
              et[x]=y;
            }
            break;
    case 8: if (dist<6700)
            { sprite(x,156,2,190,rnd(256));
              et[x]=y; }
    case 9: if (dist<5600)
            { sprite(x,160,2,190,255);
              et[x]=y; }
    case 10:if (dist<3100)
            { sprite(x,164,2,1,rnd(256));
              et[x]=y; }
}}}
 
check()
{
if (dist==9300) midlev();
if (dist==8600) midlev();
if (dist==7700) midlev();
if (dist==6700) midlev();
if (dist==5600) midlev();
if (dist==4400) midlev();
if (dist==3100) midlev();
if (dist==1700) midlev();
flag=0;
if (spcall())
{ for (a=2; a<15; a++)
  { if (et[a]!=0)
     if (spcnc(1,a,13)) flag=a;
  }
}
if (flag)
 if (et[flag]!=8)
{ crash();
  lives--;
  if (lives) stlev();
        else dist=-1;
}
 else jump=15;
}
 
gamovr()
{ say(12,12,"GAME OVER");
  pause(250);
}
 
gamwin()
{ sndoff();
  ldpic("SSAEN");
  delay(600);
  grf1();
  intmus();
  center(12,"SUPER SLED ACER");
  muspas();
  grf1();
  center(12,"WRITTEN BY MIKE WARD");
  muspas();
  grf1();
  center(12,"IDEA MIKE WARD AND");
  center(13,"GORDON HADDRELL");
  muspas();
  grf1();
  center(12,"MUSIC");
  center(14,"STILL HERE");
  center(16,"FROM ATARI ST 'STOS'");
  muspas();
  grf1();
  center(12,"(C)1991 SEAHORSE SOFTWARE");
  center(16,"CONGRADULATIONS");
  center(22,"(PRESS FIRE TO START OVER)");
  k=0;
  while((k=key(jnum,&s))!=18)
    music();
  #asm
       LI R0,>9F00
       MOVB R0,@>8400
       LI R0,>BF00
       MOVB R0,@>8400
       LI R0,>DF00
       MOVB R0,@>8400
#endasm
}
 
delay(x) int x;
{ int y,z;
  for (y=0; y<x; y++)
    for (z=0; z<100; z++);
}
 
stlev()
{ for (a=2; a<15; a++)
  { spdel(a);
    et[a]=0; }
  clear();
  sprite(1,112,2,100,128);
  say(1,11,"MISSION");
  hchar(1,19,lev+48,1);
  say(3,7,"WATCH FOR");
  switch(lev) {
    case 1 :say(3,17,"TREES");
            break;
    case 2 :say(3,17,"ROCKS");
            break;
    case 3 :say(3,17,"SKIERS");
            break;
    case 4 :say(3,17,"SNOWMEN");
            break;
    case 5 :say(3,17,"JUMPS");
            break;
    case 6 :say(3,17,"SNOWMOBILES");
            break;
    case 7 :say(3,17,"TANKS");
            break;
    case 8 :say(3,17,"AVALANCHES");
            break;
    case 9 :say(3,17,"MISSILES");
            break;
  }
  say(5,6,"PRESS FIRE BUTTON");
  k=0;
  while ((k=key(jnum,&s))!=18);
  clear();
  fail=0;
  temp=0;
  say(1,5,"LIVES:");
  hchar(1,11,108,lives);
  say(2,5,"BRAKE:");
  hchar(2,19,110,1);
  say(3,5,"LEVEL:");
  hchar(3,11,lev+48,1);
  sp2=0; speed=0;
}
 
endlev()
{ int cnt;
  speed=8;
  for (cnt=24; cnt>18; cnt--)
  { if (cnt!=24) hchar(cnt+1,1,32,32);
    hchar(cnt,1,107,32);
    if (lev==9) say(cnt,13,"FINISH");
    enemy();
    sndon();
    pause(2);
  }
  spposn(1,&r,&c);
  while (r<192)
  { r=r+8;
    sploct(1,r,c);
    pause(2);
  }
  lev++;
  while ((k=key(jnum,&s))==18);
}
 
midlev()
{ endlev();
  stlev();
}
 
crash()
{ sndoff();
  spposn(1,&r,&c);
  sprite(16,136,2,r,c);
  for (a=1; a<6; a++)
  { r=r+speed;
    sploct(16,r,c);
    pause(3);
  }
  crsnd();
  pause(4);
  sndoff();
  pause(6);
  spdel(16);
}
 
#include "dsk2.musics"
 
