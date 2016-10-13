#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char const *argv[])
{
    if(argc < 2) {
        printf("usage: ./random_gen [Number]\n");
        return -1;
    }

    int num = atoi(argv[1]);
    FILE *fp = fopen("random", "w+");

    srandom(time(NULL));
    for(int i = 0; i < num; i++) {
        fprintf(fp, "%d\n", rand());
    }
    fclose(fp);

    return 0;
}