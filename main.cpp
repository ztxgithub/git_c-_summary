#include <stdio.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <queue>
#include <stdlib.h>
#include <memory>
#include <string>
#include <iostream>
#include <sys/time.h>
using namespace std;

static long getcurrenttick()
{
    long tick ;
    struct timeval time_val;
    gettimeofday(&time_val , NULL);
    tick = time_val.tv_sec * 1000 + time_val.tv_usec / 1000 ;
    return tick;
}


int main(int argc, char *argv[]) {

    string the_base(1024 * 1024 * 10, 'x');
    long begin =  getcurrenttick();
    for( int i = 0 ;i< 100 ;++i ) {
        string the_copy = the_base;
//        the_copy[0] = 'y';
    }
    fprintf(stdout,"耗时[%d] \n",getcurrenttick() - begin );
    return 0;
}