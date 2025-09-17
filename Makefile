CC = x86_64-w64-mingw32-gcc
CFLAGS = -O2 -Wall
BUILDDIR = build

SRCS = fpk.c fpk_main.c
OBJS = $(addprefix $(BUILDDIR)/,$(SRCS:.c=.o))

all: $(BUILDDIR)/fpk64.exe

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: %.c fpk.h | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/fpk64.exe: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(BUILDDIR)
