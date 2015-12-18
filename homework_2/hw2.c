/* 
   Compiles with command line  gcc -o assign2 hw2.c
   Run : ./assign2 
   Homework #2
   Wan Kim Mok
   Due: November 04, 2015
*/

#include <stdio.h>

double result[4];

int update_matrix(double matrix_b[]){
   double sum = 0;
   int i;

   for(i=0; i<4; i++){
      sum += (matrix_b[i] * result[i]);
   }
   
   if(sum <= matrix_b[4]){
      return -1;
   }

   return 0;
}


int calculate_result(double matrix_a[][5]){
   int i, j, k = 0;
   double temp_a[4][5];

   for (i=1; i<5; i++){
      for (j=0; j<5; j++){
         if(matrix_a[0][0] == 0){
            temp_a[0][j] = 0;
         }
         else{
            temp_a[0][j] = matrix_a[0][j] * (matrix_a[i][0]/matrix_a[0][0]);
         }
      }

      for (j=0; j<5; j++){
         matrix_a[i][j] = matrix_a[i][j] - temp_a[0][j];
      }
   }

   for(i=2; i<4; i++){
      for(j=1; j<5; j++){
         if(matrix_a[1][1] == 0){
            temp_a[1][j] = 0;
         }
         else{
            temp_a[1][j] = matrix_a[1][j] * (matrix_a[i][1]/matrix_a[1][1]);
         }
      }
      for(j=1; j<5; j++){
         matrix_a[i][j] = matrix_a[i][j] - temp_a[1][j];
      }
   }

   for(i=2; i<5; i++){
      if(matrix_a[1][1] == 0){
         temp_a[2][i] = 0;
      }
      else{
         temp_a[2][i] = matrix_a[2][i] * (matrix_a[3][2]/matrix_a[2][2]);
      }
   }

   for(i=2; i<5; i++){
      matrix_a[3][i] = matrix_a[3][i] - temp_a[2][i];
   }
   

   for(i=0; i<4; i++){
      for(j=0; j<4; j++){
         if(matrix_a[i][j] != 0){
            k++;
            break;
         }
      }
   }

   if(k == 4){
      result[3] = matrix_a[3][4]/matrix_a[3][3];
      result[2] = (matrix_a[2][4] - (result[3]*matrix_a[2][3])) / matrix_a[2][2];
      result[1] = (matrix_a[1][4] - (result[3]*matrix_a[1][3]) - (result[2]*matrix_a[1][2])) / matrix_a[1][1];
      result[0] = (matrix_a[0][4] - (result[3]*matrix_a[0][3]) - (result[2]*matrix_a[0][2]) - (result[1]*matrix_a[0][1])) / matrix_a[0][0];
      return -1;
   }

   return 0;
 
}


//int rand_lp(int n, double *A, double *b, double *c, double *result){
int rand_lp(int n, double A[][4], double b[], double c[], double result[]){
   int row = 4, col = 5;
   int steps = 0; //Keep track of recomputation steps
   int inequality1 = 0, inequality2 = 1, inequality3 = 2 , inequality4 = 3;
   double matrix_a[4][5], matrix_b[5], left_hand_eq;
   int i, j, p;

   for (i=0; i<4; i++){
      //Set all result to 60000
      result[i] = 600000;
   }


   for (i=row; i< n; i++){
      left_hand_eq = A[i][0]*result[0] + A[i][1]*result[1] + A[i][2]*result[2] + A[i][3]*result[3];


      if(left_hand_eq > b[i]){
         steps++;

         for (j=0; j<4; j++){
            //solving
            if (j == 0){
               for (p = 0; p < col; p ++){
                  if (p == 4){
                     matrix_a[0][p] = b[inequality2];
                     matrix_a[1][p] = b[inequality3];
                     matrix_a[2][p] = b[inequality4];
                     matrix_a[3][p] = b[i];
                     matrix_b[p] = b[inequality1];
                  }
                  else{
                     matrix_a[0][p] = A[inequality2][p];
                     matrix_a[1][p] = A[inequality3][p];
                     matrix_a[2][p] = A[inequality4][p];
                     matrix_a[3][p] = A[i][p];
                     matrix_b[p] = A[inequality1][p];
                  }
               }

               if (calculate_result(matrix_a) == -1){
                  if (update_matrix(matrix_b) == -1){
                     inequality1 = i;
                     //printf("[j=0] inequality1: %d\n", inequality1);
                     break;
                  }
               }
            }
            else if (j == 1){
               for (p = 0; p < col; p ++){
                  if (p == 4){
                     matrix_a[0][p] = b[inequality1];
                     matrix_a[1][p] = b[inequality3];
                     matrix_a[2][p] = b[inequality4];
                     matrix_a[3][p] = b[i];
                     matrix_b[p] = b[inequality2];
                  }
                  else{
                     matrix_a[0][p] = A[inequality1][p];
                     matrix_a[1][p] = A[inequality3][p];
                     matrix_a[2][p] = A[inequality4][p];
                     matrix_a[3][p] = A[i][p];
                     matrix_b[p] = A[inequality2][p];
                  }
               }

               if (calculate_result(matrix_a) == -1){
                  if (update_matrix(matrix_b) == -1){
                     inequality2 = i;
                     //printf("[j=1] inequality2: %d\n", inequality2);
                     break;
                  }
               }
            }
            else if (j == 2){
               for (p = 0; p < col; p ++){
                  if (p == 4){
                     matrix_a[0][p] = b[inequality1];
                     matrix_a[1][p] = b[inequality2];
                     matrix_a[2][p] = b[inequality4];
                     matrix_a[3][p] = b[i];
                     matrix_b[p] = b[inequality3];
                  }
                  else{
                     matrix_a[0][p] = A[inequality1][p];
                     matrix_a[1][p] = A[inequality2][p];
                     matrix_a[2][p] = A[inequality4][p];
                     matrix_a[3][p] = A[i][p];
                     matrix_b[p] = A[inequality3][p];
                  }
               }

               if (calculate_result(matrix_a) == -1){
                  if (update_matrix(matrix_b) == -1){
                     inequality3 = i;
                     //printf("[j=2] inequality3: %d\n", inequality3);
                     break;
                  }
               }
            }
            else if (j == 3){
               for (p = 0; p < col; p ++){
                  if (p == 4){
                     matrix_a[0][p] = b[inequality1];
                     matrix_a[1][p] = b[inequality2];
                     matrix_a[2][p] = b[inequality3];
                     matrix_a[3][p] = b[i];
                     matrix_b[p] = b[inequality4];
                  }
                  else{
                     matrix_a[0][p] = A[inequality1][p];
                     matrix_a[1][p] = A[inequality2][p];
                     matrix_a[2][p] = A[inequality3][p];
                     matrix_a[3][p] = A[i][p];
                     matrix_b[p] = A[inequality4][p];
                  }
               }
               if (calculate_result(matrix_a) == -1){
                  if (update_matrix(matrix_b) == -1){
                     inequality4 = i;
                     //printf("[j=3] inequality4: %d\n", inequality4);
                     break;
                  }
                  else{
                     inequality4 = i;
                     result[0] = 600000;
                     result[1] = 600000;
                     result[2] = 600000;
                     result[3] = 600000;
                     break;
                  }
               }
            }
         }
      }
   }
   return steps;
}

int main()
{  
   double A[600000][4],  b[600000], c[4] ;
   //double result[4];
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
   return 0;
}

