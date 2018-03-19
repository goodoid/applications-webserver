all:
	g++ -g -Wall -o day1 src/server/server.cpp \
		-lpthread
		#-lsocket \

.PHONY:
	all
