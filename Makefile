CC=g++
CFLAGS=-ggdb -Wall -pedantic -std=c++11 -Werror -Wextra
DEPS = httpd.h CalcFramer.hpp CalcParser.hpp 
SRCS = httpd.cpp HandleTCPClient.cpp CalcFramer.cpp CalcParser.cpp CreateTCPServerSocket.cpp AcceptTCPConnection.cpp
MAIN_SRCS = main.cpp $(SRCS)
MAIN_OBJS = $(MAIN_SRCS:.c=.o)

default: httpd

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

httpd:    $(MAIN_OBJS)
	$(CC) $(CFLAGS) -o httpd $(MAIN_OBJS) -lpthread

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f httpd *.o
