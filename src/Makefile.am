bin_PROGRAMS = m3player

m3player_SOURCES = avtransport.c renderingcontrol.c connectionmanager.c m3player.c presets.c gstreamer.c last_change.c

BUILT_SOURCES = avtransport.gen.c renderingcontrol.gen.c connectionmanager.gen.c

m3player_CFLAGS = -I$(top_srcdir) $(GTHREAD_CFLAGS) $(LIBGUPNP_CFLAGS) $(LIBGUPNPAV_CFLAGS) $(GST_CFLAGS)
m3player_LDFLAGS = -export-dynamic $(GTHREAD_LIBS) $(LIBGUPNP_LIBS) $(LIBGUPNPAV_LIBS) $(GST_LIBS)

avtransport.gen.c: AVTransportV1.xml 
		gupnp-binding-tool --prefix avtransport --mode server AVTransportV1.xml > avtransport.gen.c

renderingcontrol.gen.c: RenderingControlV1.xml 
		gupnp-binding-tool --prefix renderingcontrol --mode server RenderingControlV1.xml > renderingcontrol.gen.c

connectionmanager.gen.c: ConnectionManagerV1.xml 
		gupnp-binding-tool --prefix connectionmanager --mode server ConnectionManagerV1.xml > connectionmanager.gen.c

m3playerdir = $(datadir)/m3player
m3player_DATA = MediaRendererV1.xml MediaRendererV2.xml \
		AVTransportV1.xml AVTransportV2.xml \
		ConnectionManagerV1.xml ConnectionManagerV2.xml \
		RenderingControlV1.xml RenderingControlV2.xml

configdir = $(sysconfdir)/m3player
dist_config_DATA = m3player.ini

hotplugdir = $(sysconfdir)/hotplug.d/button
dist_hotplug_DATA = buttons

CLEANFILES = *.gen.c
DISTCLEANFILES = *.gen.c
MAINTAINERCLEANFILES = Makefile.in *.gen.c
