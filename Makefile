CFLAGS=-I/util/include
LIBS  =-L/util/lib -lplugauth

all : mod_authn_plugauth.so

mod_authn_plugauth.so : mod_authn_plugauth.c
	apxs -i -a -c $(CFLAGS) $(LIBS) mod_authn_plugauth.c
	mv .libs/mod_authn_plugauth.so .
	rm -rf .libs
	rm -f *.la *.lo *.o *.slo

httpd/conf/httpd.conf : httpd/conf/httpd.conf.tmpl
	perl -pe 's/MODULE_SRC_ROOT/$$ENV{PWD}/g' httpd/conf/httpd.conf.tmpl > httpd/conf/httpd.conf

server : mod_authn_plugauth.so httpd/conf/httpd.conf
	httpd -E `pwd`/httpd/var/log/apache.starup.log -X -f `pwd`/httpd/conf/httpd.conf || (tail httpd/var/log/apache.starup.log && false)

check : mod_authn_plugauth.so
	httpd -t -f `pwd`/example.conf

clean:
	rm -rf .libs
	rm -f *.la *.lo *.o *.slo *.so
	rm -f httpd/conf/httpd.conf

distclean: clean
	rm -f httpd/var/log/*.log
	rm -f httpd/var/run/*.pid

