#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "headers/type.h"
#include "headers/helpers.h"

#define SMOOTHING 2.0

// For all indicators time interval is arbitrary if the correct dataset is passed in
// Type can be one of: c, o, h, l (close, open, high, low price)

// SMA (Simple Moving Average) 
int SMA(int time_period, StockData* sd[], size_t sd_size, double output[], char type) {
  int lower_index = 0;
  int upper_index = lower_index + time_period;
  int k = 0;
  while (upper_index <= sd_size) {
    double avg = 0;
    for (int i = lower_index; i < upper_index; i++) {
      avg += get_price(type, sd, i);
    }
    avg /= time_period;
    output[k++] = avg;    
    lower_index++;
    upper_index++;
  }
  return k;
}

double EMA_rec(StockData* sd[], int l, int time_period, char type) {
  if (l == time_period - 1) {
    double output[1];
    SMA(time_period, sd, time_period, output, type);
    return output[0];
  }
  double value_today = get_price(type, sd, l);
  double alpha = SMOOTHING / (time_period + 1);
  return value_today * alpha + (1 - alpha) * EMA_rec(sd, l - 1, time_period, type);
}

// EMA (Exponential Moving Average) with a smoothing factor of 2
int EMA(int time_period, StockData* sd[], size_t sd_size, double output[], char type) {
  int k;
  int cnt = 0;
  for (k = time_period - 1; k < sd_size; k++) {
    double EMA_today = EMA_rec(sd, k, time_period, type); 
    output[cnt++] = EMA_today;
  }
  return cnt;
}

// WMA (Weighted Moving Average)
int WMA(int time_period, StockData* sd[], size_t sd_size, double output[], char type) {
  int lower_index = 0;
  int upper_index = lower_index + time_period;
  int k = 0;
  int t = time_period * (time_period + 1) / 2;
  while (upper_index <= sd_size) {
    double avg = 0;
    int ltp = time_period;
    for (int i = upper_index - 1; i >= lower_index; i--, ltp--) {
      avg += get_price(type, sd, i) * ltp;
    }
    avg /= t;
    output[k++] = avg;
    lower_index++;
    upper_index++;
  }
  return k;
}

// DEMA (Double Exponential Moving Average)
int DEMA(int time_period, StockData* sd[], size_t sd_size, double output[], char type) {
  int size = sd_size - time_period + 1;
  int nn_size = size - time_period + 1;
  double EMA_n[size];
  double EMA_n_n[nn_size];
  double su[size];
  StockData* sd_new[sd_size];

  EMA(time_period, sd, sd_size, su, type);
  fill_closing_prices(su, sd_size, sd_new);
  memcpy(EMA_n, su, sizeof(EMA_n));
  
  EMA(time_period, sd_new, sd_size, su, type);
  memcpy(EMA_n_n, su, sizeof(EMA_n_n));
  
  int i;
  for (i = 0; i < nn_size; i++) {
    output[i] = 2 * EMA_n[i + time_period - 2] - EMA_n_n[i];
  }
  return i;
}

// TEMA (Triple Exponential Moving Average)
int TEMA(int time_period, StockData* sd[], size_t sd_size, double output[], char type) {
  int size = sd_size - time_period + 1;
  int nn_size = size - time_period + 1;
  int nnn_size = nn_size - time_period + 1;
  double EMA_n[size];
  double EMA_n_n[nn_size];
  double EMA_n_n_n[nnn_size];
  double su[size];
  StockData* sd_new[sd_size];

  EMA(time_period, sd, sd_size, su, type);
  fill_closing_prices(su, sd_size, sd_new);
  memcpy(EMA_n, su, sizeof(EMA_n));
  
  EMA(time_period, sd_new, sd_size, su, type);
  fill_closing_prices(su, sd_size, sd_new);
  memcpy(EMA_n_n, su, sizeof(EMA_n_n));

  EMA(time_period, sd_new, sd_size, su, type);
  memcpy(EMA_n_n_n, su, sizeof(EMA_n_n_n));

  int i;
  for (i = 0; i < nnn_size; i++) {
    output[i] = 3 * EMA_n[i + 2 * time_period - 3] - 3 * EMA_n_n[i + time_period - 2] + EMA_n_n_n[i];
  }
  return i;
}

double KAMA_rec(double SC, StockData* sd[], int i) {
  if (i == 0) {
    return sd[i]->close;
  }
  double rec = KAMA_rec(SC, sd, i - 1);
  return rec + SC * (sd[i]->close - rec);
}

// KAMA (Kaufman Adaptive Moving Average) 
// TODO: write unit tests for this
int KAMA(int time1, int time2, StockData* sd[], size_t sd_size, double output[]) {
  double direction = fabs(sd[sd_size - 1]->close - sd[sd_size - 10]->close);
  double volatility = 0;
  for (int i = 1; i < sd_size; i++) {
    volatility += fabs(sd[i]->close - sd[i - 1]->close);
  } 
  double ER = direction / volatility;
  double fast_alpha = 2.0 / (time1 + 1); 
  double slow_alpha = 2.0 / (time2 + 1);
  double SC = pow((ER * (fast_alpha - slow_alpha) + slow_alpha), 2);
  int i;
  for (i = 0; i < sd_size; i++) {
    output[i] = KAMA_rec(SC, sd, i);
  }
  return i;
}

// MACD (Moving Average Convergence Divergence)
int MACD(int time1, int time2, StockData* sd[], size_t sd_size, double signal_line[], double macd[], char type) {
  int k = 0;
  int max;
  if (time1 > time2) {
    max = time1;
  } else {
    max = time2;
  }

  for (int i = max; i < sd_size; i++, k++) {
    signal_line[k] = EMA_rec(sd, i, time1, type);
    macd[k] = EMA_rec(sd, i, time2, type);
  }
  return k;
}

// STOCH (Stochastic Oscillator)
int STOCH(int time_period, StockData* sd[], size_t sd_size, double pk[], double pd[], char type) {
  int k = 0;
  for (int i = time_period + 1; i < sd_size; i++) {
    double C = sd[i]->close;
    double L_period = lowest_price(time_period, sd, i, 'l');
    double H_period = highest_price(time_period, sd, i, 'h');
    double percent_k = (C - L_period) / (H_period - L_period) * 100.0; 
    pk[k++] = percent_k;
  }
  
  StockData* sd_new[k];
  fill_closing_prices(pk, k, sd_new);
  SMA(3, sd_new, k, pd, type);

  return k - 2;
}

// RSI (Relative Strenght Index)
// Implemented with SMA as an averaging function
int RSI(int time_period, StockData* sd[], size_t sd_size, double output[], char type) {
  int k = 0;
  for (int i = time_period + 1; i < sd_size; i++) {
    double sumU = 0;
    double sumD = 0;
    for (int j = i - time_period; j < i; j++) {
      double diff = get_price(type, sd, j) - get_price(type, sd, j - 1);
      if (diff > 0) {
        sumU += diff;
      } else {
        sumD += fabs(diff);
      }
    }
    
    double avgU = sumU / time_period;
    double avgD = sumD / time_period;
    double RS = avgU / avgD;
    output[k++] = 100 - 100 / (1 + RS);
  }

  return k;
}

// Williams %R
int WillR(int time_period, StockData* sd[], size_t sd_size, double output[], char type) {
  int k = 0;
  for (int i = time_period + 1; i < sd_size; i++) {
    double C = get_price(type, sd, i);
    double lowest_low = lowest_price(time_period, sd, i, 'l');
    double highest_high = highest_price(time_period, sd, i, 'h');
    double will_r = (highest_high - C) / (highest_high - lowest_low) * -100;
    output[k++] = will_r;
  } 

  return k;
}
