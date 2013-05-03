.SUFFIXES: .cpp .c .o

NAME=NetUtil
PATH:=${ALCHEMY_HOME}/achacks:${PATH}
INC =
CC = gcc
CCPP = g++
CFLAGS = -O3 -Wall
LDFLAGS =
OBJS = 

.cpp.o:
	$(CCPP) $(INC) $(CFLAGS) -c $<
.c.o:
	$(CC) $(INC) $(CFLAGS) -c $<

all: $(NAME)

glue.c : 

glue.o : glue.c

$(NAME) : glue.o
	$(CC) $(INC) $(CFLAGS) -swc -o $(NAME).swc glue.o $(OBJS) $(LDFLAGS) 

clean:
	rm *.o $(NAME).swc 
	rm swfbridge.log 
	rm *achacks*
	rm -r _sb_*
