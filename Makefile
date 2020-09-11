CC = gcc
CFLAGS = -g -Wall -o

prog: obj/get_symbol_code.o obj/notation_converters.o obj/encode_data.o obj/main.o
	$(CC) $(CFLAGS) prog.exe $^

obj/main.o: main.c 
	$(CC) -c $(CFLAGS) $@ $^

#obj/%.o: %.asm
#    ml /c /Fo $@ $<

obj/encode_data.o: encode_data.asm
	ml /c /Fo $@ $^

obj/notation_converters.o: notation_converters.asm
	ml /c /Fo $@ $^

obj/get_symbol_code.o: get_symbol_code.asm
	ml /c /Fo $@ $^