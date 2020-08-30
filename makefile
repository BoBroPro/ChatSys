CC := g++
FLAG:= -std=c++17 
all: client.out server.out

client.out: client.o
	$(CC) $(FLAG) $^ -o $@
server.out: server.o extractID.o dealconnmsg.o dealloginedmsg.o dealmsg.o IDandname.o ultoa.o
	$(CC) $(FLAG) $^ -o $@

client.o: client.cpp utility.hpp
	$(CC) $(FLAG) -c $< -o $@

server.o: server.cpp utility.hpp 
	$(CC) $(FLAG) -c $< -o $@

extractID.o: extractID.cpp extractID.hpp utility.hpp
	$(CC) $(FLAG) -c $< -o $@

dealconnmsg.o: dealconnmsg.cpp dealconnmsg.hpp IDandname.hpp utility.hpp
	$(CC) $(FLAG) -c $< -o $@

dealloginedmsg.o: dealloginedmsg.cpp dealloginedmsg.hpp extractID.hpp ultoa.hpp utility.hpp
	$(CC) $(FLAG) -c $< -o $@

dealmsg.o: dealmsg.cpp dealmsg.hpp dealloginedmsg.hpp ultoa.hpp utility.hpp
	$(CC) $(FLAG) -c $< -o $@

IDandname.o: IDandname.cpp IDandname.hpp utility.hpp
	$(CC) $(FLAG) -c $< -o $@

ultoa.o: ultoa.cpp ultoa.hpp utility.hpp 
	$(CC) $(FLAG) -c $< -o $@

.PHONY: clean
clean:
	-rm *.o *.out
	