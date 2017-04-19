CC=gcc
LD=gcc
CFLAGS=-fPIC -I/usr/local/curl/include
LIBS=-L/usr/local/curl/lib -lcurl
LDFLAGS=-shared
OBJECTS=client.o auth.o

libplugauth.so : $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o libplugauth.so $(LIBS)

%.o : %.c plugauth.h plugauth_private.h
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f *.o *.so
