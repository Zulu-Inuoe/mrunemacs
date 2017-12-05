# mrunemacs - emacs runner
# Written in 2017 by Wilfredo Velázquez-Rodríguez <zulu.inuoe@gmail.com>
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.
# You should have received a copy of the CC0 Public Domain Dedication along
# with this software. If not, see
# <http://creativecommons.org/publicdomain/zero/1.0/>.

OBJ = obj
BIN = bin

all: $(BIN)/mrunemacs.exe
$(OBJ):
	mkdir $(OBJ)
$(BIN):
	mkdir $(BIN)
$(OBJ)/icon.res: icon.ico icon.rc $(OBJ)
	windres icon.rc -O coff -o $(OBJ)/icon.res
$(OBJ)/mrunemacs.o: mrunemacs.c $(OBJ)
	gcc -o $(OBJ)/mrunemacs.o -c mrunemacs.c
$(BIN)/mrunemacs.exe: $(OBJ)/mrunemacs.o	$(OBJ)/icon.res $(BIN)
	gcc -mwindows -o $(BIN)/mrunemacs.exe $(OBJ)/mrunemacs.o $(OBJ)/icon.res

clean:
	rm -rf $(OBJ)
	rm -rf $(BIN)