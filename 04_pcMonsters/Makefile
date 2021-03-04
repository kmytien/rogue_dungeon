#makefile for 1.04 pcMonsters
CC = gcc
CXX = g++
ECHO = echo
RM = rm -f

TERM = "\"S2021\""

CFLAGS = -Wall -Werror -ggdb -funroll-loops -DTERM=$(TERM) -pg
CXXFLAGS = -Wall -Werror -ggdb -funroll-loops -DTERM=$(TERM)

LDFLAGS = -pg

BIN = rlg327
OBJS = rlg327.o heap.o dungeon.o path.o utils.o 327_assignment1.04.o

all: $(BIN) etags

$(BIN): $(OBJS)
	@$(ECHO) Linking $@
	@$(CC) $^ -o $@ $(LDFLAGS)

-include $(OBJS:.o=.d)

%.o: %.c
	@$(ECHO) Compiling $<
	@$(CC) $(CFLAGS) -MMD -MF $*.d -c $<

%.o: %.cpp
	@$(ECHO) Compiling $<
	@$(CXX) $(CXXFLAGS) -MMD -MF $*.d -c $<

.PHONY: all clean clobber etags

clean:
	@$(ECHO) Removing all generated files
	@$(RM) *.o $(BIN) *.d TAGS core vgcore.* gmon.out

clobber: clean
	@$(ECHO) Removing backup files
	@$(RM) *~ \#* *pgm

etags:
	@$(ECHO) Updating TAGS
	@etags *.[ch]
