CFLAGS=-I/util/include
LIBS  =-L/util/lib -lplugauth
VERSION=0.01
PREFIX=/usr/local

all : mod_authn_plugauth.so

mod_authn_plugauth.so : mod_authn_plugauth.c
	apxs -c $(CFLAGS) $(LIBS) mod_authn_plugauth.c
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
	rm -f *.spec
	rm -f httpd/conf/httpd.conf

dist : mod_authn_plugauth-$(VERSION).tar.gz

mod_authn_plugauth-$(VERSION).tar.gz :
	rm -rf mod_authn_plugauth-$(VERSION)
	mkdir -p mod_authn_plugauth-$(VERSION)
	cp Makefile *.c mod_authn_plugauth-$(VERSION)
	bsdtar zcvf mod_authn_plugauth-$(VERSION).tar.gz mod_authn_plugauth-$(VERSION)
	rm -rf mod_authn_plugauth-$(VERSION)

install : mod_authn_plugauth.so
	mkdir -p $(DESTDIR)$(PREFIX)/lib/apache2/modules
	install -m 755 mod_authn_plugauth.so $(DESTDIR)$(PREFIX)/lib/apache2/modules

spec : acps-mod_authn_plugauth.spec.tmpl
	env VERSION=$(VERSION) RELEASE=$$((`arpm -qa | grep acps-mod_authn_plugauth | cut -d- -f4 | cut -d. -f1` + 1)) perl -pe 's/(VERSION|RELEASE)/$$ENV{$$1}/eg' acps-mod_authn_plugauth.spec.tmpl > acps-mod_authn_plugauth.spec

rpm : spec mod_authn_plugauth-$(VERSION).tar.gz
	mkdir -p ~/rpmbuild/SOURCES
	cp -a mod_authn_plugauth-$(VERSION).tar.gz ~/rpmbuild/SOURCES
	rpmbuild -bb acps-mod_authn_plugauth.spec

distclean: clean
	rm -f httpd/var/log/*.log
	rm -f httpd/var/run/*.pid
	rm -f *.tar.gz

