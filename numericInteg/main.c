#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define Pi 3.14159264
#define epsilon 0.00001

int n;

int i, j; //index exclusive


//double a;
//double b;

double T, Thalf;
double err;

double f(double x) {
	return sin(x);
}

double integTask(double (*f)(double), double a, double b, double eps, int n) {

	double h;
	double halfh;
	double halfn = floor(n / 2) + 1;
	//n = 2 * halfn - 1;

	h = (b - a) / halfn;
	halfh = (h / 2);


	//printf("halfn:= %d , n:= %d \n ", halfn,n );
	printf("a:= %f , b:= %f \n ", a,b);
	printf("halfh:= %f , h:= %f \n ", halfh, h);

	double sum = 0;
	double sum2 = 0;

	for (i = 1; i < halfn; i++) {
		sum += f(a + i * h);
	}

	Thalf = (halfh) * (f(a) + 2 * sum + f(b));

	for (i = 1; i < halfn; i++) {
		sum2 += f(a + (2 * i - 1) * halfh);
	}

	T = (Thalf / 2) + halfh * sum2;


	err = fabs(T - Thalf) / 3;
	printf("%9.8f %9.8f \n", Thalf, T);
	printf(" error: %9.8f \n", err);


	if(err<eps){
		return T;
	}else{
		double T1=integTask(f,a,(a+b)/2,eps/2,2*n);
		double T2=integTask(f,(a+b)/2,b,eps/2,2*n);
		return T1+T2;
	}
}



int main(int argc, char** argv) {
	if (argc != 2) {
		printf("missing args \n");
		return -1;

	}

	double a = 0;
	double b = Pi;

	n = atoi(argv[1]);

	double Integral=integTask(f,a,b,epsilon,n);
	printf("integral= %9.8f ",Integral);

	//printf("%d--%f   %d--%f \n",halfn ,h, n,  halfh);
	return 0;
}
