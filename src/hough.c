#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	FILE *fpi, *fpo;
	unsigned char  idat;

	if((fpi=fopen("/vagrant/image/music.jpg", "rb")) == NULL){
		fprintf(stderr, "input file open error\n");
		exit(1);
	}

	if((fpo=fopen("/vagrant/image/music_change.jpg", "wb")) == NULL){
		fprintf(stderr, "output file open error\n");
		exit(1);
	}

	fclose(fpi);
	fclose(fpo);

	return (0);
}

