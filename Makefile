CC = gcc
CFLAGS = -g -Wall -o

prog: obj/get_symbol_code.o obj/notation_converters.o obj/get_ec_codewords.o obj/encode_message.o obj/get_bit_sequence.o obj/construct_qr_code.o obj/main.o
	$(CC) $(CFLAGS) prog.exe $^

obj/main.o: main.c 
	$(CC) -c $(CFLAGS) $@ $<

obj/construct_qr_code.o: construct_qr_code.c construct_qr_code.h
	$(CC) -c $(CFLAGS) $@ $<

obj/get_bit_sequence.o: get_bit_sequence.asm
	ml /c /Fo $@ $<

obj/get_ec_codewords.o: get_ec_codewords.asm
	ml /c /Fo $@ $<
obj/notation_converters.o: notation_converters.asm
	ml /c /Fo $@ $<

obj/encode_message.o: encode_message.asm
	ml /c /Fo $@ $<

obj/get_symbol_code.o: get_symbol_code.asm
	ml /c /Fo $@ $<