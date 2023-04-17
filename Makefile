BUILD_DIR=build/
HEADERS_DIR=./ headers/
LIB_SRCS_DIR=lib/
LIBS_DIR=$(BUILD_DIR)$(LIB_SRCS_DIR)
EXECS_SRCS_DIR=programs/
##################      Do not change below       #############################
CFLAGS=-Wall -Werror -Wextra
IFLAGS=$(addprefix -I, $(HEADERS_DIR))
LFLAGS=$(addprefix -L, $(LIBS_DIR))

all: example_module_id

debug: CFLAGS+=-g
debug: example_module_id

example_module_id: libanti-uav_link.a
	gcc $(CFLAGS) $(IFLAGS) $(LFLAGS) $(EXECS_SRCS_DIR)example_module_id.c -lanti-uav_link -o example_module_id

libanti-uav_link.a: $(LIB_SRCS_DIR)anti-uav_link.o
	ar rc $(BUILD_DIR)$(LIB_SRCS_DIR)$@ $(addprefix $(BUILD_DIR), $<)
	ranlib $(BUILD_DIR)$(LIB_SRCS_DIR)$@

%.o: %.c
	mkdir -p $(BUILD_DIR)$(@D)
	gcc $(CFLAGS) $(IFLAGS) $(LFLAGS) -c $< -o $(BUILD_DIR)$@

clean:
	-rm -rf $(BUILD_DIR)