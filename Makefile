CC=g++
CFLAGS=-Wall
OBJDIR=obj
NAME=Supaplex
OBJDIR2=obj2
NAME2=GUI
RM=rm
LIBS=-lncurses -lpthread

SRCS=Simulator.cpp Field.cpp Game.cpp OpenListItem.cpp Supaplex.cpp TSPSolver.cpp stdafx.cpp
SRCS2=Simulator.cpp Field.cpp Game.cpp OpenListItem.cpp GUI-ascii.cpp main.cpp TSPSolver.cpp stdafx.cpp

OBJS:=$(SRCS:.cpp=.o)
OBJS:=$(addprefix $(OBJDIR)/,$(OBJS))
OBJS2:=$(SRCS2:.cpp=.o)
OBJS2:=$(addprefix $(OBJDIR2)/,$(OBJS2))


all: $(OBJDIR) $(NAME) $(OBJDIR2) $(NAME2)

$(OBJDIR):
	mkdir $(OBJDIR)
	
$(OBJDIR2):
	mkdir $(OBJDIR2)

$(OBJDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR2)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	
$(NAME2): $(OBJS2)
	$(CC) $(CFLAGS2) -o $@ $^ $(LIBS)

clean:
	$(RM) $(OBJDIR)/*.o
	$(RM) $(OBJDIR2)/*.o
