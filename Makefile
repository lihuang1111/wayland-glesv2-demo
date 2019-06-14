export CC=/home1/lihuang/3328_linux/prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc

PROJECT_DIR ?= /home1/lihuang/3328_linux/glesv2-binary-shader
PRG_INC_DIR ?= $(PROJECT_DIR)/include
PRG_LIB_DIR ?= $(PROJECT_DIR)/lib

simple-egl:	simple-egl.c
		${CC} ${CFLAGS} simple-egl.c -o ${@} -ldrm -lffi -lwayland-server -lm -lEGL -lGLESv2 -lwayland-egl -lwayland-client -lwayland-cursor -I ${PRG_INC_DIR} -L ${PRG_LIB_DIR}
