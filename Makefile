windows:
	clang src/*.c -o ThatWasAce -I./include -L./lib -lallegro -lallegro_image -g
	./ThatWasAce.exe