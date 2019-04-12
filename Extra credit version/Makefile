all:	sender recv

sender:	sender_signal.o
	g++ sender_signal.o -o sender

recv:	recv_signal.o
	g++ recv_signal.o -o recv

sender.o: sender_signal.cpp
	g++ -c sender_signal.cpp

recv.o:	recv_signal.cpp
	g++ -c recv_signal.cpp

clean:
	rm -rf *.o sender recv
