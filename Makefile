BUILD_DIR=build/
HEADERS_DIR=./ headers/
LIB_SRCS_DIR=lib/
LIBS_DIR=$(BUILD_DIR)$(LIB_SRCS_DIR)
EXECS_SRCS_DIR=programs/

LIB_TARGETS+=anti-uav_tcp_connector
##################      Do not change below       #############################
LIB_OBJ=$(addprefix $(LIB_SRCS_DIR), $(LIB_TARGETS:=.o))
CFLAGS=-Wall -Werror -Wextra
IFLAGS=$(addprefix -I, $(HEADERS_DIR))
LFLAGS=$(addprefix -L, $(LIBS_DIR))
LIBS=-lpthread -lanti-uav_tcp_connector

all: example.out

debug: CFLAGS+=-g
debug: example.out

example.out: libanti-uav_tcp_connector.a
	gcc $(CFLAGS) $(IFLAGS) $(LFLAGS) $(EXECS_SRCS_DIR)example.c $(LIBS) -o $@

libanti-uav_tcp_connector.a: $(LIB_OBJ)
	ar rc $(BUILD_DIR)$(LIB_SRCS_DIR)$@ $(addprefix $(BUILD_DIR), $<)
	ranlib $(BUILD_DIR)$(LIB_SRCS_DIR)$@

%.o: %.c
	mkdir -p $(BUILD_DIR)$(@D)
	gcc $(CFLAGS) $(IFLAGS) -c $< -o $(BUILD_DIR)$@

clean:
	-rm -rf $(BUILD_DIR)
	-rm example.out