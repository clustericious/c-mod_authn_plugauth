mod_authn_plugauth.so : mod_authn_plugauth.c
	apxs -i -a -c mod_authn_plugauth.c
	mv .libs/mod_authn_plugauth.so .
	rm -rf .libs
	rm -f *.la *.lo *.o *.slo	

server : mod_authn_plugauth.so
	httpd -E `pwd`/httpd/var/log/apache.starup.log -X -f `pwd`/httpd/conf/httpd.conf

check : mod_authn_plugauth.so
	httpd -t -f `pwd`/example.conf

clean distclean:
	rm -rf .libs
	rm -f *.la *.lo *.o *.slo *.so
	rm -f httpd/var/log/*.log
	rm -f httpd/var/run/*.pid
