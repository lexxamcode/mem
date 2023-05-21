#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

const int const_global_int = 1337;
int global_int = 100500;

void func()
{
    char* local_string = "crackme";
    int local_int = 42;
    while(true)
    {
	system("clear");
	printf("PID: %d\n", getpid());
        printf("GLOBAL CONST INT: %d - %p\n"
               "GLOBAL INT: %d - %p\n"
               "LOCAL INT: %d - %p\n"
               "LOCAL CHAR*: %s - %p\n",
		const_global_int, &const_global_int, 
		global_int, &global_int, 
		local_int, &local_int,
        local_string, &local_string);
        char decision = getchar();

	if (decision == 'n')
		scanf("%d", &global_int);
    }
}

int main()
{
    func();
    return 0;
}
