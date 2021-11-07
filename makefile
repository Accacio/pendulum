# [[file:Readme.org::*Testing][Testing:1]]
# Pendulum
all: pendulum
pendulum: src/pendulum.c
	@gcc $< -Os -lncursesw -lm -ltcc -lpthread -ldl -o $@


run:
	@./pendulum

clean:
	@ if [ -f pendulum ]; then rm pendulum; fi;
	@echo "Cleaned"

# end
# Testing:1 ends here
