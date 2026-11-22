# IFJ26 compiler - submission Makefile.
# Used in the flat submission archive (all .c/.h files in one directory),
# scripts/flatten.py copies it there. Not meant for the src/ tree itself.
# AI has been used to generate this Makefile
# Aleš Obr <xobrale00@stud.fit.vutbr.cz

CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -pedantic -O2
LDFLAGS =
TARGET  = ifj26

SRCS = $(wildcard *.c)
HDRS = $(wildcard *.h)
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Every object is rebuilt when any header changes (simple and safe for a flat tree)
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
