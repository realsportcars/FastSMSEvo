ALL : fastsms_plugin.so

interface.h : interface.xml
	dbus-binding-tool --mode=glib-server --prefix=connection interface.xml > interface.h

conn.o : conn.c conn.h interface.h
	gcc -c conn.c -o conn.o `pkg-config --libs --cflags glib-2.0 gtk+-2.0 gconf-2.0`

plugin.o : plugin.c
	gcc -c plugin.c -o plugin.o `pkg-config --cflags hildon-input-method-framework-3.0 hildon-input-method-ui-3.0 glib-2.0 gtk+-2.0 gconf-2.0`

fastsms_plugin.so : plugin.o conn.o
	gcc plugin.o conn.o -shared -o fastsms_plugin.so `pkg-config --libs hildon-input-method-framework-3.0 hildon-input-method-ui-3.0 glib-2.0 gtk+-2.0 gconf-2.0`

clean :
	rm -f *.o
	rm -f interface.h
	rm -f *.so

