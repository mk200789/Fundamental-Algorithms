/*
	Experimenting with read file
	gcc -o -read read_file.c
	./read test_in
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


int main(int argc, char *argv[]){
	FILE *fp;
	char c;
	int line_count = 0;

	fp = fopen(argv[1], "r");


	if (fp == NULL){
		//If there's no file, print message and exit.
		printf("There's no file.\n");
		exit(0);
	}
	else{
		while(!feof(fp)){
			c = fgetc(fp);

			if (c == '\n'){
				line_count++;
			}			
		}
		rewind(fp);
	}

	printf("total line: %d\n", line_count+1);

	int m[line_count][7];
	int i;
	for (i = 0; i <= line_count; i++){
		fscanf(fp, "%*s ( %d, %d) ( %d, %d) (%d, %d)", &m[i][0], &m[i][1], &m[i][2], &m[i][3], &m[i][4], &m[i][5]);
	}

	for (i = 0; i <= line_count; i++){
		printf("%d, %d, %d, %d, %d, %d\n", m[i][0], m[i][1], m[i][2], m[i][3], m[i][4], m[i][5]);
	}
	

	fclose(fp);
	return 0;
}