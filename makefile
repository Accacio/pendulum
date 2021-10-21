# [[file:Readme.org::*Testing][Testing:1]]
# Pendulum
compile:
	gcc src/pendulum.c -Os -lncursesw -I/usr/include/ncursesw -lm -ltcc -o pendulum -ldl

run:
	@./pendulum

clean:
	@ if [ -f pendulum ]; then rm pendulum; fi;
	@echo "Cleaned"

# end
# Testing:1 ends here
