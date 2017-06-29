# cron_map

cron_map is a data structure that stores discrete values with a temporal association.  

Data is added to the cron map using a cron expression paired with a value.  The cron expression is used to build a tree like structure,such that all time points are covered.
The rational behind creating such a data structure, is to store/access data that has a known time pattern. The hope is that the overhead required to insert and retrieve data is outweighed by the savings in memory compared to other data structures that resemble time series.

The boost::icl library is used extensively.

Feel free to challenge everything about this project.  There is room for significant improvement.
