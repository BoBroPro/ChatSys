CC := g++
FLAG:= -std=c++17 
LIB:= /usr/local/lib
all: client.out server.out

client.out: client.o
	$(CC) $(FLAG) $^ -o $@
server.out: server.o extractID.o dealconnmsg.o dealloginedmsg.o dealmsg.o IDandname.o ultoa.o dealsigninorup.o dealsignin.o dealsignup1.o dealsignup2.o \
dealsignup3.o puserfrommapID.o otherfunc.o

	$(CC) $(FLAG) -L$(LIB) -lhiredis $^ -o $@

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

dealmsg.o: dealmsg.cpp dealmsg.hpp dealloginedmsg.hpp ultoa.hpp utility.hpp otherfunc.hpp
	$(CC) $(FLAG) -c $< -o $@

IDandname.o: IDandname.cpp IDandname.hpp utility.hpp
	$(CC) $(FLAG) -c $< -o $@

ultoa.o: ultoa.cpp ultoa.hpp utility.hpp 
	$(CC) $(FLAG) -c $< -o $@

dealsigninorup.o: dealsigninorup.cpp dealsigninorup.hpp utility.hpp
	$(CC) $(FLAG) -c $< -o $@

dealsignin.o: dealsignin.cpp dealsignin.hpp utility.hpp otherfunc.hpp
	$(CC) $(FLAG) -c $< -o $@

dealsignup1.o: dealsignup1.cpp dealsignup1.hpp IDandname.cpp IDandname.hpp utility.hpp otherfunc.hpp
	$(CC) $(FLAG) -c $< -o $@

dealsignup2.o: dealsignup2.cpp dealsignup2.hpp utility.hpp
	$(CC) $(FLAG) -c $< -o $@

dealsignup3.o: dealsignup3.cpp dealsignup3.hpp utility.hpp hiredisutility.hpp otherfunc.hpp
	$(CC) $(FLAG) -c $< -o $@

puserfrommapID.o: puserfrommapID.cpp puserfrommapID.hpp utility.hpp
	$(CC) $(FLAG) -c $< -o $@

otherfunc.o: otherfunc.cpp otherfunc.hpp utility.hpp
	$(CC) $(FLAG) -c $< -o $@

utility.hpp: hiredisutility.hpp

.PHONY: clean
clean:
	-rm *.o *.out
	