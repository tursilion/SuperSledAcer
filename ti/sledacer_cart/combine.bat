@rem * Cartridge layout, keeping to 8k blocks for ease
@rem * Bank0 - >6000
@rem *					0040	boot, unpacker (1k)
@rem *					0440	game_prog
@rem *          1984  final_p
@rem *          1fe0
@rem * Bank1 - >6002  
@rem *					0040	game_proh
@rem *          0e1e  sledacer_c
@rem *          10d0  sledacer_p
@rem *          1c08  final_c
@rem *          1e22
@rem * Bank2 - >6004
@rem *					0040  demq
@rem *          06B0
@rem * Bank3 - >6006
@rem *					0040	

@rem *game_prog (a000,1ffa) 1544 (5444)
@rem *game_proh (bffa,1132) dde (3550)
@rem *sledacer_c (2000,1800,V) 2b2 (690)
@rem *sledacer_p (0000,1800,v) b38 (2872)
@rem *final_c (2000,1800,V) 21a (538)
@rem *final_p (0000,1800,v) 65c (1628)
@rem *demq (2000,700) 670 (1648)

@rem ** WARNING:: DEMQ has patched DSRLNK that does not DSRLNK!
@rem using tiobj2bin on unpack will leave a big header you need to manually delete
@rem remember to take the c99 E/A startup code out of the executable, it loads in
@rem at startup and we need to delete the E/A copy code. At >A00C you should see
@rem bytes 0201 20FA 0202 02C2 - these load R1 with the destination address, and R2
@rem with the length. Change 20FA to 0000 (write to ROM) and 02C2 to 0002 (two bytes)
@rem this code loads color to a lower address then moves it up, delete the move code
@rem this is at >a4F8 - jmp >a516 - 100E

\work\setbinsize\release\setbinsize.exe header.bin 64
\work\setbinsize\release\setbinsize.exe unpack.bin 1024
\work\setbinsize\release\setbinsize.exe gameprog.pack 5444
\work\setbinsize\release\setbinsize.exe gameproh.pack 3550
\work\setbinsize\release\setbinsize.exe sledacer_c.pack 690
\work\setbinsize\release\setbinsize.exe sledacer_p.pack 2872
\work\setbinsize\release\setbinsize.exe final_c.pack 538
\work\setbinsize\release\setbinsize.exe final_p.pack 1628

@rem those are the only files that are stacked

copy /y /b header.bin + /b unpack.bin + /b gameprog.pack + /b final_p.pack /b bank0.bin
copy /y /b header.bin + /b gameproh.pack + /b sledacer_c.pack + /b sledacer_p.pack + /b final_c.pack /b bank1.bin
copy /y /b header.bin + /b demq.pack /b bank2.bin
copy /y /b header.bin bank3.bin

\work\setbinsize\release\setbinsize.exe bank0.bin 8192
\work\setbinsize\release\setbinsize.exe bank1.bin 8192
\work\setbinsize\release\setbinsize.exe bank2.bin 8192
\work\setbinsize\release\setbinsize.exe bank3.bin 8192

copy /y /b bank0.bin + /b bank1.bin + /b bank2.bin + /b bank3.bin /b sledacer32KB8.bin

