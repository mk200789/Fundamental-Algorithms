/* 
	Compiles with command line  gcc -o assign3 hw3.c -lX11 -lm -L/usr/X11R6/lib
	Note: Make sure your input file does not have an empty line, or else error will occur. 
	Run : ./assign3 test_in
	Homework #3
	Wan Kim Mok
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


int main(int argc, char *argv[]){
	FILE *fp;
	char c;
	int line_count = 0;

	fp = fopen(argv[1], "r");

	int previous_val = 0;
	if (fp == NULL){
		//If there's no file, print message and exit.
		printf("There's no file.\n");
		exit(0);
	}
	else{
		while(!feof(fp)){
			c = fgetc(fp);

			if (c == 10){
				line_count++;
			}
		}
		rewind(fp);
	}

	printf("total line: %d\n", line_count);
	int i;
	//stores variable v or h
	char temp[line_count+1];

	//stores the input in a 2d array
	int m[line_count+1][4];
	
	for (i=0; i <=line_count; i++){
		fscanf(fp, "%s %*d, %*d, %*d", &temp[i]);
	}
	rewind(fp);

	for (i=0; i<=line_count; i++){
		//printf("%d : %d \n", i, temp[i]);
		if (temp[i] == 104){
			//horizal line segment
			fscanf(fp, "%*s %d, %d, %d", &m[i][1], &m[i][0], &m[i][2]);
			m[i][3] = m[i][1];
		}
		else if (temp[i] == 118){
			//vertical line segment
			fscanf(fp, "%*s %d, %d, %d", &m[i][0], &m[i][1], &m[i][3]);
			m[i][2] = m[i][0];
		}
	}

	for (i=0; i<=line_count; i++){
		printf("(%d, %d) to (%d , %d)\n", m[i][0], m[i][1], m[i][2], m[i][3]);
	}


	fclose(fp);
	return 0;
}