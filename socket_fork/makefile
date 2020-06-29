src = $(wildcard *.c)
obj = $(patsubst %.c,%.o,$(src))

all:serve client

serve: serve.o wrap.o
	gcc serve.o wrap.o -o serve -Wall 
client: client.o wrap.o
	gcc client.o wrap.o -o client -Wall
%.o:%.c
	gcc -c $< -Wall 
.PHONY:clean all
clean:
	-rm -rf server client $(obj) 





