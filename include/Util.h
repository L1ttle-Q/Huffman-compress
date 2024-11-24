#ifndef ___UTIL_H__
#define ___UTIL_H__

#include <cstdio>
#include <iostream>

#define size_t uint32_t

enum Method{null=0, binary=1};

FILE* getInputFile(Method op=null)
{
    FILE* fp;
    bool Err = false;
    char input_file[50];

    do
    {
        Err = false;
        fprintf(stdout, "Input file name(input file):");
        scanf("%s", input_file);
        if (op == binary) fp = fopen(input_file, "rb");
        else fp = fopen(input_file, "r");
        if (!fp)
        {
            Err = true;
            fprintf(stdout, "Fail to read this file!\n");
        }
        else return fp;
    } while(Err);
    return fp;
}

FILE* getOutputFile(Method op=null)
{
    FILE* fp;
    bool Err = false;
    char output_file[50];

    do
    {
        Err = false;
        fprintf(stdout, "Input file name(output file):");
        scanf("%s", output_file);
        if(op == binary) fp = fopen(output_file, "wb");
        else fp = fopen(output_file, "w");
        if (!fp)
        {
            Err = true;
            fprintf(stdout, "Fail to write this file!\n");
        }
        else return fp;
    } while(Err);
    return fp;
}

template<typename T>
void Write_binary(T s, FILE* out)
{
    u_char tmp = '\0';
    int j = 0;
    for (auto c: s)
    {
        tmp = tmp << 1 | (c == '1');
        j++;
        if (j & 8)
        {
            fwrite(&tmp, sizeof(u_char), 1, out);
            j = 0;
        }
    }
    if (j)
    {
        tmp <<= 8 - j;
        fwrite(&tmp, sizeof(u_char), 1, out);
    }
}

void Write_binary_stream(std::string s, FILE* out)
{
    static u_char tmp = '\0';
    static int j = 0;

    if (s.empty() && j)
    {
        tmp <<= 8 - j;
        fwrite(&tmp, sizeof(u_char), 1, out);
        tmp = 0; j = 0;
    }

    for (auto c: s)
    {
        tmp = tmp << 1 | (c == '1');
        j++;
        if (j & 8)
        {
            fwrite(&tmp, sizeof(u_char), 1, out);
            j = 0;
        }
    }
}

char Read_binary(FILE* in, bool skip = false)
{
    static u_char c = '\0';
    static int j = -1;

    if (skip) {j = -1; return 0;}
    if (!~j)
    {
        if (!fread(&c, sizeof(u_char), 1, in)) return EOF;
        j = 7;
    }
    return (c >> (j--)) & 1;
}

#endif