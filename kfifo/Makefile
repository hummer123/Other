PROGNAME = main
OBJS     = kfifo.o main.o
CFLAGS   = -g -lpthread


$(PROGNAME): $(OBJS)
	$(CC) -o $(PROGNAME) $(OBJS) $(CFLAGS) 

%.o: %.c
	$(CC) -c $^ -o $@ -g -lpthread
	
clean:
	rm -f *.o $(PROGNAME)
	