lib_LTLIBRARIES = libyumaagt.la

libyumaagt_la_SOURCES = \
$(top_srcdir)/netconf/src/agt/agt_acm.c \
$(top_srcdir)/netconf/src/agt/agt.c \
$(top_srcdir)/netconf/src/agt/agt_cap.c \
$(top_srcdir)/netconf/src/agt/agt_cb.c \
$(top_srcdir)/netconf/src/agt/agt_cli.c \
$(top_srcdir)/netconf/src/agt/agt_connect.c \
$(top_srcdir)/netconf/src/agt/agt_hello.c \
$(top_srcdir)/netconf/src/agt/agt_ncx.c \
$(top_srcdir)/netconf/src/agt/agt_ncxserver.c \
$(top_srcdir)/netconf/src/agt/agt_nmda.c \
$(top_srcdir)/netconf/src/agt/agt_not.c \
$(top_srcdir)/netconf/src/agt/agt_plock.c \
$(top_srcdir)/netconf/src/agt/agt_proc.c \
$(top_srcdir)/netconf/src/agt/agt_rpc.c \
$(top_srcdir)/netconf/src/agt/agt_rpcerr.c \
$(top_srcdir)/netconf/src/agt/agt_ses.c \
$(top_srcdir)/netconf/src/agt/agt_signal.c \
$(top_srcdir)/netconf/src/agt/agt_state.c \
$(top_srcdir)/netconf/src/agt/agt_yang_library.c \
$(top_srcdir)/netconf/src/agt/agt_sys.c \
$(top_srcdir)/netconf/src/agt/agt_time_filter.c \
$(top_srcdir)/netconf/src/agt/agt_timer.c \
$(top_srcdir)/netconf/src/agt/agt_top.c \
$(top_srcdir)/netconf/src/agt/agt_tree.c \
$(top_srcdir)/netconf/src/agt/agt_util.c \
$(top_srcdir)/netconf/src/agt/agt_val.c \
$(top_srcdir)/netconf/src/agt/agt_val_parse.c \
$(top_srcdir)/netconf/src/agt/agt_xml.c \
$(top_srcdir)/netconf/src/agt/agt_xpath.c \
$(top_srcdir)/netconf/src/agt/agt_cfg.c \
$(top_srcdir)/netconf/src/agt/agt_commit_complete.c \
$(top_srcdir)/netconf/src/agt/agt_commit_validate.c \
$(top_srcdir)/netconf/src/agt/agt_not_queue_notification_cb.c

libyumaagt_la_CPPFLAGS = -DDISABLE_YUMA_INTERFACES -I$(top_srcdir)/netconf/src/agt -I$(top_srcdir)/netconf/src/mgr -I$(top_srcdir)/netconf/src/ncx -I$(top_srcdir)/netconf/src/platform -I$(top_srcdir)/netconf/src/ydump $(XML_CPPFLAGS)
libyumaagt_la_LDFLAGS = -version-info 2:0:0 $(top_builddir)/netconf/src/ncx/libyumancx.la $(XML_LIBS) -lcrypto -lrt $(LIBS)
