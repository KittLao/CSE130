#######################################################################
#
# Copyright (C) 2020 David C. Harrison. All right reserved.
#
# You may not use, distribute, publish, or modify this code without 
# the express written permission of the copyright holder.
#
#######################################################################

CFLAGS = -Wall 
CC = gcc -g

default: fileman

fileman: *.c fileman.a
	$(CC) -o $@ $^ $(CFLAGS) fileman.a 2>&1 | tee make.out
	@strip fileman

check:
	@./check.sh 

grade: clean fileman
	@./grade.sh 

clean:
	@rm -f fileman *.out

submit: clean
	@tar czvf ~/CSE130-Assignment5.tar.gz \
	--exclude=fileman.h --exclude=Makefile *.c
