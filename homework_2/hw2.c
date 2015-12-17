
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//int rand_lp(int n, double *A, double *b, double *c, double *result){
int rand_lp(int n, double A[][4], double b[], double c[], double result[]){

   int row = 4, col 5; 
   int steps = 0;
   int inequality1 = 1, inequality2 = 2, inequality3 = 3 , inequality4 = 4;
   double matrix_a[row][col], matrix_b[col];
   int s1, s2, i, j, left_hand_eq;

   for (i=0; i<row; i++){
      //Set all result to 10000
      result[i] = 10000;
   }

   for (i=row; i<n; i++){
      left_hand_eq = a[i][0]*result[0] + a[i][1]*result[1] + a[i][2]*result[2] + a[i][3]*result[3];

      if(left_hand_eq > b[i]){
         steps++;
         for (j=0; j<row; j++){
            //solving
            if (j == 0){

            }
            else if (j == 1){

            }
            else if (j == 2){

            }
            else{
               
            }
         }
      }
   }

   //return steps;
   return 0;
}

main()
{  
   double A[600000][4],  b[600000], c[4] ;
   double result[4];
   int i, j; double s, t;
   printf("Preparing test: 4 variables, 600000 inequalities\n");
   A[0][0] = 1.0; A[0][1] = 2.0; A[0][2] = 1.0; A[0][3] = 0.0; b[0] = 10000.0;
   A[1][0] = 0.0; A[1][1] = 1.0; A[1][2] = 2.0; A[1][3] = 1.0; b[1] = 10000.0;
   A[2][0] = 1.0; A[2][1] = 0.0; A[2][2] = 1.0; A[2][3] = 3.0; b[2] = 10000.0;
   A[3][0] = 4.0; A[3][1] = 0.0; A[3][2] = 1.0; A[3][3] = 1.0; b[3] = 10000.0;
   c[0]=1.0; c[1]=1.0; c[2]=1.0; c[3]=1.0;
   for( i=4; i< 100000; i++ )
   {  A[i][0] = (12123*i)%104729; 
      A[i][1] = (47*i)%104729; 
      A[i][2] = (2011*i)%104729; 
      A[i][3] = (7919*i)%104729;
      b[i] = A[i][0] + 2*A[i][1] + 3*A[i][2] + 4* A[i][3] + 1 + (i%137);
   }
   A[100000][0] = 0.0; A[100000][1] = 6.0; A[100000][2] = 1.0; 
   A[100000][3] = 1.0; b[100000] = 19.0;
   for( i=100001; i< 200000; i++ )
   {  A[i][0] = (2323*i)%101111; 
      A[i][1] = (74*i)%101111; 
      A[i][2] = (2017*i)%101111; 
      A[i][3] = (7915*i)%101111;
      b[i] = A[i][0] + 2*A[i][1] + 3*A[i][2] + 4* A[i][3] + 2 + (i%89);
   }
   A[200000][0] = 5.0; A[200000][1] = 2.0; A[200000][2] = 0.0; 
   A[200000][3] = 1.0; b[200000] = 13.0;
   for( i=200001; i< 300000; i++ )
   {  A[i][0] = (23123*i)%100003; 
      A[i][1] = (47*i)%100003; 
      A[i][2] = (2011*i)%100003; 
      A[i][3] = (7919*i)%100003;
      b[i] = A[i][0] + 2*A[i][1] + 3*A[i][2] + 4* A[i][3] + 2 + (i%57);
   }
   A[300000][0] = 1.0; A[300000][1] = 2.0; A[300000][2] = 1.0; 
   A[300000][3] = 3.0; b[300000] = 20.0;
   A[300001][0] = 1.0; A[300001][1] = 0.0; A[300001][2] = 5.0; 
   A[300001][3] = 4.0; b[300001] = 32.0;
   A[300002][0] = 7.0; A[300002][1] = 1.0; A[300002][2] = 1.0; 
   A[300002][3] = 7.0; b[300002] = 40.0;
   for( i=300003; i< 400000; i++ )
   {  A[i][0] = (13*i)%103087; 
      A[i][1] = (99*i)%103087; 
      A[i][2] = (2012*i)%103087; 
      A[i][3] = (666*i)%103087;
      b[i] = A[i][0] + 2*A[i][1] + 3*A[i][2] + 4* A[i][3] + 1;
   }
   for( i=400000; i< 500000; i++ )
   {  A[i][0] = 1; 
      A[i][1] = (17*i)%999983; 
      A[i][2] = (1967*i)%444443; 
      A[i][3] = 2;
      b[i] = A[i][0] + 2*A[i][1] + 3*A[i][2] + 4* A[i][3] + (1000000.0/(double)i);
   }
   for( i=500000; i< 600000; i++ )
   {  A[i][0] = (3*i)%111121; 
      A[i][1] = (2*i)%999199; 
      A[i][2] = (2*i)%444443; 
      A[i][3] = i;
      b[i] = A[i][0] + 2*A[i][1] + 3*A[i][2] + 4* A[i][3] + 1.3;
   }
   
   printf("Running test: 600000 inequalities, 4 variables\n");
   //j = rand_lp(600000, &(A[0][0]), &(b[0]), &(c[0]), &(result[0]));
   j = rand_lp(600000, A, b, c, result);
   printf("Test: extremal point (%f, %f, %f, %f) after %d recomputation steps\n", result[0], result[1], result[2], result[3], j);
   printf("Answer should be (1,2,3,4)\n End Test\n");
}

