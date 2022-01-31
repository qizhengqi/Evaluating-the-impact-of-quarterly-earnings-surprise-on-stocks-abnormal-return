# Evaluating-the-impact-of-quarterly-earnings-surprise-on-stocks-abnormal-return

This is a C++ Project for NYU course FRE-GY6883. In this project, my teammates and I:  
● Retrieved historical price data using Libcurl from Yahoo Finance  
● Stored EPS information in an STL map with tickers as keys  
● Sorted all Russell 1000 stocks by surprise percentage and split into three groups from highest to lowest: Beat, Meet and Miss  
● Implemented Bootstrapping method to simulate and compute Average Abnormal Return (AAR), Cumulative AAR (CAAR) and their standard deviations for each group  
● Visualized results with Gnuplot  
● Reduced runtime by 83% using multi-threads  
