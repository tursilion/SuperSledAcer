/* Super Sled Acer - Yet another silly SSA game! */

/* modified version of put routines */
#include "slmikes.c"

/* externs for PCC's _doint function */

extern unsigned _rax, _rbx, _rcx, _rdx, _rsi, _rdi, _res, _rds;
extern char _carryf, _zerof;

/* buffer space for shapes */

char sled[810],
     enemy[7560],
     man[540],
     blank[270];

/* constants */
int tankx[8]={ 0,1,1,1,0,-1,-1,-1 };
int tanky[8]={ -1,-1,0,1,1,1,0,-1 };

#define AHEAD 0
#define LEFTS 270
#define RIGHTS 540
#define STEP 270
#define SMALL_STEP 66
#define MID_STEP 130
#define SHOT 738
#define NUMEN 10

/* variables */

int GAMEON,CGA;
unsigned FIRE,LEFT,RIGHT;
unsigned MAXx,MINx,SCALE;
unsigned LTHRES,RTHRES;
int JOYCAL,KEYMODE;
char LKEY,RKEY,SKEY;
char gp[768];      /* game palette - we get this from the grid .BMP */
int x,y,dist,slspeed,anim;
int level,lives,score,brake,jump;
int ex[NUMEN],ey[NUMEN],et[NUMEN],exd[NUMEN],eyd[NUMEN];
int sizex[NUMEN],sizey[NUMEN];
int time,perfecttime,diff,cheat;
int brakefail,Perfect;

main(argc,argv) int argc; char *argv[];
{ /* main function */
if (strcmp(argv[1],"-greet")==0)
  greet();

mike_init();

CGA=0;
if ((*argv[1]=='C')||(*argv[1]=='c'))
{ CGA=1;
  puts("\nCGA MODE!\n\n");
}

if (CGA)
  set_cga();
else
  set_vga();

readdat();
JOYCAL=0;
KEYMODE=0;
MAXx=130;
MINx=126;
LTHRES=127;
RTHRES=129;
SCALE=1;
LKEY='S';
RKEY='D';
SKEY=' ';
diff=2;
score=0;
GAMEON=1;
while(GAMEON)
{ if (CGA)
    set_cga();
  else
    set_vga();

  load_bmp("sledacer.bmp");
  fade_in(5);

  if (score!=0)
  { vprint(232,192,255|0x100,"Score:");
    numprint(280,192,255,score);
  }

  waitkey();

  if (GAMEON)
  { fade_out(3);

    options();

    if (GAMEON)
      game();
  }
}
sound_off();
set_txt();
printf("\nSuper Sled Acer - (c)1991-1999 by M.Brent, [P]jan99\n\n");
printf("*** SHAREWARE BEG ***\n");
printf("If'n y'all likes this proggie, send a dollar or so of yer local\n");
printf("currency my way, will'ya? If that's too cheap, I 'spose I could be\n");
printf("persuaded to accept more. If that's too rich, send me something\n");
printf("else. I like dolphins. You can get on my good side sending me something\n");
printf("dolphinish. Or send me something neat. Or weird. Or wacky. Send a picture,\n");
printf("a pebble, a piece of a spy satelitte, your car, your cat (especially if\n");
printf("it's a tiger or a lion), your autographed Babe Ruth rookie card, your\n");
printf("non-autographed photo of your best friend, anything to do with Weird Al,\n");
printf("Lion King, video games, electronics. Or just send me a note, via\n");
printf("Email, snail-mail, or fax. And visit my web page. And bug me frequently\n");
printf("to update it. Thanks.\n\n");
printf("M.Brent (Tursi)\n");
printf("Email: tursi@yiffco.com\n");
printf("WWW: http://tursi.yiffco.com\n");
}

input()
{ /* read the joystick/keyboard and return LEFT,RIGHT (1-8) and FIRE */
int x;
char c;

if (JOYCAL)
  LEFT=RIGHT=0;     /* reset values if joystick */

FIRE=0;

c=scr_csts();
if (c)
{ if ((c>='a')&&(c<='z'))
    c=c-32;  /* make uppercase */

  if (c==27)
    GAMEON=0;  /* end game on ESC */

  if (c==LKEY)
  { if (KEYMODE)
    { if (LEFT<8)
        LEFT++;
    }
    else
      LEFT=8;
    RIGHT=0;
  }
  if (c==RKEY)
  { if (KEYMODE)
    { if (RIGHT<8)
        RIGHT++;
    }
    else
      RIGHT=8;
    LEFT=0;
  }
  if (c==SKEY)
  { LEFT=0;
    RIGHT=0;
  }
}

cheat=0;

/* use BIOS to read keyboard flags and check for shift keys */
_rax=0x0200;  /* routine 02 */
_doint(0x16);  /* keyboard BIOS int 16h */
if (_rax&0x03) /* either shift key */
  FIRE=1;

if ((_rax&0x03)==0x03)
{ cheat=1;  /* both shift keys at once activates cheat */
  FIRE=0; /* if you want to slow down, don't cheat :) */
}

if (JOYCAL)
{ /* read joystick if it was used */
  x=stickx();
  /* now figure out the return values */
  if (x<LTHRES)
  { LEFT=(LTHRES-x)/SCALE;   /* result should be 0-8 */
    if (LEFT>8) LEFT=8;
  }
  if (x>RTHRES)
  { RIGHT=(x-RTHRES)/SCALE;  /* same here */
    if (RIGHT>8) RIGHT=8;
  }

  if (joybut())
    FIRE=1;        /* one of the 4 buttons was pressed */
}
/* done */
}

waitkey()
{ /* wait for space or fire button to be pressed - this does joycal, too */
int wait;
char c;

wait=1;
while (wait)
{ c=scr_csts();
  if (c==' ')
    wait=0;
  if (c==27)
  { wait=0;
    GAMEON=0;
  }

  /* check joystick button */
  if (joybut())  
  { wait=0;        /* one of the 4 buttons was pressed */
    if (JOYCAL==0)
      calibrate();
  }
}
}

calibrate()
{ /* calibrate the joystick */
int x,y,z,z2,div;

JOYCAL=1;
set_txt();
scr_cursoff();

puts("\n\nFirst we must calibrate your joystick. This won't hurt.. much..\n");

while (joybut());
/* wait for button release */

puts("\nJust swirl your joystick in a full circle several times, ensuring\n");
puts("that you reach it's entire maximum range. Press a button when done.\n");
puts("\n(Press <ESC> to abort this if you have no joystick!)\n");
MAXx=0; MINx=9999;
FIRE=0; div=2;
while (FIRE==0)
{ x=stickx();
  if (x>MAXx)
    MAXx=x;
  if (x<MINx)
    MINx=x;

  while((MAXx-MINx)/div>60)
    div=div*2;

  scr_rowcol(12,1);
  scr_clrl();

  if (MINx!=9999)
  { z2=40-((mid(MINx,MAXx)-MINx)/div);
    scr_rowcol(12,z2);
    putchar('<');
  }
  else
  { z2=40;
    scr_rowcol(12,40);
    putchar('<');
  }

  if (MAXx!=0)
  { scr_rowcol(12,41+((MAXx-mid(MINx,MAXx))/div));
    putchar('>');
  }

  scr_rowcol(12,z2+((x-MINx)/div));
  putchar('*');

  /* check joystick button */
  if (joybut())   
    FIRE=1;

  /* emergency escape */
  if (scr_csts()==27)
    FIRE=2;
}
if (FIRE==1)
{ y=MAXx-MINx;   /* get the range */
  x=y/3;         /* three parts... left, right and centered */
  LTHRES=((y/2)+MINx)-(x/2); /* center minus 1/2 one area */
  RTHRES=((y/2)+MINx)+(x/2); /* center plus 1/2 one area */
  SCALE=x/8;               /* 8 positions in one section */
  if (SCALE==0) SCALE=1;  /* just in case it's too low */
}
}

int mid(x,y) int x,y;
{ /* return the midrange of x->y */
int z;

z=y-x;
z=z/2;
z=z+x;
return(z);
}

int stickx()
{ /* read the joystick x position myself */
/* using inline assembly!! */
#asm
  cli

  mov ah,1
  xor al,al
  xor cx,cx
  mov dx,513
  out dx,al
discharge:
  in al,dx
  test al,ah
  loopne discharge

  sti
  xor ax,ax
  sub ax,cx
#
/* routine from book 'Tricks of the Game Programming Gurus' */
}

int joybut()
{ /* directly check the buttons */
int x;

_outb(0x201,0);   /* clear the latch */
if ((_inb(0x201)&0xf0)!=0xf0)
  x=1;
else
  x=0;
return(x);
}

readdat()
{ /* read the character data into the arrays */
int i;

load_bmp("sledgrid.bmp");
get(&sled[AHEAD],0,0,16,16);
get(&sled[LEFTS],16,0,16,16);
get(&sled[RIGHTS],32,0,16,16);
get(&enemy[0],48,0,16,16);
get(&enemy[270],64,0,16,16);
get(&enemy[540],80,0,8,8);
get(&enemy[606],88,0,8,8);
get(&enemy[672],80,8,8,8);
get(&enemy[SHOT],88,8,8,8);
get(&enemy[810],96,0,8,16);
get(&enemy[940],96,8,8,16);

for (i=0; i<4; i++)
  get(&enemy[1080+(STEP*i)],112+(16*i),0,16,16);

for (i=0; i<9; i++)
{ get(&enemy[2160+(540*i)],176+(16*i),0,16,16);
  get(&enemy[2430+(540*i)],176+(16*i),16,16,16);
}

get(&enemy[7020],32,16,16,16);
get(&enemy[7290],48,16,16,16);

get(&man[0],0,16,8,8);
get(&man[66],8,16,8,8);
get(&man[132],16,16,8,8);
get(&man[198],24,16,8,8);
get(&man[264],0,24,8,8);
get(&man[330],8,24,8,8);
get(&man[396],16,24,8,8);
get(&man[462],24,24,8,8);

for (i=0; i<768; i++)
  gp[i]=tp[i];        /* save palette */
}

greet()
{ /* do the traditional greet screen */
printf("\n\n\nHere we go again!\n");
printf("Special greetings go out to:\n\n");
printf("Aslan the Golden Lion: who has given me love and forgiveness,\n");
printf("but is still far too busy. :/\n");
printf("Orzel the Naughty Eagle: My #1 groupie, silly bird, confident, and friend.\n");
printf("Peggy Hanks the Person: :) Who sent me my first fan-mail :) :)\n");
printf("Mind Grind 'Cyber-Cafe': great little place in Kelowna, BC, who hath\n");
printf("provided much stimulating chocolate mocha to me. :)\n");
printf("Dolfin the Dolphin: for understanding.\n");
printf("Anyone who has uploaded my stuff anywhere: Thanks! I need the exposure!\n\n");
printf("Here are go... still working... finally got back to it. :)\n");
printf("I need more inspiration. :) Well.. Super Sales Acer is, amazingly\n");
printf("enough, due to be published on two shareware CDs... one being\n");
printf("GameHead #7, the other I have no idea because they won't tell me!\n");
printf("(But they figured they could send me a copy when it's released,\n");
printf("so I guess that's cool.) Yeah... I'm asking for a donation or two\n");
printf("now, but don't take it too seriously. When the games *deserve* money,\n");
printf("I'll make sure you pay. >:) If not... pay if you want, don't if you\n");
printf("don't, but keep the game either way. :)\n");
printf("                                          Ta Ta!\n\n");
exit(0);
}

show(x,a) char *x,a;
{ /* used for the option menu, mostly :) */
int y;

y=(80-strlen(x))/2;
scr_rowcol(3,y);
scr_aputs(x,a);
scr_scdn();
delay(speed);
}

options()
{ /* do option screen! */
int x;
char c;

x=1;
while (x)
{ set_txt();
  scr_cursoff();
  scr_rowcol(1,15);
  scr_aputs("(another diversion brought to you by Mike Brent!)",9);
  scr_rowcol(0,32);
  scr_aputs("Super Sled Acer",7);

  while (joybut());

  show("(c)1991, 1996 by Mike Brent (originally on the TI-99/4A)",1);
  show("---",8);
  show("SPACE or FIRE to begin, ESC to quit",15);
  show("---",8);
  show("(R)ead instructions                                    ",11);
  show("(C)alibrate joystick                                   ",11);
  show("(D)ifficulty:                                          ",11);
  show("(M)ode (used for keyboard control only):               ",11);
  show("(Either SHIFT key to activate the brakes)",3);
  show("(K)eys: (Turn Left: x   Go Straight: x   Turn Right: x)",11);   
  show("---",8);
  show("OPTIONS",2);
  show("---",8);
  show("Overuse may cause failure!! Good Luck, Super Sled Acer!",4);
  show("positive braking system, only a simple aid to slow down!",4);
  show("braking system. -WARNING- this is not designed to have a",4);
  show("Your only aid will be your quick reflexes, and a simple",4);
  show("down Doom Mountain, avoiding all obstacles in your path.",4);
  show("Your Mission: Navigate your Super Sno-Speed Racer successfully",4);
  show("---",8);
  scr_rowcol(14,32);
  scr_co(LKEY);
  scr_rowcol(14,49);
  if (SKEY==' ')
    puts("SPC");
  else
    scr_co(SKEY);
  scr_rowcol(14,65);
  scr_co(RKEY);
  scr_rowcol(16,53);
  if (KEYMODE)
    puts("Proportional");
  else
    puts("Direct");
  scr_rowcol(17,26);
  switch (diff)
  { case 1: puts("Expert"); break;
    case 2: puts("Normal"); break;
    case 3: puts("Easier"); break;
    default:puts("??????");
  }
  x=1;
  while (x==1)
  { c=scr_csts();
    if (c==' ') x=0;
    if (c==27) x=0;
    if (joybut())
    { x=0;
      if (JOYCAL==0)
        calibrate();
    }
    if ((c>='a')&&(c<='z')) c=c-32;
    switch(c)
    { case 'C': calibrate();
                x=2;
                break;
      case 'R': readdoc();
                x=2;      /* exit this loop, but not the big one - redraw */
                break;
      case 'M': if (KEYMODE)
                  KEYMODE=0;
                else
                  KEYMODE=1;
                scr_rowcol(16,53);
                if (KEYMODE)
                  puts("Proportional");
                else
                  puts("Direct      ");
                break;
      case 'K': LKEY=blinkget(14,32);
                SKEY=blinkget(14,49);
                if (SKEY==' ')
                { scr_rowcol(14,49);
                  puts("SPC");
                }
                else
                { scr_rowcol(14,50);
                  puts("  ");
                }
                RKEY=blinkget(14,65);
                break;
      case 'D': diff--;
                if (diff<1) diff=3;
                scr_rowcol(17,26);
                switch (diff)
                { case 1: puts("Expert"); break;
                  case 2: puts("Normal"); break;
                  case 3: puts("Easier"); break;
                  default:puts("??????");
                }
                break;
    } /* case */
  } /* inner WHILE */
} /* outer WHILE */
if (c==27) GAMEON=0;
}

int blinkget(r,c) int r,c;
{ /* flash a cursor and get 1 keypress */
char x;
int i;

x=0; i=0;
while (x==0)
{ scr_rowcol(r,c);
  if (i<30)
    scr_co('*');
  else
    scr_co(' ');
  i++;
  if (i>59) i=0;
  delay(speed);
  x=scr_csts();
}
if ((x>='a')&&(x<='z'))
  x=x-32;
scr_rowcol(r,c);
scr_co(x);
return(x);
}

readdoc()
{ /* display the SLEDACER.DOC file to the screen for the user */
FILE *fp;
int i,i2;
char *fl,buf[81],c;

cls();
fp=fopen("sledacer.doc","r");
scr_rowcol(24,0);
if (fp==NULL)
{ scr_aputs("* Unable to load SLEDACER.DOC *",14);
  delay(speed*60);
}
else
{ i=-1;
  fl=1;
  buf[0]=0;
  while (fl)
  { scr_aputs(buf,7);
    i++;
    if (i==20)
    { scr_aputs("\n-- More - Space or Fire to continue, <ESC> to abort --",8);
      i2=1;
      while (joybut());
      while (i2)
      { if (joybut()) i2=0;
        c=scr_csts();
        if (c==' ') i2=0;
        if (c==27) { i2=0; fl=0; }
      }
    puts("\n\n");
    i=0;
    }
    if (fl)
      fl=fgets(buf,80,fp);
  }
  fclose(fp);
  if (c!=27)
  { scr_aputs("\n**Done** Press Space or Fire to continue",8);
    i2=1;
    while (joybut());
    while (i2)
    { if (joybut()) i2=0;
      c=scr_csts();
      if (c==' ') i2=0;
    }
  }
}
}

game()
{ /* play the game once */
int i,i2;
char xs[80];

if (CGA)
  set_cga();
else
  set_vga();

for (i=0; i<768; i++)
  tp[i]=gp[i];    /* load game palette */

fade_in(3);

level=1;
lives=3;
score=0;

while ((level>=1)&&(level<=9)&&(GAMEON))
{ /* main level loop: 0 = dead... 10 = win... */
  cls();

  if (CGA)  /* gotta fill the screen with white */
    for (i=0; i<200; i++)
      for (i2=0; i2<scrn_lines; i2++)
        _poke(0xff,line_table[i]+i2,scrn_base);

  get(blank,0,0,16,16);
  
  vprint(0,0,255,"Score:");
  numprint(56,0,255,score);
  vprint(0,8,255,"Level:");
  numprint(56,8,255,level);
  vprint(0,16,255,"Lives:");
  numprint(56,16,255,lives);
  vprint(0,24,255,"Brake:");
  for (i=0; i<20; i++)
    vprint(56+(i*8),24,255,"-");
  vprint(200,0,255,"Distance:    %");
  vprint(200,8,255,"Time:");

  Perfect=1;

  x=152; y=70;
  put(&sled[AHEAD],x,y);

  strcpy(xs,"Mission x");
  xs[8]=level+48;
  vprint(120,120,255,xs);

  for (i=0; i<NUMEN; i++)
    et[i]=-1;

  brake=0;
  brakefail=0;
  jump=0;

  while (scr_csts());  /* wait for clear keyboard/joystick */
  while (joybut());

  strcpy(xs,"Watch for ");
  switch (level)
  { case 1: strcat(xs,"trees"); break;
    case 2: strcat(xs,"rocks"); break;
    case 3: strcat(xs,"skiers"); break;
    case 4: strcat(xs,"snowmobiles"); break;
    case 5: strcat(xs,"jumps"); break;
    case 6: strcat(xs,"snowmen"); break;
    case 7: strcat(xs,"avalanches"); break;
    case 8: strcat(xs,"tanks"); break;
    case 9: strcat(xs,"missiles"); break;
  }
  vprint(160-((strlen(xs)*8)/2),128,255,xs);

  waitkey();

  for (i=0; i<320; i=i+16)
    put(blank,i,120);

  anim=0;
  dist=0;
  time=0;
  perfecttime=241+((level-1)*50);
  slspeed=0;

  while ((dist<1500+(level*400))&&(GAMEON))
  { moveenemy();
    sound_off();
    if (jump)
    { jump--;
      if (jump==0) sound_on(1000);
      score++;
      put(blank,x,y);
      if (jump%3==0)
        if (jump>24)
          y--; 
        else
          y++;
      put(&sled[AHEAD],x,y);
    }
    else
    { moveplayer();
      check();
    }
    update();
    delay(speed*diff);
  }

  if (dist>=10000)
    level=0;
  else
    if (GAMEON)
    { finlevel();
      level++;
    }
}

if (level==0)
  gameover();

if (level==10)
  gamewin();
}

moveplayer()
{ /* move the player */
int i,ox,oy,s,z;
char failstr[21],qw[3];

strcpy(failstr,"******FAILURE!******");

input();
ox=x; oy=y;

if ((FIRE==0)||(brakefail))
{ if (slspeed<8) slspeed++;
  if ((brake>0)&&(anim%10==0))
  { brake--;
    vprint(56+brake*8,24,255|0x100,"-");
  }
}
else
{ if (slspeed<3) slspeed++;
  if (slspeed>3) slspeed--;
  if ((brake<20)&&(anim%5==0))
  { brake++;
    vprint(48+brake*8,24,255|0x100,">");
  }
  if (brake==20)
  { brake=0;
    qw[1]=0;
    for (i=0; i<20; i++)
    { qw[0]=failstr[i];
      vprint(56+i*8,24,255|0x100,qw);
    }
    brakefail=120;
  }
}

if (brakefail)
{ brakefail--;
  if (brakefail<1)
  { for (i=0; i<20; i++)
      vprint(56+i*8,24,255|0x100,"-");
  }
}

s=AHEAD;

if (LEFT)
{ s=LEFTS;
  z=(LEFT-(8-slspeed));
  if (z>0)
    x=x-z;
  if (x<3) x=3;
}

if (RIGHT)
{ s=RIGHTS;
  z=(RIGHT-(8-slspeed));
  if (z>0)
    x=x+z;
  if (x>300) x=300;
}

put(blank,ox,oy);
put(&sled[s],x,y);
}

moveenemy()
{ /* move/start all enemies */
int t,i,i2,z,ox,oy,s;

t=(rand()%13)+1; /* (1-10) */
if (t<=level)
{ /* new enemy attempt */
  z=-1;
  for (i=0; i<NUMEN; i++)
    if (et[i]==-1)
      z=i;
  if (z!=-1)
  { switch (t)
    { case 1: et[z]=0; ex[z]=rand()%304; ey[z]=199;
              exd[z]=0; eyd[z]=0; break;
      case 2: et[z]=1; ex[z]=rand()%304; ey[z]=199;
              exd[z]=0; eyd[z]=0; break;
      case 3: et[z]=2; ex[z]=0; ey[z]=rand()%100+70;
              exd[z]=7; eyd[z]=4; break;
      case 4: et[z]=3; ex[z]=304; ey[z]=rand()%100+70;
              exd[z]=-(rand()%5+5); eyd[z]=0; break;
      case 5: et[z]=4; ex[z]=rand()%304; ey[z]=199;
              exd[z]=0; eyd[z]=0; break;
      case 6: et[z]=5; ex[z]=rand()%304; ey[z]=199;
              exd[z]=0; eyd[z]=0; break;
      case 7: et[z]=26; ex[z]=rand()%304; ey[z]=10;
              exd[z]=0; eyd[z]=rand()%4+9; break;
      case 8: et[z]=8; ex[z]=rand()%304; ey[z]=199;
              exd[z]=0; eyd[z]=-1; break;
      case 9: et[z]=24; ex[z]=rand()%304; ey[z]=199;
              exd[z]=0; eyd[z]=-7; break;
    }
  }
}

/* move the enemies */
for (i=0; i<NUMEN; i++)
{ if (et[i]!=-1)
  { ox=ex[i]; oy=ey[i];
    s=et[i]*STEP;
    switch (et[i])
    { case 2: if (exd[i]>7)
                exd[i]=7;
              if (exd[i]==7)
                if (anim%55==0)
                  exd[i]=5;
              if (exd[i]<7)
              { s=s+SMALL_STEP;
                if (exd[i]==6)
                  s=s+SMALL_STEP;
                exd[i]++;
              }
              break;
      case 3: if (anim%2)
                s=s+MID_STEP;
              break;
      case 5: if ((exd[i]>7)||(exd[i]<-7))
                exd[i]=0;
              if ((exd[i]==0)&&(rand()%55==3))
                exd[i]=sgn(x-ex[i]);
              if (exd[i])
              { exd[i]=exd[i]+sgn(x-ex[i]);
                s=s+STEP;
                if (anim%2)
                  s=s+STEP;
              }
              break;
      case 26:
      case 24: if (anim%2)
                 s=s+STEP;
               break;
      case 8:
      case 10:
      case 12:
      case 14:
      case 16:
      case 18:
      case 20:
      case 22: z=(et[i]-8)/2;
               if ((tankx[z]!=sgn(x-ex[i]))||(tanky[z]!=sgn(y-ey[i])))
               { /* need to re-aim tank */
                 if (anim%4==0) /* slowly */
                 { et[i]=et[i]+(2*sgn(x-ex[i]));
                   if (et[i]<8) et[i]=22;
                   if (et[i]>22) et[i]=8;
                   s=et[i]*STEP;
                 }
               }
               if (anim%2)
                 s=s+STEP;
               if (rand()%10==0)
               { /* fire */
                 z=-1;
                 for (i2=0; i2<NUMEN; i2++)
                   if ((et[i2]==-1)&&(z==-1))
                     z=i2;
                 if (z!=-1)
                 { et[z]=28; ex[z]=ex[i]; ey[z]=ey[i];
                   exd[z]=tankx[(et[i]-8)/2]*5;
                   eyd[z]=tanky[(et[i]-8)/2]*5;
                 }
               }
               break;
      case 28: s=SHOT;
               break;
    } /* case */
    ex[i]=ex[i]+exd[i];
    ey[i]=ey[i]+eyd[i]-slspeed;
    put(blank,ox,oy);
    if (offscreen(ex[i],ey[i]))
      et[i]=-1;
    else
    { put(&enemy[s],ex[i],ey[i]);
      sizex[i]=(char)enemy[s+1];
      sizey[i]=(char)enemy[s];
    }
  } /* if */
} /* for */
} /* function */

int abs(x) int x;
{ /* return absolute value of x */
if (x<0) x=-x;
return(x);
}

check()
{ /* check for collision and deal with it. Level=0 if player game over */
int i,c,x1,y1,x2,y2,r;

c=0;
for (i=0; i<NUMEN; i++)
{ if (et[i]!=-1)
  { x1=ex[i]+(sizex[i]>>1);
    y1=ey[i]+(sizey[i]>>1);
    x2=x+8;
    y2=y+8;
    r=(sizex[i]>>1);
    if ((abs(x1-x2)<=r)&&(abs(y1-y2)<=r)&&(cheat==0))
    { if (et[i]==4)
      { jump=48;
        sound_on(220);
      }
      else
        c=1;
    }
  }
}
if (c)
{ crash();
  lives--;
  Perfect=0;
  dist=dist/2;
  numprint(56,16,255,lives);
  if (lives)
  { for (i=0; i<NUMEN; i++)
      if (et[i]!=-1)
      { put(blank,ex[i],ey[i]);
        et[i]=-1;
      }
    put(blank,x,y);
    x=152;
    y=70;
    put(&sled[AHEAD],x,y);
    brake=0;
    if (brakefail)
      brakefail=1;  /* trick other routine into resetting display */
  }
  else
    dist=10000;
}
}

crash()
{ /* animate the crash */
int i,x2,y2,a,d;

x2=x+4;
y2=y+14;
a=0;
d=1;
for (i=0; i<16; i++)
{ put(blank,x2,y2);
  y2++;
  put(&man[a*SMALL_STEP],x2,y2);
  a=a+d;
  if (a==4) d=-1;
  if (a==0) d=1;
  delay(speed);
}
put(blank,x2,y2);
y2++;
put(&man[SMALL_STEP*5],x2,y2);
delay(speed*4);
for (i=0; i<10; i++)
{ put(&man[SMALL_STEP*6],x2,y2);
  delay(speed*2);
  put(&man[SMALL_STEP*7],x2,y2);
  delay(speed*2);
}
delay(speed*4);
put(blank,x2,y2);
}

int offscreen(x,y) int x,y;
{ /* return true if x or y is offscreen */
int z;

z=0;

if ((x<0)||(x>304))
  z=1;

if ((y<8)||(y>199))
  z=1;

return(z);
}

int sgn(x) int x;
{ /* return sign of x */
int z;

z=0;

if (x<0) z=-1;
if (x>0) z=1;
return(z);
}

update()
{ /* update displays */
unsigned z;

score=score+slspeed;
numprint(56,0,255,score);
time++;
numprint(248,8,255,time);
dist=dist+slspeed;
if (dist==0) dist=1;
z=dist/((1500+(level*400))/100);
numprint(280,0,255,z);
anim++;
}

finlevel()
{ /* end of level stuff */
int z;
slspeed=0;
while (y<196)
{ moveenemy();
  put(blank,x,y);
  y=y+8;
  put(&sled[AHEAD],x,y);
  delay(speed*diff);
}
put(blank,x,y);
vprint(80,80,255,"Time:");
delay(speed*20);
numprint(200,80,255,time);
delay(speed*20);
vprint(80,100,255,"Percentage:");
delay(speed*20);
z=((time-perfecttime)*100)/perfecttime;
z=100-z;
if ((Perfect)&&(z==100))
{ vprint(200,100,255,"PERFECT!");
  z=1000;
}
else
{ numprint(200,100,255,z);
  z=z*5;
}
delay(speed*20);
vprint(80,120,255,"Bonus:");
delay(speed*20);
numprint(200,120,255,z);
score=score+z;
numprint(56,0,255,score);
delay(speed*60);
delay(speed*60);
}

numprint(x,y,c,n) unsigned int x,y,c,n;
{ /* use vprint to display a number */
char b[6]; int i,f;

f=0;

c=c|0x100; /* always solid */
i=0;
if (n>9999)
{ b[i++]=n/10000+48;
  n=n%10000;
  f=1;
}
if ((f)||(n>999))
{ b[i++]=n/1000+48;
  n=n%1000;
  f=1;
}
if ((f)||(n>99))
{ b[i++]=n/100+48;
  n=n%100;
  f=1;
}
if ((f)||(n>9))
{ b[i++]=n/10+48;
  n=n%10;
  f=1;
}
b[i++]=n+48;
b[i]=0;
vprint(x,y,c,b);
}

gameover()
{ /* game over routine */
int y,x,ys;
int i1;
char buf[580];
char buf2[580];

y=33; x=124; ys=1;
get(buf2,x,y,8,72);
vprint(x,y,255|0x100,"GAME OVER");
get(buf,x,y,8,72);

while (y<190)
{ put(buf2,x,y);
  y=y+ys;
  get(buf2,x,y,8,72);
  put(buf,x,y);
  ys++;
  delay(speed*2);
}

while (ys)
{ put(buf2,x,y);
  y=y-ys;
  get(buf2,x,y,8,72);
  put(buf,x,y);
  ys=ys-2;
  if (ys<0) ys=0;
  delay(speed*2);
}

for (i1=0; i1<256; i1++)
{ vprint(x,y,i1,"GAME OVER");
  delay(speed);
}
}

gamewin()
{ /* game win routine */
int i,i1,i2;

fade_out(3);
load_bmp("sledwin.bmp");
fade_in(2);
for (i2=0; i2<100; i2=i2)
{ for (i1=0; i1<255; i1++)
  { i=i1*3;
    if ((tp[i]==tp[i+1])&&(tp[i]==tp[i+2])&&(tp[i]!=63)&&(tp[i]>50))
    { tp[i]=tp[i]-1;
      if (tp[i]<51)
        tp[i]=62;
      tp[i+1]=tp[i];
      tp[i+2]=tp[i];
      colour(i1,tp[i],tp[i+1],tp[i+2]);
    }
  }
  delay(speed);
  if (scr_csts()) i2=1001;
}
set_txt();
scr_cursoff();
show("<press ESC to exit>",10);
show("---",8);
show("Great work, and thanks for playing!!",6);
show("---",8);
show("be invincible!",5);
show("if you hold down BOTH Shift keys you will",5);
show("Not many ways to cheat in this one, but",5);
show("---",8);
show("Next up is At the Races, then Super Speed Acer!",3);
show("2 months late. Oh well, it's all in fun. :)",3);
show("Finally finished and released on 13 March 1996, at least",3);
show("---",8);
show("of Doom Mountain! Congratulations!",9);
show("You have successfully navigated your way to the bottom",9);
show("---",8);
show("YOU MADE IT!",11);
while (scr_csts()!=27);
}


