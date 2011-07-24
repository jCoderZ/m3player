AM_CFLAGS = -I$(top_srcdir) $(GTHREAD_CFLAGS) $(LIBGUPNP_CFLAGS) $(LIBGUPNPAV_CFLAGS) $(GST_CFLAGS)

noinst_PROGRAMS = m3ddity-player

#BUILT_SOURCES = avtransport.gen.c renderingcontrol.gen.c connectionmanager.gen.c
avtransport.$(OBJEXT): avtransport.gen.c avtransport.c
renderingcontrol.$(OBJEXT): renderingcontrol.gen.c renderingcontrol.c
connectionmanager.$(OBJEXT): connectionmanager.gen.c connectionmanager.c

avtransport.gen.c: AVTransport/description2.xml 
		gupnp-binding-tool --prefix avtransport --mode server AVTransport/description1.xml > avtransport.gen.c

renderingcontrol.gen.c: RenderingControl/description2.xml 
		gupnp-binding-tool --prefix renderingcontrol --mode server RenderingControl/description1.xml > renderingcontrol.gen.c

connectionmanager.gen.c: ConnectionManager/description2.xml 
		gupnp-binding-tool --prefix connectionmanager --mode server ConnectionManager/description1.xml > connectionmanager.gen.c

m3ddity_player_SOURCES = avtransport.c renderingcontrol.c connectionmanager.c m3ddity-player.c \
	presets.c  presets.h  gstreamer.c  gstreamer.h  last_change.c\
	last_change.h
m3ddity_player_LDFLAGS = -export-dynamic $(GTHREAD_LIBS) $(LIBGUPNP_LIBS) $(LIBGUPNPAV_LIBS) $(GST_LIBS)

EXTRA_DIST = description1.xml description2.xml \
	     AVTransport/description1.xml AVTransport/description2.xml \
	     ConnectionManager/description1.xml ConnectionManager/description2.xml \
	     RenderingControl/description1.xml RenderingControl/description2.xml

CLEANFILES = *.gen.c
DISTCLEANFILES = *.gen.c
MAINTAINERCLEANFILES = Makefile.in *.gen.c
