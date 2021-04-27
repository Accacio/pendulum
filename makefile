# Pendulum
compile:
	gcc src/pendulum.c -Os -lncurses -lm -o pendulum

run:
	@./pendulum

clean:
	@ if [ -f pendulum ]; then rm pendulum; fi;
	@echo "Cleaned"

# end
