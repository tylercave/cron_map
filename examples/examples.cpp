// examples.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\cron_map\cron_map.h"
#include <iostream>
#include <string>
using namespace cave::cron_map;
using namespace std;
int main()
{
    // create a cron map without specifying default value.  Will use default value for that type.
    cron_map<double> cm;

    // print structure of cron map
    
    cout 
        << "Default tree structure:" << endl
        << cm << endl;

    // add a new value
    
    cm.add(format_cron_expr("* * * * jun-oct sun,sat"),3.14);
    cout << "New tree structure:" << endl
        << cm << endl;
    
    cout << "Value for Sat, July 1st, 2017:" << cm(time_from_string("2017-07-01 00:00")) << endl;
    cout << "Value for Sun, July 2nd, 2017:" << cm(time_from_string("2017-07-02 00:00")) << endl;
    cout << "Value for Mon, July 3rd, 2017:" << cm(time_from_string("2017-07-03 00:00")) << endl;
    // remove a pattern from the cron map
    cm.remove(format_cron_expr("* * * * * sat"));   // create a cron map with a default value
    cout << "New tree structure:" << endl
        << cm << endl;
    cout << "Value for Sat, July 1st, 2017:" << cm(time_from_string("2017-07-01 00:00")) << endl;
    cout << "Value for Sun, July 2nd, 2017:" << cm(time_from_string("2017-07-02 00:00")) << endl;
    cout << "Value for Mon, July 3rd, 2017:" << cm(time_from_string("2017-07-03 00:00")) << endl;
    // create a cron_cmpts

    auto cmpts = convert_to_cron_cmpts("* * 7-22 * * *");
    // add value to cron map with cron_cmpts
    cout << "Adding from cmpts:\n";
    cm.add(cmpts, 777);
    cout << "Value for Sun, July 2nd, 2017 22:00:" << cm(time_from_string("2017-07-02 22:00")) << endl;
    cout << "Value for Sun, July 2nd, 2017 23:00:" << cm(time_from_string("2017-07-02 23:00")) << endl;
    return 0;
}

