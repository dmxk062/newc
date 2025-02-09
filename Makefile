CC := gcc
AS := as

ASFLAGS :=

CCFLAGS := -nostdlib
CCFLAGS += -nostdinc
CCFLAGS += -fno-unwind-tables
CCFLAGS += -fno-asynchronous-unwind-tables
CCFLAGS += -Wno-builtin-declaration-mismatch
CCFLAGS += -fno-stack-protector
CCFLAGS += -Wall
CCFLAGS += -Iinclude/ -include include/types.h

LDFLAGS :=

BUILDDIR = build
SRC_C = $(wildcard lib/*.c) $(wildcard lib/*/*.c)
OBJ_C = $(addprefix $(BUILDDIR)/, $(SRC_C:.c=.o))

SRC_A = $(wildcard asm/*.S)
OBJ_A = $(addprefix $(BUILDDIR)/, $(SRC_A:.S=.o))

SRC_TEST = $(wildcard test/*)
OUT_TEST = $(notdir $(SRC_TEST))

LIBRARY = $(BUILDDIR)/newc.o

BUILDDIRS = $(addprefix $(BUILDDIR)/, lib/struct lib/libc asm test)
$(BUILDDIRS):
	@mkdir -p $(BUILDDIRS) build

clean:
	@rm -rf $(BUILDDIRS) build

$(OUT_TEST): $(LIBRARY)
	@mkdir -p "$(BUILDDIR)/test/$@"
	$(CC) $(CCFLAGS) $(wildcard test/$@/*) $(LIBRARY) -o $(BUILDDIR)/$(OUT_TEST)

tests: $(OUT_TEST)

$(LIBRARY): $(OBJ_A) $(OBJ_C) $(BUILDDIRS)
	ld -r $(LDFLAGS) -o $(LIBRARY) $(OBJ_C) $(OBJ_A)

$(BUILDDIR)/%.o: %.c $(BUILDDIRS)
	$(CC) $(CCFLAGS) -fPIC -c $< -o $@

$(BUILDDIR)/%.o: %.S $(BUILDDIRS)
	$(AS) $(ASFLAGS) $< -o $@

all: tests
