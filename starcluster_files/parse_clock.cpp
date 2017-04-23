#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("USAGE: zcat bench.U0_0001.gz | %s [split_depth]:[num_jobs]\n",argv[0]);
		return 1;
	}
	unsigned int split_depth=20;
	unsigned int num_jobs=0;

	unsigned int job=0;
	unsigned long long *job_clock;

	sscanf(argv[1],"%u:%u",&split_depth,&num_jobs);
	unsigned int depth=0, l_depth=0;
	unsigned long long time=0, l_time=0;
	int n_read;

	job_clock=(unsigned long long*)calloc(num_jobs,sizeof(unsigned long long));

	while ((n_read = scanf("^%u:%llu\n", &depth, &time)) >= 0) {
		if (n_read < 2) {
			scanf("%*[^\n]");
			scanf("\n");
			//printf("----\n");
			continue;
		}
		//printf("%d\t%u\t%llu\n",n_read,depth,time);
		if (depth <= split_depth) {
			if (l_depth == split_depth) {
				long long d_time = (time-l_time);
				if (num_jobs) {
					job_clock[job++]+=d_time;
					job %= num_jobs;
				} else {
					printf("%lld\n",d_time);
				}
			}
			l_depth=depth;
			l_time =time ;
		}
	}

	if (num_jobs)
		for (unsigned int i=0;i<num_jobs;i++) 
			printf("%lld\n",job_clock[i]);
	return 0;
}
