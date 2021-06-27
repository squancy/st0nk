#ifndef TYPE_H
#define TYPE_H

typedef struct stock_data {
  double open;
  double high;
  double low;
  double close;
  double volume;
  char* symbol;
  char* exchange;
  char* date;
} StockData;

#endif
