// Kalrav Srivastava
// ks874

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>
#include <fcntl.h>

struct thrdInd
{
	float *map;
	int start;
	int last;
	float Sum;
};

void *sumthrd(void *arg)
{
	struct thrdInd *thrdInd = (struct thrdInd *)arg;
	float sum = 0;

	int start = thrdInd->start;
	int last = thrdInd->last;

	for(int i = start; i <= last; i++)
	{
		sum = sum + (thrdInd->map[i]);
	}

	thrdInd->Sum = sum;	
	pthread_exit(thrdInd);
}

int main(int argc, char *argv[])
{
	struct thrdInd **thrdInd = malloc(atoi(argv[1]) * sizeof(struct thrdInd));

	int fd = open(argv[2], O_RDWR);
	if (fd == -1)
	{
		perror("Can't open.");
		exit(EXIT_FAILURE);
	}

	//struct stat *fs
	
	int size = lseek(fd, (size_t)0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	int fNum = size/(sizeof(float));
	int div = fNum/(atoi(argv[1]));
	int remFlts = fNum % (atoi(argv[1]));

	float *map;
	map = mmap(0, size, PROT_READ, MAP_SHARED, fd, 0);
	if (map == MAP_FAILED)
	{
		close(fd);
		perror("Map error");
		exit(EXIT_FAILURE);
	}

	pthread_t thrd[atoi(argv[1])];
	int tc;

	for (int i = 0; i < atoi(argv[1]); i++)
	{
		struct thrdInd *curr = malloc(sizeof(struct thrdInd));
		
		curr->map = map;
		curr->start = 0 + (div * i);
		curr->last = (div * (i + 1)) - 1;
		
		if (i == atoi(argv[1]) - 1)
		{
			curr->last = (curr->last) + remFlts;
		}

		thrdInd[i] = curr;

		tc = pthread_create(&thrd[i], NULL, sumthrd, thrdInd[i]);
		if (tc != 0)
		{
			perror("Failed thread creation.");
		}
	}

	float SUM = 0;
	void *r;
	
	for (int i = 0; i < atoi(argv[1]); i++)
	{
		pthread_join(thrd[i], &r);
		struct thrdInd temp = *(struct thrdInd *)r;
		SUM = SUM + (temp.Sum);
	}

	printf("\nSum:\t%.6f\n", SUM);

	close(fd);
	free(thrdInd);

	pthread_exit(NULL);
}