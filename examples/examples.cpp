//MIT License
//
//Copyright(c) 2017 Tyler Cave
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "stdafx.h"
#include "..\cron_map\cron_map.h"
#include <iostream>
#include <string>
using namespace cave::cron_map;
using namespace std;
int main()
{
    // create a cron map without specifying default value.  Will use default value for that type.
    cron_map<set<string>> cm(set<string>{"I am the default"});

    // print structure of cron map
    
    cout 
        << "Default tree structure:" << endl
        << cm << endl;

    // add a new value
    
    cm.add(format_cron_expr("* * * * jun-oct sun,sat"), set<string>{"Summer Weekend"});
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
    cm.add(cmpts, set<string>{"Waking Hours"});
    cout << "Value for Sun, July 2nd, 2017 22:00:" << cm(time_from_string("2017-07-02 22:00")) << endl;
    cout << "Value for Sun, July 2nd, 2017 23:00:" << cm(time_from_string("2017-07-02 23:00")) << endl;


    return 0;
}

