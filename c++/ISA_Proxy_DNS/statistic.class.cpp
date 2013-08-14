/*******************************
 *
 * DNS Proxy
 * Author: Branislav Blaskovic
 * xblask00@stud.fit.vutbr.cz
 *
 * *****************************/

#include "statistic.class.h"

Statistic::Statistic()
{
    // Track the running time of program
    time(&time_start);

    // Set some default values
    stat_type.source = 0;
    stat_type.destination = 0;
    stat_type.type = 0;
    stat_type.average = 0;
    stat_type.hour = 0;
    stat_type.histogram = 0;

    number_of_hits = 0;

    int i;
    for(i = 0; i < HISTOGRAM_SIZE; i++)
    {
        hits_by_time[i] = 0;
    }
}

void Statistic::add(string domain, string ip, string type, string server_ip)
{
    time_t seconds;
    time(&seconds);
    struct tm *tm_time;
    tm_time = localtime(&seconds);

    // Print debug log
    cerr << "Request from " << ip << " for " << domain << " " << type << " using server '" << server_ip << "'" << endl;

    // Add to database
    // depends on arguments
    number_of_hits++;
    hits_by_time[tm_time->tm_hour]++;
    if(stat_type.destination)
    {
        map_by_destination[domain]++;
        time_by_destination[seconds][domain]++;
    }
    if(stat_type.source)
    {
        map_by_source[ip]++;
        time_by_source[seconds][ip]++;
    }
    if(stat_type.type)
    {
        map_by_type[type]++;
        time_by_type[seconds][type]++;
    }
}

void Statistic::print_map(string header, map <string, int> log)
{
    map <string, int>::iterator it;

    cout << "|----------------------------|" << endl;
    cout <<  header << endl;

    // Loop through log and print it    
    for(it = log.begin(); it != log.end(); it++)
    {
        printf("| %4d x | %s\n", it->second, it->first.c_str());
    }
}

void Statistic::print_map_last_hour(string header, map <time_t, map<string, int> > log)
{
    map <time_t, map<string, int> >::iterator it;
    map <string, int>::iterator it2;
    int hour;
    struct tm *aa;
    time_t now;
    time(&now);

    map <string, int> results;

    cout << "|----------------------------|" << endl;
    cout <<  header << endl;

    // Filter the last ones
    for(it = log.begin(); it != log.end(); it++)
    {
        // 3600 seconds ago? (lats hour)
        if(it->first < (now - 3600)) continue;

        for(it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
        {
            // Add to finall
            results[it2->first] += it2->second;
        }
    }

    // Write it
    for(it2 = results.begin(); it2 != results.end(); it2++)
    {
        printf("| %4d x | %s\n", it2->second, it2->first.c_str());
    }
}

void Statistic::print_map_average_hour(string header, map <string, int> log)
{
    map <string, int>::iterator it;
    time_t now;
    double average;
    time(&now);

    cout << "|----------------------------|" << endl;
    cout <<  header << endl;
    
    for(it = log.begin(); it != log.end(); it++)
    {
        average = it->second * 3600.0 / (now - time_start);
        printf("| %5.1f x | %s\n", average, it->first.c_str());
    }
}

void Statistic::print_map_histogram(string header, int log[HISTOGRAM_SIZE])
{
    int i, a;
    double hits = 0;
    struct tm *aa;
    time_t now;
    double help = 0.0, percent = 0.0;
    time(&now);

    cout << "|----------------------------|" << endl;
    cout <<  header << endl;

    // Count all hits
    //for(i = 0; i < HISTOGRAM_SIZE; i++)
    //{
    //    hits += log[i];
    //}

    // Print it
    for(i = 0; i < HISTOGRAM_SIZE; i++)
    {
        // Calculate it
        // help = 1.0 * (now - time_start) / (60 * 60 * 24);
        
        // If not 0 hits
        percent = (log[i] > 0) ? (log[i] * 100.0 / number_of_hits) : 0.0;

        // Print it
        printf("| %2d:00 | ", i);

        // Make progressbar
        for(a = 0; a < floor(percent / 10); a++)
        {
            printf("+");
        }
        for(a; a < 10; a++)
        {
            printf(".");
        }

        // Print per cent
        printf(" | ~ %3d%%", (int)floor(percent));
        printf("\n");
    }
}

void Statistic::print_log()
{
    // Print the sum up
    cout << "|----------------------------|" << endl;
    cout << "| Total requests: " << number_of_hits << "x" << endl;

    // Need statistics by destination?
    if(stat_type.destination)
    {
        print_map("| Statistics by destination", map_by_destination);
        if(stat_type.hour) print_map_last_hour("| Statistics by destination\n| - for last hour", time_by_destination);
        if(stat_type.average) print_map_average_hour("| Statistics by destination\n| - average for hour", map_by_destination);
    }

    // Need statistics by source?
    if(stat_type.source)
    {
        print_map("| Statistics by source", map_by_source);
        if(stat_type.hour) print_map_last_hour("| Statistics by source\n| - for last hour", time_by_source);
        if(stat_type.average) print_map_average_hour("| Statistics by source\n| - average for hour", map_by_source);
    }

    // Need statistics by type?
    if(stat_type.type)
    {
        print_map("| Statistics by type", map_by_type);
        if(stat_type.hour) print_map_last_hour("| Statistics by type\n| - for last hour", time_by_type);
        if(stat_type.average) print_map_average_hour("| Statistics by type\n| - average for hour", map_by_type);
    }
    if(stat_type.histogram)
    {
        print_map_histogram("| Histogram", hits_by_time);
    }

    cout << "|----------------------------|" << endl;
}
