all: send

send: send.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $+ -o $@ -lwiringPi -lpthread -lcrypt -lrt

clean:
	$(RM) *.o send
