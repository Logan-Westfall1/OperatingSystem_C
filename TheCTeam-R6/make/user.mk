.POSIX:

user/core.o: user/core.c include/string.h include/mpx/serial.h \
  include/mpx/device.h include/processes.h include/sys_req.h

USER_OBJECTS=\
	user/core.o \
	 user/comHandler.o \
	 user/comLibrary.o \
	 user/pcbUser.o \
	 user/memUser.o



