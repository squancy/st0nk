#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include "headers/type.h"
#include "headers/api.h"
#include "headers/helpers.h"
#include "headers/lib.h"
#include "headers/colors.h"
#include "headers/fonts.h"

#define MAX_SIZE 10000

// Returns the minimum of 2 ints
static int min(int x, int y) {
  return x < y ? x : y;
}

// Returns the maximum of 2 ints
static int max(int x, int y) {
  return x < y ? y : x;
}

static void get_last_five_units(StockData* sd[], int len, int start, StockData* arr[]) {
  int k = 0;
  for (int i = start; i > start - 5; i--) {
    arr[k++] = sd[i];
  }
}

static void print_signal(int x) {
  if (x == 2) {
    printf(GREEN "\tBuy signal" RESET "/" RED "Sell signal\n" RESET);
  } else if (x == 1) {
    printf(GREEN "\tBuy signal\n" RESET);
  } else {
    printf(RED "\tSell signal\n" RESET);
  }
}

static char* format_date(char* date) {
  char* res = malloc(sizeof(char) * strlen(date) + 1);
  int i = 0;
  while (date[i] != 'T') {
    res[i] = date[i]; 
    i++;
  }

  res[i] = '\0';
  return res;
}

static void print_candle_info(char* pattern_name, int x, char* date1, char* date2) {
  printf(UNST "%s\n" UNEN, pattern_name);
  printf("\tDate: (%s; %s)\n", date1, date2);
  print_signal(x);
}

// Perform a candlestick pattern recognition on a given stock
void candlestick_recognition(char* from, char* to, char* symbol) {
  // First get the required stock data 
  StockData* sd[MAX_SIZE];
  char url[256];
  snprintf(url, sizeof(url), "&symbols=%s&sort=ASC&date_from=%s&date_to=%s&limit=1000", symbol, from, to);
  char* final_url = hist_url("eod", url);
  char* response = get_json(final_url, 0);
  cJSON* json = cJSON_Parse(response);
  json = cJSON_GetObjectItemCaseSensitive(json, "data");
  cJSON* obj;
  int len = 0;
  
  cJSON_ArrayForEach(obj, json) {
    double open = cJSON_GetObjectItemCaseSensitive(obj, "open")->valuedouble;
    double high = cJSON_GetObjectItemCaseSensitive(obj, "high")->valuedouble;
    double low = cJSON_GetObjectItemCaseSensitive(obj, "low")->valuedouble;
    double close = cJSON_GetObjectItemCaseSensitive(obj, "close")->valuedouble;
    char* date = format_date(cJSON_GetObjectItemCaseSensitive(obj, "date")->valuestring);
  
    // Only open, low, high and close prices are relevant
    StockData* el = malloc(sizeof(StockData));
    el->open = open;
    el->high = high;
    el->low = low;
    el->close = close;
    el->date = date;
    sd[len++] = el;
  }
  
  for (int i = 5; i < len; i++) {
    int k = 0;
    float bodies[5];
    float lower_wicks[5];
    float upper_wicks[5];
    StockData* arr[5];
    get_last_five_units(sd, len, i, arr);
    for (int j = 0; j < 5; j++) {
      bodies[k] = fabs(arr[j]->close - arr[j]->open);
      lower_wicks[k] = fabs(arr[j]->low - min(arr[j]->close, arr[j]->open));
      upper_wicks[k] = fabs(arr[j]->high - max(arr[j]->close, arr[j]->open));
      k++;
    }

    /*
      Some of the candlestick patterns can indicate both a bullish and bearish reversal depending on whether it appears at the end
      of an uptrend or downtrend
    */

    // Hammer/Hanging man: the lower wick is greater than the body and the upper wick is at most 30% of the body
    // Bullish/Bearish pattern
    if (bodies[0] < lower_wicks[0] && upper_wicks[0] < bodies[0] * 0.3) {
      print_candle_info("Hammer/Hanging man", 2, arr[0]->date, arr[0]->date);
    }

    // Inverse hammer/Shooting star: the upper wick is greater than the body and the lower wick is at most 30% of the body
    // Bullish/Bearish pattern
    if (bodies[0] < upper_wicks[0] && lower_wicks[0] < bodies[0] * 0.3) {
      print_candle_info("Inverse hammer/Shooting star", 2, arr[0]->date, arr[0]->date);
    }

    // Bullish/Bearish engulfing: first red candle is shorter, second green candle is larger, completely engulfs the first candle
    // Bullish pattern
    if (bodies[0] > bodies[1] && arr[0]->high > arr[1]->high && arr[0]->low < arr[1]->low
      && arr[0]->open < arr[0]->close && arr[1]->open > arr[1]->close) {
      print_candle_info("Bullish/Bearish engulfing", 2, arr[1]->date, arr[0]->date);
    }

    // Piercing line: first candle is a long red, 2nd one is a long green, its close price must be more than the middle of the 1st candle
    // Bullish pattern
    if (lower_wicks[0] < bodies[0] * 0.33 && upper_wicks[0] < bodies[0] * 0.33 && lower_wicks[1] < bodies[1] * 0.33
      && upper_wicks[1] < bodies[1] * 0.33 && arr[0]->close > ((arr[1]->close + arr[1]->open) / 2) && arr[0]->close > arr[0]->open
      && arr[1]->close < arr[1]->open) {
      print_candle_info("Piercing line", 1, arr[1]->date, arr[0]->date);
    }

    // Morning/Evening star: 1st candle is a long red, 2nd is a short one with a gap, 3rd one is a long green
    // Bullish/Bearish pattern
    if (arr[0]->close > arr[0]->open && bodies[1] < bodies[0] && arr[1]->open < arr[0]->open && arr[1]->close < arr[0]->open
      && arr[2]->close < arr[2]->open && bodies[2] > bodies[1] && arr[1]->open < arr[2]->close && arr[1]->close < arr[2]->close) {
      print_candle_info("Morning/Evening star", 2, arr[2]->date, arr[0]->date);
    }

    // Three white soldiers: three green candles with short wicks, all open and close progressively higher than the previous one
    // Bullish pattern
    int candles[3];
    for (int i = 0; i < 3; i++) {
      candles[i] = arr[i]->close > arr[i]->open && lower_wicks[i] < bodies[i] * 0.33 && upper_wicks[i] < bodies[i] * 0.33;
      if (i < 2) {
        candles[i] = candles[i] && arr[i]->open > arr[i + 1]->open && arr[i]->close > arr[i + 1]->close;
      }
    }

    if (candles[0] && candles[1] && candles[2]) {
      print_candle_info("Three white soldiers", 1, arr[2]->date, arr[0]->date);
    }

    // Three black crows: 3 red candles, each closes and opens progressively lower than the previous day
    // Bearish pattern
    for (int i = 0; i < 3; i++) {
      candles[i] = arr[i]->close < arr[i]->open && lower_wicks[i] < bodies[i] * 0.33 && upper_wicks[i] < bodies[i] * 0.33;
      if (i < 2) {
        candles[i] = candles[i] && arr[i]->open < arr[i + 1]->open && arr[i]->close < arr[i + 1]->close;
      }
    }

    if (candles[0] && candles[1] && candles[2]) {
      print_candle_info("Three black crows", 0, arr[2]->date, arr[0]->date);
    }
  }

  // Free memory
  for (int i = 0; i < len; i++) {
    free(sd[i]->date);
    free(sd[i]);
  }
}
