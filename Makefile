build: steg.c
	$(CC) steg.c -o steg -Wall -Wextra -pedantic -std=c99 -g
enc: steg.c
	$(CC) steg.c -o steg -g -Wall -Wextra -pedantic -std=c99
	./steg.exe -e -t test-files/original.bmp generated-files/enc-output.bmp test-files/plain.txt

dec: steg.c
	$(CC) steg.c -o steg -g -Wall -Wextra -pedantic -std=c99
	./steg.exe -d -t generated-files/enc-output.bmp generated-files/dec-out.txt

.PHONY: build 
