main: main.c spaceship.h spaceship.c
	gcc -Iraylib-5.5_linux_amd64/include -o main main.c spaceship.h spaceship.c -Lraylib-5.5_linux_amd64/lib -l:libraylib.a -lm -Wall -Wextra -Werror && ./main