CC=gcc
# -std=c11
FLAGS= -std=gnu11 -Wall -Wextra -pedantic -MMD -g -fsanitize=address -fsanitize=leak -fsanitize=undefined
OPT=-O3
LIBS=-lSDL2 -lm -lpthread

OUT=asteroid
SRCS=$(shell find . -name "*.c")
OBJS=$(SRCS:.c=.o)
DEPS=$(OBJS:%.o=%.d)

$(OUT): $(OBJS)
	$(CC) $(FLAGS) $(OPT) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) -c $< -o $@ $(FLAGS) $(OPT)

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OBJS) $(OUT) $(DEPS)

-include $(DEPS)
