
#include "../src/mapper_internal.h"
#include <mapper/mapper.h>
#include <stdio.h>
#include <math.h>
#include <lo/lo.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#ifdef WIN32
#define usleep(x) Sleep(x/1000)
#endif

#define eprintf(format, ...) do {               \
    if (verbose)                                \
        fprintf(stdout, format, ##__VA_ARGS__); \
} while(0)

int done;

void ctrlc(int signal)
{
    done = 1;
}

void input_handler ( mapper_signal msig,
                              mapper_db_signal props,
                              int instance_id,
                              void *value,
                              int count,
                              mapper_timetag_t *tt )
{

    printf("received val = %i\n",*(int*)value);
}

void handler(mapper_signal sig, mapper_db_signal props,
             int instance_id, void *value, int count,
             mapper_timetag_t *timetag)
{
    if (value) {
        printf("handler: Got %f\n", (*(float*)value));
    }
}

int main(int argc, char **argv)
{
    done = 0;
    signal(SIGINT, ctrlc); //install handler

    printf("init device\n");
    mapper_device mydev = mdev_new("testrecv", 9000,0);
    int min_i=0, max_i = 10;
    mapper_signal testInput =
        mdev_add_input(mydev, "/testIn", 1, 'i', 0, &min_i, &max_i, input_handler, 0);
    
    float mn=0, mx=1;
    mapper_signal sig =
    mdev_add_input(mydev, "/mapped1", 1, 'f', 0, &mn, &mx, handler, 0);
    
    printf("registered input /testIn\n");
    
    while (!mdev_ready(mydev)) {
        printf("waiting for device...\n");
        mdev_poll(mydev,500);
    }
    
    printf("device ready!\n");
    
    while (!done) {
        mdev_poll(mydev, 50);
    }
    printf("\n\nfreeing device\n");
    mdev_free(mydev);
    
}
