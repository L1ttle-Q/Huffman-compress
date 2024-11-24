#include "huffman.h"
#include "decompress.h"
#include "Util.h"

bool echoDict = false;

int main()
{
    int op;
    Huffman* sol;
    Huffman_Decompress* decompress;
    FILE* fp_in;
    FILE* fp_out;
    do
    {
        printf("1: compress file\n");
        printf("2: decompress file\n");
        printf("3: (dis/en)able dict echo\n");
        printf("4: exit\n");
        int err = scanf("%d", &op);
        if (!err)
        {
            printf("error input!\n");
            continue;
        }
        if (!~err) exit(0);

        switch (op)
        {
        case 1:
            fp_in = getInputFile(binary);
            fp_out = getOutputFile(binary);
            sol = new Huffman(fp_in, fp_out);
            sol->Solve();
            if (echoDict) sol->Print_Dict();
            delete sol;
            break;

        case 2:
            fp_in = getInputFile(binary);
            fp_out = getOutputFile(binary);
            decompress = new Huffman_Decompress(fp_in, fp_out);
            decompress->Solve();
            if (echoDict) decompress->Print_Dict();
            delete decompress;
            break;

        case 3:
            echoDict = !echoDict;
            break;

        case 4:
            exit(0);
            break;

        default:
            printf("error input!\n");
            continue;
        }

    } while(op);
    return 0;
}
