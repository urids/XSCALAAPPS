/*
 * PerformanceReults.h
 *
 *  Created on: Sep 9, 2016
 *      Author: uriel
 */

#pragma once

if (myRank == 0) {
	double Gflops;
	gettimeofday(&tval_after, NULL );
	timersub(&tval_after, &tval_before, &tval_result);
	double secs = (double) tval_result.tv_sec;
	double mils = (double) (tval_result.tv_usec) / 1000000;
	Gflops = (double) (2 * pow(size, 3)) / ((secs + mils) * pow(1024, 3));
	printf("time: %ld.%06ld\n", (long int) tval_result.tv_sec,
			(long int) tval_result.tv_usec);
	printf("performance: %f  Gflops", Gflops);

	FILE* resultsFp;
	resultsFp = fopen("resultsMtxMult.txt", "a");
	fprintf(resultsFp, "%f ",Gflops);
	fclose(resultsFp);

}
