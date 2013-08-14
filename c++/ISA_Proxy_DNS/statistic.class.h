/*******************************
 *
 * DNS Proxy
 * Author: Branislav Blaskovic
 * xblask00@stud.fit.vutbr.cz
 *
 * *****************************/

#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <list>
#include <time.h>
#include <map>

using namespace std;

#define HISTOGRAM_SIZE 24

typedef struct stat_type_struct
{
    int source;
    int destination;
    int type;
    int hour;
    int average;
    int histogram;
} TStat_type;

class Statistic
{
    time_t time_start;

    int number_of_hits;
    map <string, int> map_by_destination;
    map <string, int> map_by_source;
    map <string, int> map_by_type;

    map <time_t, map<string, int> > time_by_destination;
    map <time_t, map<string, int> > time_by_source;
    map <time_t, map<string, int> > time_by_type;
    int hits_by_time[HISTOGRAM_SIZE];

    public:
        Statistic();
        TStat_type stat_type;
        void add(string, string, string, string);
        void print_log();
        void print_map(string, map <string, int>);
        void print_map_last_hour(string, map <time_t, map<string, int> >);
        void print_map_histogram(string, int log[]);
        void print_map_average_hour(string, map <string, int>);
};
