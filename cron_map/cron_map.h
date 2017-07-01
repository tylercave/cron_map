#pragma once


#include <string>
#include <set>
#include <map>
#include "boost/icl/split_interval_map.hpp"
#include <boost/icl/ptime.hpp>
#include <ostream>
#include <ctime>
#include <regex>
#include <algorithm>
#include "boost\algorithm\string.hpp"
#include <exception>
#include <sstream>


using namespace boost::icl;
using namespace boost::posix_time;
namespace cave {
    namespace cron_map {
        struct cron_cmpts {
            boost::icl::interval_set<int> dow;
            boost::icl::interval_set<int> month;
            boost::icl::interval_set<int> dom;
            boost::icl::interval_set<int>hour;
            boost::icl::interval_set<int>minute;
            boost::icl::interval_set<int> seconds;
        };

        template<typename T>
        class cron_map
        {
            using seconds_map = split_interval_map <int, T, boost::icl::partial_enricher>;
            using minutes_map = split_interval_map<int, seconds_map, boost::icl::partial_enricher>;
            using hours_map = split_interval_map<int, minutes_map, boost::icl::partial_enricher>;
            using dom_map = split_interval_map<int, hours_map, boost::icl::partial_enricher>;
            using month_map = split_interval_map<int, dom_map, boost::icl::partial_enricher>;
            using dow_map = split_interval_map<int, month_map, boost::icl::partial_enricher>;
        public:
            cron_map();
            cron_map(T default_val);
            ~cron_map();

            void add(std::pair<cron_cmpts, T> cron_pair);
            void add(std::pair<std::string, T> cron_pair);
            void add(std::string cron_expr, T value);
            void add(cron_cmpts cmpts, T value);
            void remove(std::string cron_expr);
            void remove(cron_cmpts cmpts);

            inline T operator()(ptime time);
            friend std::ostream  & operator<<(std::ostream & os, cron_map<T> const & cm);
            friend  bool __cdecl operator==(class cron_map<T> const & cm_left, class cron_map<T> const & cm_right);

            cron_map<T> & operator+=(const cron_map<T>& classObj);
            cron_map<T> & operator+=(std::pair<std::string, T> cron_pair);
        private:
            T default_value{};

            seconds_map sm;
            minutes_map mm;
            hours_map hm;
            dom_map dm;
            month_map mn;
            dow_map wm;

            void init();
        };
    } // namespace cron_map
} // namespace cave
