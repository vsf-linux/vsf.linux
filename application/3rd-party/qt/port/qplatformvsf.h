#ifdef __VSF__
// on windows/linux/macos connect/accpet/bind/socket/select etc are MACROs to avoid conflict with the same API in hostos.
#  ifdef connect
#    undef connect
#  endif
#  ifdef accept
#    undef accept
#  endif
#  ifdef socket
#    undef socket
#  endif
#  ifdef select
#    undef select
#  endif
#  ifdef bind
#    undef bind
#  endif
#  ifdef socket
#    undef socket
#  endif
#endif