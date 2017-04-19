mod_authn_plugauth.so : mod_authn_plugauth.c
	apxs -i -a -c mod_authn_plugauth.c
	mv .libs/mod_authn_plugauth.so .
	rm -rf .libs
	rm -f *.la *.lo *.o *.slo	

server : mod_authn_plugauth.so
	httpd -X -f `pwd`/example.conf

clean:
	rm -rf .libs
	rm -f *.la *.lo *.o *.slo *.so
