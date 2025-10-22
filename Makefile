windows:
	clang src/*.c -o ThatWasAce -I./include -L./lib \
	-lallegro -lallegro_image -lallegro_audio -lallegro_acodec -lallegro_font -lallegro_ttf -g
	./ThatWasAce.exe