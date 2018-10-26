CC	=	clang
CPP	=	clang++
CPPFLAGS=	-ggdb3 -Wall
LFLAGS	=	-ggdb3 -Wall
SRCS	=	pubsub.cpp
OBJS	=	$(SRCS:.cpp=.o)

all: pubsub

pubsub: $(OBJS)
	$(CPP) -o pubsub $(OBJS) $(LFLAGS)

.cpp.o:
	$(CPP) -o $@ -c $< $(CPPFLAGS)

clean:
	rm -f pubsub $(OBJS) $~
