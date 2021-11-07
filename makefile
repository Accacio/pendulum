# [[file:Readme.org::*Testing][Testing:1]]
# Pendulum
compile:
	gcc -g src/pendulum.c -Os -lncursesw -I/usr/include/ncursesw -lm -ltcc -o pendulum -ldl -lpthread

run:
	@./pendulum

clean:
	@ if [ -f pendulum ]; then rm pendulum; fi;
	@echo "Cleaned"

# end
# Testing:1 ends here
