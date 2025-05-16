CC = m68k-amigaos-gcc
STRIP = m68k-amigaos-strip
CFLAGS = -mcrt=nix13 -g -O2 -Wno-builtin-declaration-mismatch
OUTPUT = /mnt/d/winuae/data_hd/w13/
BUILD_DIR = build

all: $(BUILD_DIR) $(BUILD_DIR)/hello $(BUILD_DIR)/helloamiga $(BUILD_DIR)/helloconsole $(BUILD_DIR)/hellointuition $(BUILD_DIR)/bounce

$(BUILD_DIR):
	mkdir -p $@

clear:
	rm -rf $(BUILD_DIR)

install: all
	cp $(BUILD_DIR)/* $(OUTPUT)

$(BUILD_DIR)/hellointuition: hellointuition.c
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/hellointuition hellointuition.c
	$(STRIP) $(BUILD_DIR)/hellointuition

$(BUILD_DIR)/helloconsole: helloconsole.c
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/helloconsole helloconsole.c
	$(STRIP) $(BUILD_DIR)/helloconsole

$(BUILD_DIR)/hello: hello.c
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/hello hello.c
	$(STRIP) $(BUILD_DIR)/hello

$(BUILD_DIR)/helloamiga: helloamiga.c
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/helloamiga helloamiga.c
	$(STRIP) $(BUILD_DIR)/helloamiga

$(BUILD_DIR)/bounce: bounce.c
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/bounce bounce.c
	$(STRIP) $(BUILD_DIR)/bounce
