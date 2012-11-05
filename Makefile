CC=g++
CFLAGS=-Wall
OBJDIR=obj
NAME=Supaplex
RM=rm

SRCS=Simulator.cpp Field.cpp Game.cpp OpenListItem.cpp Supaplex.cpp \
TSPSolver.cpp stdafx.cpp

OBJS:=$(SRCS:.cpp=.o)
OBJS:=$(addprefix $(OBJDIR)/,$(OBJS))


all: $(OBJDIR) $(NAME)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	$(RM) $(OBJDIR)/*.o
