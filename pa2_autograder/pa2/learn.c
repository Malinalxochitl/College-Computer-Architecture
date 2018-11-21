#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int M, K, N;

void multiplyX(int a, int b, int c, double first[a][b],double second[b][c], double result[a][c]){
	int i, j, k; 
	double temp;
	for (i = 0;i < a;i++) { //row i
		for (j = 0;j < c;j++) { //column j
			temp = 0;
			for (k = 0;k < b;k++) {
				temp += first[i][k] * second[k][j];
			}
			result[i][j] = temp;
		}
	}
}

void multiplyY(int a, int b, double first[a][b], double second[b], double third[a]) {
	double result[a];
	double temp;
	int i, j;
	for (i = 0;i < a;i++) {
		temp = 0;
		for (j = 0;j < b;j++) {
			temp += first[i][j] * second[j];
		}
		result[i] = temp;
	}
	memcpy(third, result, sizeof(result));
}

void transpose(int a, int b, double orig[a][b], double result[b][a]) {
	int i, j;
	for (i = 0;i < a;i++) {
		for (j = 0;j < b;j++) {
			result[j][i] = orig[i][j];
		}
	}
}

void inverse(int a, int b, double matrix[a][b]) {
	double identity[a][b];
	int i, j;
	for (i = 0;i < a;i++) {
		for (j = 0;j < b;j++) {
			if(i == j) {
				identity[i][j] = 1;
			} else {
				identity[i][j] = 0;
			}
		}
	}
	for (j = 0;j < b;j++) {
		if (matrix[j][j] != 1) {
			double num = matrix[j][j];
			int k;
			for (k = 0;k < b;k++) {
				matrix[j][k] = matrix[j][k] / num;
				identity[j][k] = identity[j][k] / num;
			}
		}
		for (i = j+1;i < a;i++) {
			if(matrix[i][j] != 0) {
				int k = 0;
				double num = matrix[i][j];
				for (k = 0;k < b;k++) {
					matrix[i][k] = matrix[i][k] - (matrix[j][k] * num);
					identity[i][k] = identity[i][k] - (identity[j][k] * num);
				}
			}
		}
	}
	for (j = b-1;j >= 0;j--) {
		for (i = j-1;i >= 0;i--) {
			if(matrix[i][j] != 0) {
				int k = 0;
				double num = matrix[i][j];
				for (k = 0;k < b;k++) {
					matrix[i][k] = matrix[i][k] - (matrix[j][k] * num);
					identity[i][k] = identity[i][k] - (identity[j][k] * num);
				}
			}
		}
	}
	memcpy(matrix, identity, sizeof(identity));
}

void print(int a, int b, double matrix[a][b]) {
	int i, j;
	for (i = 0;i < a;i++) {
		for (j = 0;j < b;j++) {
			printf("%f ", matrix[i][j]);
		}
		printf("\n");
	}
}

int main(int argc, char** argv) {
	FILE *fp = fopen(argv[1], "r"); //open the training data
	int i, j;
	fscanf(fp, "%d", &K);//number of attributes
	fscanf(fp, "%d", &N);//number of entries in training data
	double X[N][K+1];
	double XT[K+1][N];
	double XI[K+1][K+1];
	double Y[N];
	double W[K+1];
	for (i = 0;i < N;i++) {
		X[i][0] = 1;
		for (j = 1;j <= K;j++) {
			fscanf(fp, "%lf%*c[^\n]", &X[i][j]); //XI is currently X
		}
		fscanf(fp, "%lf%*c[^\n]", &Y[i]);
	}
	fclose(fp);
	fp = fopen(argv[2], "r"); //open the test data
	fscanf(fp, "%d", &M);//number of entries in test data
	double test[M][K];
	for (i = 0;i < M;i++) {
		for (j = 0;j < K;j++) {
			fscanf(fp, "%lf%*c[^\n]", &test[i][j]);
		}
	}
	fclose(fp);
	transpose(N, K+1, X, XT);
	multiplyX(K+1, N, K+1, XT, X, XI);
	inverse(K+1, K+1, XI);
	multiplyY(K+1, N, XT, Y, W);
	multiplyY(K+1, K+1, XI, W, W);
	for (i = 0; i < M;i++) {
		double price = W[0];
		for (j = 1;j < K+1;j++) {
			price = price + (W[j] * test[i][j-1]);
		}
		printf("%0.0lf\n", price);
	}
	return 0;
}
