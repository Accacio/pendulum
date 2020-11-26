##
# Pendulum
#
# @file
# @version 0.1

compile:
	gcc pendulum.c -Os -lncurses -lm -lGL -lglut -o pendulum

run:
	@./pendulum

# end
