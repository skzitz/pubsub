CC	=	clang
CPP	=	clang++
CPPFLAGS=	-ggdb3 -Wall
LFLAGS	=	-ggdb3 -Wall
SRCS	=	pubsub.cpp
OBJS	=	$(SRCS:.cpp=.o)

.PHONY: all clean docs

all: pubsub

pubsub: $(OBJS)
	$(CPP) -o pubsub $(OBJS) $(LFLAGS)

.cpp.o:
	$(CPP) -o $@ -c $< $(CPPFLAGS)

clean:
	rm -rf pubsub $(OBJS) $~ html latex

docs:
	@doxygen
