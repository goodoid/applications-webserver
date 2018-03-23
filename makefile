output="day3"
all:
	g++ -g -Wall -o $(output) src/server/server.cpp \
		-lpthread
		#-lsocket \

.PHONY:
	all
