#include <stdio.h>

int main(int argc  ,char** argv)
{
	#ifdef __x86_64__
		printf("__x86_64__");
	#elif __i386__
        printf("__i386__");
	#endif

	return 0;
}
