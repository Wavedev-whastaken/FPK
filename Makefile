CC = gcc
CFLAGS = -O3

all: fpk.exe ui.exe api.exe fpk.dll

# DLL build
fpk.dll: fpk_dll.o
	$(CC) -shared -o fpk.dll fpk_dll.o

fpk_dll.o: fpk.c fpk.h
	$(CC) $(CFLAGS) -DFPK_DLL_EXPORTS -c fpk.c -o fpk_dll.o

# Static object for EXEs
fpk.o: fpk.c fpk.h
	$(CC) $(CFLAGS) -c fpk.c -o fpk.o

fpk.exe: fpk_main.o fpk.o
	$(CC) $(CFLAGS) -o fpk.exe fpk_main.o fpk.o

fpk_main.o: fpk_main.c fpk.h
	$(CC) $(CFLAGS) -c fpk_main.c

ui.exe: ui.o fpk.dll
	$(CC) $(CFLAGS) -o ui.exe ui.o -mwindows fpk.dll

ui.o: ui.c fpk.h
	$(CC) $(CFLAGS) -c ui.c

api.exe: api.o fpk.o
	$(CC) $(CFLAGS) -o api.exe api.o fpk.o

api.o: api.c fpk.h
	$(CC) $(CFLAGS) -c api.c

clean:
	del /Q *.o 2>nul
	del /Q fpk.exe 2>nul
	del /Q ui.exe 2>nul
	del /Q api.exe 2>nul
	del /Q fpk.dll 2>nul