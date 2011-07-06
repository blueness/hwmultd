ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = hwmultd.pod

dist_man_MANS = hwmultd.8

hwmultd.8: hwmultd.pod
	pod2man $(srcdir)/hwmultd.pod > hwmultd.8

CLEANFILES = $(srcdir)/hwmultd.8
