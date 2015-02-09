CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic
LFLAGS = -lrt -lpthread

.PHONY : rivercrossing clean
all : clean rivercrossing

debug : CFLAGS += -g
debug : all
    
rivercrossing : main.o
	gcc $(CFLAGS) -o $@ $< $(LFLAGS)

clean :
	rm -f rivercrossing rivercrossing.o rivercrossing.out
	
%.o : %.c
	gcc $(CFLAGS) -o $@ -c $<
