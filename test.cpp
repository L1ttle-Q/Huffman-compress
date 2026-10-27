#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <chrono>

const int MaxFileNameLength = 50,
          MaxCommandLength = 150;

int main(int argc, char* argv[])
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
    #ifdef _WIN32
    const char* default_executable = "build\\huffman.exe";
    #else
    const char* default_executable = "build/huffman";
    #endif
    snprintf(command_1, MaxCommandLength, "\"%s\" < %s", argc > 1 ? argv[1] : default_executable, test_file);

    auto start = std::chrono::high_resolution_clock::now();

    if (system(command_1) != 0) return 1;

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "\nElapsed time: " << elapsed.count() << " ms" << std::endl;

    char command_2[MaxCommandLength];
    #ifdef _WIN32
    for (char* p = in_file; *p; ++p) if (*p == '/') *p = '\\';
    for (char* p = decom_file; *p; ++p) if (*p == '/') *p = '\\';
    snprintf(command_2, MaxCommandLength, "fc /b \"%s\" \"%s\"", in_file, decom_file);
    #else
    snprintf(command_2, MaxCommandLength, "diff %s %s && echo \"Files are identical.\"", in_file, decom_file);
    #endif

    return system(command_2) == 0 ? 0 : 1;
}
