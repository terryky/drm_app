include ../Makefile.env

TARGET = drm_atomic

SRCS = 
SRCS += main.c
SRCS += render_dbgstr.c
SRCS += render_grid.c
SRCS += render_telltale.c
SRCS += _bmp_data.c
SRCS += ../common/util_drm.c

OBJS =
OBJS += $(SRCS:%.c=./%.o)

INCLUDES +=-I../common/

CFLAGS   +=

LDFLAGS  +=

include ../Makefile.include
