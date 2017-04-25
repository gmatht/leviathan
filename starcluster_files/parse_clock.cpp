#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int use_float=getenv("PC_FLOAT")>0; 
int round_robin=getenv("PC_RR")>0; 
int pc_max=getenv("PC_MAX")>0; 
int pc_sum=getenv("PC_SUM")>0; 

unsigned int num_jobs=0;

int inline to_job (int i) {
	int m = (i % num_jobs);
	int r = (i / num_jobs);
	if (r) {
		srand(r); /*set random seed*/
		return((rand()+m)%num_jobs);
	}
	else {
		return m;
	}
}

void inline print_time (unsigned long long t) {
	if (use_float)
		printf("%lf\n",(double)t/(1000*1000));
	else
		printf("%lld\n",t);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("USAGE: zcat bench.U0_0001.gz | %s [split_depth]:[num_jobs]\n",argv[0]);
		return 1;
	}
	unsigned int split_depth=20;

	unsigned int job=0;
	unsigned long long *job_clock;

	sscanf(argv[1],"%u:%u",&split_depth,&num_jobs);
	unsigned int depth=0, l_depth=0;
	unsigned long long time=0, l_time=0;
	unsigned long long max=0;
	double sum;
	
	int n_read;

	job_clock=(unsigned long long*)calloc(num_jobs,sizeof(unsigned long long));

	while ((n_read = scanf("^%u:%llu\n", &depth, &time)) >= 0) {

		// swallow lines not of the right format
		if (n_read < 2) {
			scanf("%*[^\n]");
			scanf("\n");
			//printf("----\n");
			continue;
		}

		if (depth <= split_depth) {
			if (l_depth == split_depth) {
				long long d_time = (time-l_time);
				if (num_jobs) {
					if (d_time > max) max = d_time;
					if (round_robin) {
						job_clock[job++]+=d_time;
						job %= num_jobs;
					} else {
						job_clock[to_job(job++)]+=d_time;
					}
				} else {
					print_time(d_time);
				}
			}
			l_depth=depth;
			l_time =time ;
		}

		//end of one formulas benchmark
		if (depth==0) {
			if (num_jobs)
				for (unsigned int i=0;i<num_jobs;i++) {
					if (job_clock[i] > max) max = job_clock[i];
					if (!(pc_max||pc_sum)) {
						print_time(job_clock[i]);
					}
			}
			if (pc_max) 
				print_time(max);
			sum+=(double)max;

			job=0; depth=0; l_depth=0; time=0; l_time=0; max=0;
			memset(job_clock, 0, num_jobs*sizeof(unsigned long long));
			continue;
		}

	}

	free(job_clock);

	if (depth) {
		fprintf (stderr, "Missing '^0' at end of input?");
		return 1;
	} else {
		if (pc_sum)
                	printf("\t%4.2lf\n",sum/(1000*1000));
	}



	return 0;
}
