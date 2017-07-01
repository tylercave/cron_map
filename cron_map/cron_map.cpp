#include "cron_map.h"

using namespace boost::icl;
using namespace boost::posix_time;
namespace cave {
    namespace cron_map {

        static const discrete_interval<int> seconds_interval = interval<int>::closed(0, 59);
        static const discrete_interval<int> minutes_interval = interval<int>::closed(0, 59);
        static const discrete_interval<int> hours_interval = interval<int>::closed(0, 23);
        static const discrete_interval<int> dom_interval = interval<int>::closed(1, 31);
        static const discrete_interval<int> month_interval = interval<int>::closed(1, 12);
        static const discrete_interval<int> dow_interval = interval<int>::closed(1, 7);

        static const std::string month_names_string = "(JAN|JANUARY|FEB|FEBRUARY|MAR|MARCH|APR|APRIL|MAY|JUN|JUNE|JUL|JULY|AUG|AUGUST|SEP|SEPTEMBER|OCT|OCTOBER|NOV|NOVEMBER|DEC|DECEMBER)";
        static const std::string dow_names_string = "(SUN|SUNDAY|MON|MONDAY|TUE|TUESDAY|WED|WEDNESDAY|THU|THURSDAY|FRI|FRIDAY|SAT|SATURDAY)";

        static const std::string seconds_minutes_string = "[1-5]?[0-9](-[1-5]?[0-9])?";
        static const std::string hour_string = "(([1]?[0-9])|(2[0-3]))(-(([1]?[0-9])|(2[0-3])))?";
        static const std::string dom_string = "(([1-2]?[1-9])|(3[0-1]))(-(([1-2]?[0-9])|(3[0-1])))?";
        static const std::string month_string =
            std::regex_replace(
                "([1-9]|(1[0-2])(-([1-9]|(1[0-2])))?)|(MONTH_NAME(-MONTH_NAME)?)",
                std::regex("MONTH_NAME"),
                month_names_string
            );

        static const std::string dow_string =
            std::regex_replace(
                "([1-7](-[1-7])?)|(DOW_NAME(-DOW_NAME)?)",
                std::regex("DOW_NAME"),
                dow_names_string
            );

        static  inline const std::string expand_cron_cmp(std::string cron_cmpt) {
            return std::regex_replace(
                "\\*|CRON_CMP([,](CRON_CMP))*",
                std::regex("CRON_CMP"),
                cron_cmpt
            );
        };

        template<typename T>
        static inline bool is_valid(const  boost::icl::interval_set<T>  cron_cmpt, discrete_interval<T> bounds) {
            return !cron_cmpt.empty() && boost::icl::first(*cron_cmpt.begin()) >= bounds.lower() && boost::icl::last(*cron_cmpt.rbegin()) <= bounds.upper();
        };
        static inline bool is_valid(const cron_cmpts & cmpts) {
            return  is_valid(cmpts.dow, dow_interval) &&
                is_valid(cmpts.month, month_interval) &&
                is_valid(cmpts.dom, dom_interval) &&
                is_valid(cmpts.hour, hours_interval) &&
                is_valid(cmpts.minute, minutes_interval) &&
                is_valid(cmpts.seconds, seconds_interval);

        }
        static discrete_interval<int> str_to_interval(const std::string str, const discrete_interval<int> all) {
            std::istringstream ss(str);
            std::string first;
            std::string last;
            std::vector<std::string> str_cmpts;
            std::getline(ss, first, '-');
            std::getline(ss, last, '-');
            if (last.empty()) {
                if (first == "*") {
                    return all;
                }
                else {
                    return  discrete_interval<int>::closed(std::stoi(first), std::stoi(first));
                }
            }
            else {
                return  discrete_interval<int>::closed(std::stoi(first), std::stoi(last));
            }
        }

        static boost::icl::interval_set<int> convert_string_to_cron_cmpt(const std::string cmpt, const discrete_interval<int> all) {
            std::istringstream ss(cmpt);
            std::string token;
            std::vector<std::string> str_cmpts;

            boost::icl::interval_set<int> intervals;
            while (std::getline(ss, token, ',')) {
                intervals.add(str_to_interval(token, all));
            }
            return  intervals;
        }
        static std::string & convert_month_dow_to_int(std::string &cron_expr) {
            cron_expr = std::regex_replace(cron_expr, std::regex("JAN|JANUARY|SUN|SUNDAY"), "1");
            cron_expr = std::regex_replace(cron_expr, std::regex("FEB|FEBRUARY|MON|MONDAY"), "2");
            cron_expr = std::regex_replace(cron_expr, std::regex("MAR|MARCH|TUE|TUESDAY"), "3");
            cron_expr = std::regex_replace(cron_expr, std::regex("APR|APRIL|WED|WEDNESDAY"), "4");
            cron_expr = std::regex_replace(cron_expr, std::regex("MAY|THU|THURSDAY"), "5");
            cron_expr = std::regex_replace(cron_expr, std::regex("JUN|JUNE|FRI|FRIDAY"), "6");
            cron_expr = std::regex_replace(cron_expr, std::regex("JUL|JULY|SAT|SATURDAY"), "7");
            cron_expr = std::regex_replace(cron_expr, std::regex("AUG|AUGUST"), "8");
            cron_expr = std::regex_replace(cron_expr, std::regex("SEP|SEPTEMBER"), "9");
            cron_expr = std::regex_replace(cron_expr, std::regex("OCT|OCTOBER"), "10");
            cron_expr = std::regex_replace(cron_expr, std::regex("NOV|NOVEMBER"), "11");
            cron_expr = std::regex_replace(cron_expr, std::regex("DEC|DECEMBER"), "12");
            return cron_expr;
        };

        static cron_cmpts  convert_to_cron_cmpts(std::string cron_expr) {
            cron_cmpts cmpts;
            std::stringstream cron_expr_stringstream;
            cron_expr_stringstream << "^("
                << expand_cron_cmp(seconds_minutes_string) << ")\\s("
                << expand_cron_cmp(seconds_minutes_string) << ")\\s("
                << expand_cron_cmp(hour_string) << ")\\s("
                << expand_cron_cmp(dom_string) << ")\\s("
                << expand_cron_cmp(month_string) << ")\\s("
                << expand_cron_cmp(dow_string)
                << ")$";
            const std::string cron_expr_string{ cron_expr_stringstream.str() };
            if (!std::regex_match(cron_expr, std::regex(cron_expr_string))) {
                throw std::invalid_argument("invalid cron expression");
            }
            std::string new_cron_expr(cron_expr);
            new_cron_expr = convert_month_dow_to_int(new_cron_expr);
            std::istringstream ss(new_cron_expr);
            std::string token;
            std::vector<std::string> str_cmpts;
            for (std::string token; ss >> token;) {
                str_cmpts.push_back(token);
            }
            cmpts.dow = convert_string_to_cron_cmpt(str_cmpts[5], dow_interval);
            cmpts.month = convert_string_to_cron_cmpt(str_cmpts[4], month_interval);
            cmpts.dom = convert_string_to_cron_cmpt(str_cmpts[3], dom_interval);
            cmpts.hour = convert_string_to_cron_cmpt(str_cmpts[2], hours_interval);
            cmpts.minute = convert_string_to_cron_cmpt(str_cmpts[1], minutes_interval);
            cmpts.seconds = convert_string_to_cron_cmpt(str_cmpts[0], seconds_interval);

            return cmpts;

        };

        static inline std::string format_cron_expr(const std::string cron_expr) {
            std::string new_cron_expr(boost::algorithm::to_upper_copy(cron_expr));
            return new_cron_expr;
        }

        template<typename T>
        cron_map<T>::cron_map(T default_val) :default_value{ default_val }
        {
            init();
        }

        template<typename T>
        cron_map<T>& cron_map<T>::operator+=(const cron_map<T> & classObj)
        {
            // TODO: insert return statement here
            // TODO: insert return statement here
            this->wm += classObj.wm;
            return *this;
        }
        template<typename T>
        inline cron_map<T>& cron_map<T>::operator+=(std::pair<std::string, T> cron_pair)
        {
            // convert string into cron componenets
            cron_cmpts cmpts = convert_to_cron_cmpts(cron_pair.first());
            // split the dow interval map with the intersection of the dow cron cmpt
            // with the intersections of dow, split the month interval map with the intersection of the month cmpt
            // with the intersections of month, split the month interval map with the intersection of the dom cmpt
            // with the intersections of dom, split the month interval map with the intersection of the hour cmpt
            // with the intersections of hour, split the month interval map with the intersection of the minute cmpt
            // with the intersections of minute, split the month interval map with the intersection of the second cmpt
            // write the value of seconds with the value in the cron pair
            // // split the existing intervals on the intersection of the cron_component 
            return *this;
        }
        template<typename T>
        inline void cron_map<T>::init()
        {
            sm.add(std::make_pair(seconds_interval, default_value));
            mm.add(std::make_pair(minutes_interval, sm));
            hm.add(std::make_pair(hours_interval, mm));
            dm.add(std::make_pair(dom_interval, hm));
            mn.add(std::make_pair(month_interval, dm));
            wm.add(std::make_pair(dow_interval, mn));
        };

        template<typename T>
        inline cron_map<T>::cron_map() : default_value{}
        {
            init();
        };

        template<typename T>
        void cron_map<T>::add(cron_cmpts cmpts, T value)
        {
            auto dow_intersect = wm & cmpts.dow;

            wm -= dow_intersect;

            std::transform(dow_intersect.begin(), dow_intersect.end(), boost::icl::adder(wm, wm.end()),
                [value, cmpts](auto dow_pair) {
                auto month_intersect = std::get<1>(dow_pair) & cmpts.month;
                std::get<1>(dow_pair) -= month_intersect;
                std::transform(month_intersect.begin(), month_intersect.end(), boost::icl::adder(std::get<1>(dow_pair), std::get<1>(dow_pair).end()),
                    [value, cmpts](auto month_pair) {
                    auto dom_intersect = std::get<1>(month_pair) & cmpts.dom;
                    std::get<1>(month_pair) -= dom_intersect;
                    std::transform(dom_intersect.begin(), dom_intersect.end(), boost::icl::adder(std::get<1>(month_pair), std::get<1>(month_pair).end()),
                        [value, cmpts](auto dom_pair) {
                        auto hour_intersect = std::get<1>(dom_pair) & cmpts.hour;
                        std::get<1>(dom_pair) -= hour_intersect;
                        std::transform(hour_intersect.begin(), hour_intersect.end(), boost::icl::adder(std::get<1>(dom_pair), std::get<1>(dom_pair).end()),
                            [value, cmpts](auto hour_pair) {
                            auto minute_intersect = std::get<1>(hour_pair) & cmpts.minute;
                            std::get<1>(hour_pair) -= minute_intersect;

                            std::transform(minute_intersect.begin(), minute_intersect.end(), boost::icl::adder(std::get<1>(hour_pair), std::get<1>(hour_pair).end()),
                                [value, cmpts](auto minute_pair) {
                                auto second_intersect = std::get<1>(minute_pair) & cmpts.seconds;
                                std::get<1>(minute_pair) -= second_intersect;
                                std::transform(second_intersect.begin(), second_intersect.end(), boost::icl::adder(std::get<1>(minute_pair), std::get<1>(minute_pair).end()),
                                    [value, cmpts](auto second_pair) {
                                    return std::make_pair(
                                        std::get<0>(second_pair),
                                        value
                                    );}
                                );
                                return std::make_pair(
                                    std::get<0>(minute_pair),
                                    std::get<1>(minute_pair)
                                ); 
                            });
                            return std::make_pair(
                                std::get<0>(hour_pair),
                                std::get<1>(hour_pair)
                            ); 
                        });

                        return std::make_pair(
                            std::get<0>(dom_pair),
                            std::get<1>(dom_pair)
                        ); 
                    });

                    return std::make_pair(
                        std::get<0>(month_pair),
                        std::get<1>(month_pair)
                    ); 
                });


                return std::make_pair(
                    std::get<0>(dow_pair),
                    std::get<1>(dow_pair)
                );
            });

        }
        template<typename T>
        void cron_map<T>::add(std::pair<cron_cmpts, T> cron_pair)
        {
            add(cron_pair.first, cron_pair.second);
        }
        ;
        template<typename T>
        void cron_map<T>::add(std::pair<std::string, T> cron_pair)
        {
            add(cron_pair.first, cron_pair.second);
        }
        template<typename T>
        inline void cron_map<T>::add(std::string cron_expr, T value)
        {
            cron_cmpts cmpts = convert_to_cron_cmpts(cron_expr);
            add(cmpts, value);
        }
        ;
        template<typename T>
        inline void cron_map<T>::remove(std::string cron_expr)
        {
            add(cron_expr, default_value);
        }
        ;
        template<typename T>
        inline void cron_map<T>::remove(cron_cmpts cmpts)
        {

            add(cmpts, default_value);
        }
        ;
        template<typename T>
        cron_map<T>::~cron_map()
        {
        };
    } // namespace cron_map
} // namespace cave
