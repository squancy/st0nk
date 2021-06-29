#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "type.h"
int SMA(int time_period, StockData* sd[], size_t sd_size, double output[], char type);
int EMA(int time_period, StockData* sd[], size_t sd_size, double output[], char type);
int WMA(int time_period, StockData* sd[], size_t sd_size, double output[], char type);
int DEMA(int time_period, StockData* sd[], size_t sd_size, double output[], char type);
int TEMA(int time_period, StockData* sd[], size_t sd_size, double output[], char type);
int KAMA(int time1, int time2, StockData* sd[], size_t sd_size, double output[]);
int MACD(int time1, int time2, StockData* sd[], size_t sd_size, double signal_line[], double macd[], char type);
int STOCH(int time_period, StockData* sd[], size_t sd_size, double pk[], double pd[], char type);
int RSI(int time_period, StockData* sd[], size_t sd_size, double output[], char type);
int WillR(int time_period, StockData* sd[], size_t sd_size, double output[], char type);

#endif
