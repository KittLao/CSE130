#######################################################################
#
# Copyright (C) 2018 David C. Harrison. All right reserved.
#
# You may not use, distribute, publish, or modify this code without 
# the express written permission of the copyright holder.
#
#######################################################################

include Makefile.libs  

CFLAGS = -Wall
CC = gcc -g

SRC=$(wildcard *.c)

sort: $(SRC) merge.a
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) merge.a 2>&1 | tee make.out

check: grade

grade: clean sort
	@./grade.sh 

clean:
	@rm -f sort make.out

submit: clean
	@tar czvf ~/CSE130-Assignment1.tar.gz \
	--exclude=merge.h --exclude=Makefile *.c *.h Makefile.libs
