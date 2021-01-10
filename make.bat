cls
cc65 -t c64 -g main.c
cc65 -t c64 -g graphics.c
ca65 main.s
ca65 graphics.s
ld65 -o chartest.prg -C c64.cfg -Ln chartest.lbl -m chartest.map main.o graphics.o c64.lib



