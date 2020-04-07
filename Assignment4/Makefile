#######################################################################
#
# Copyright (C) 2020 David C. Harrison. All right reserved.
#
# You may not use, distribute, publish, or modify this code without 
# the express written permission of the copyright holder.
#
#######################################################################

include Makefile.libs  

CFLAGS = -Wall -lpthread
CC = gcc -g

default: manpage cartman

manpage: manpage.c manpage.a
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) manpage.a 2>&1 | tee make-manpage.out
	@strip manpage

cartman: cartman.c cartman.a
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) cartman.a 2>&1 | tee make-cartman.out
	@strip cartman

grade: clean manpage cartman *.out
	@./grade.sh 

clean:
	@rm -f manpage make.out

submit: clean
	@tar czvf ~/CSE130-Assignment4.tar.gz \
	--exclude=*.h --exclude=Makefile manpage.c cartman.c Makefile.libs
