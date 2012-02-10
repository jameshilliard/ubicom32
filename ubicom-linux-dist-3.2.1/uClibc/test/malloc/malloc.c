
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
 * This really needs to be determined by available system memory as on
 * systems without swap the test will fail, assume we have 16MB
 * available.
 */
#define N_PTRS 1000
#define N_ALLOCS 20000
#define MAX_SIZE 0x4000

#define random_size()	(random()%MAX_SIZE)
#define random_ptr()	(random()%N_PTRS)

int test1(void);
int test2(void);

int main(int argc, char *argv[])
{
	return test1() + test2();
}

int test1(void)
{
	void **ptrs;
	int i,j;
	int size;
	int ret = 0;

	srandom(0x19730929);

	ptrs = malloc(N_PTRS*sizeof(void *));

	for(i=0; i<N_PTRS; i++){
		size = random_size();
#if defined(__UCLIBC__) && !defined(__MALLOC_GLIBC_COMPAT__)
		if (!size) size = 1; /* Avoid allocating zero size, it
				      * will not work */
#endif
		if ((ptrs[i] = malloc(size)) == NULL) {
			printf("malloc random failed! %i\n", i);
			++ret;
		}
	}
	for(i=0; i<N_ALLOCS; i++){
		j = random_ptr();
		free(ptrs[j]);

		size = random_size();
#if defined(__UCLIBC__) && !defined(__MALLOC_GLIBC_COMPAT__)
		if (!size) size = 1; /* Avoid allocating zero size, it
				      * will not work */
#endif
		ptrs[j] = malloc(size);
		if (!ptrs[j]) {
			printf("malloc failed! %d, size %d\n", i, size);
			++ret;
		}
		memset(ptrs[j],0,size);
	}
	for(i=0; i<N_PTRS; i++){
		free(ptrs[i]);
	}

	return ret;
}

int test2(void)
{
	void *ptr = NULL;
	int ret = 0;

	ptr = realloc(ptr,100);
	if (!ptr) {
		printf("couldn't realloc() a NULL pointer\n");
		++ret;
	} else {
		free(ptr);
	}

	ptr = malloc(100);
	ptr = realloc(ptr, 0);
	if (ptr) {
		printf("realloc(,0) failed\n");
		++ret;
		free(ptr);
	}

	return ret;
}

