CC=gcc
CFLAGS=
FMT=indent

mysh: shell.c interpreter.c shellmemory.c pcb.c readyQueue.c scheduler.c
	$(CC) $(CFLAGS) -c shell.c interpreter.c shellmemory.c pcb.c readyQueue.c scheduler.c
	$(CC) $(CFLAGS) -o mysh shell.o interpreter.o shellmemory.o pcb.o readyQueue.o scheduler.o

style: shell.c shell.h interpreter.c interpreter.h shellmemory.c shellmemory.h pcb.c pcb.h readyQueue.h readyQueue.c scheduler.h
	$(FMT) $?

clean: 
	$(RM) mysh; $(RM) *.o; $(RM) *~

