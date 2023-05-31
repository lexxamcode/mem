#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>

const int const_global_int = 1337;
int global_int = 100500;

void func()
{
    char* local_string = "crackme";
    int local_int = 42;
    float local_float = 3.14;
    double local_double = 2.12345678;
    while(true)
    {
//	system("clear");
	printf("PID: %d\n", getpid());
        printf("GLOBAL CONST INT: %d - %p\n"
               "GLOBAL INT: %d - %p\n"
               "LOCAL INT: %d - %p\n"
               "LOCAL string: %s - %p\n"
	       "LOCAL FLOAT: %f - %p\n"
	       "LOCAL DOUBLE: %lf - %p\n",
		const_global_int, &const_global_int, 
		global_int, &global_int,
		local_int, &local_int,
        	local_string, (void*)local_string,
		local_float, &local_float,
		local_double, &local_double);
        char decision = getchar();
	
	std::cout << "STRING CONTENT: ";
	size_t length = strlen(local_string);
        char bytes[length + 1];
	for (int i = 0; i < length; i++)
        {
            std::cout << std::hex << static_cast<int>(local_string[i]) << " - " << std::hex << (void*)(local_string) + i << std::endl;;
        }	
	std::cout << std::endl;

	if (decision == 'n')
		scanf("%d", &global_int);
    }
}

int main()
{
    func();
    return 0;
}
