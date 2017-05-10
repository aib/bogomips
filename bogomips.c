#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include <pthread.h>
#include <unistd.h>

#define MAX_THREADS 1024
#define REPORT_DELAY 1000000UL

#define SUB(a, b, mod) (((a) < (b)) ? (((mod) - (b)) + (a) + 1) : ((a) - (b)))

struct thread_data {
	int num;
	volatile unsigned long long value;
	volatile unsigned long long last_seen_value;
};

void *thread_start(void *);

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage:\n\t%s <threads>\n\n", argv[0]);
		return 1;
	}

	int N = atoi(argv[1]);

	if (N < 1 || N > MAX_THREADS) {
		fprintf(stderr, "Number of threads must be between 1 and %d\n", MAX_THREADS);
		return 1;
	}

	pthread_t *threads = malloc(sizeof(pthread_t) * N);
	struct thread_data *thread_data = malloc(sizeof(struct thread_data) * N);
	if (threads == NULL || thread_data == NULL) {
		fprintf(stderr, "Unable to allocate memory\n");
		return 2;
	}

	for (int i = 0; i < N; ++i) {
		thread_data[i].num = i;
		thread_data[i].value = 0;
		thread_data[i].last_seen_value = 0;
		pthread_create(&threads[i], NULL, thread_start, &thread_data[i]);
	}

	while (1) {
		usleep(REPORT_DELAY);

		unsigned long mips = 0;
		for (int i = 0; i < N; ++i) {
			unsigned long long dv = SUB(thread_data[i].value, thread_data[i].last_seen_value, ULLONG_MAX);
			thread_data[i].last_seen_value = thread_data[i].value;
			mips += dv / REPORT_DELAY;
		}

		printf("%lu\n", mips);
	}

	return 0;
}

void *thread_start(void *data)
{
	struct thread_data *td = (struct thread_data*) data;

	while (1) {
		++td->value;
	}
}
