#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <chrono>

const int MaxFileNameLength = 50,
          MaxCommandLength = 150;

int main()
{
    fprintf(stdout, "Test num:");
    int num = 0;
    scanf("%d", &num);
    char test_file[MaxFileNameLength],
         in_file[MaxFileNameLength],
         decom_file[MaxFileNameLength];

    snprintf(test_file, MaxFileNameLength, "test/%d/test.txt", num);
    snprintf(in_file, MaxFileNameLength, "test/%d/in.txt", num);
    snprintf(decom_file, MaxFileNameLength, "test/%d/decom.txt", num);

    FILE *fp = fopen(test_file, "r");
    if (!fp)
    {
        fprintf(stdout, "Fail to read the test file!\n");
        return 1;
    }
    else fclose(fp);

    char command_1[MaxCommandLength];
    snprintf(command_1, MaxCommandLength, "make/huffman < %s", test_file);

    auto start = std::chrono::high_resolution_clock::now();

    system(command_1);

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "\nElapsed time: " << elapsed.count() << " ms" << std::endl;

    char command_2[MaxCommandLength];
    #ifdef _WIN64
    snprintf(command_2, MaxCommandLength, "fc %s %s", in_file, decom_file);
    #else
    snprintf(command_2, MaxCommandLength, "diff %s %s && echo \"Files are identical.\"", in_file, decom_file);
    #endif

    system(command_2);

    return 0;
}