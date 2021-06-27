#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/type.h"
#include "headers/colors.h"
#include "headers/fonts.h"
#include "headers/analysis.h"

const char* CHART_TYPES[] = {
  "ohlc", "candle", "line", "renko", "pnf"
};
const int CHART_TYPES_SIZE = 5;
const char* SUPPORTED_INDICATORS[] = {
  "SMA", "EMA", "WMA", "DEMA", "TEMA", "KAMA", "MACD", "STOCH", "RSI"
};
const int INDICATOR_SIZE = 9;

void free_sd(StockData* sd[], int n) {
  for (int i = 0; i < n; i++) {
    free(sd[i]);
  }
}

double get_price(char type, StockData* sd[], int i) {
  switch (type) {
    case 'c':
      return sd[i]->close;
    case 'o':
      return sd[i]->open;
    case 'h':
      return sd[i]->high;
    case 'l':
      return sd[i]->low;
    default:
      return sd[i]->close;
  }
}

void fill_closing_prices(double numbers[], int max, StockData* sd[]) {
  for (int i = 0; i < max; i++) {
    StockData* stock_data = malloc(sizeof(StockData));
    stock_data->open = 0;
    stock_data->high = numbers[i];
    stock_data->low = numbers[i];
    stock_data->close = numbers[i];
    stock_data->volume = 0;
    stock_data->symbol = "\0";
    stock_data->exchange = "\0";
    stock_data->date = "\0";
    sd[i] = stock_data;
  }
}

// Convert an array of doubles to a string: [a1, a2, a3, ..., an]
char* arr_to_str(double nums[], int len) {
  char* res = malloc(len * 320 + 1);
  res[0] = '[';
  for (int i = 0; i < len; i++) {
    char out[50];
    snprintf(out, sizeof(out), "%f, ", nums[i]);
    strcat(res, out);
  }
  int slen = strlen(res);
  res[slen - 2] = ']';
  res[slen - 1] = '\0';
  return res;
}

// Get the highest price of type during the last n days
double highest_price(int time_period, StockData* sd[], int sd_size, char type) {
  int start = sd_size - time_period;
  double highest = get_price(type, sd, start);
  for (int i = start + 1; i <= sd_size; i++) {
    double current_price = get_price(type, sd, i);
    if (current_price > highest) {
      highest = current_price;
    }
  }
  return highest;
}

// Get the lowest price of type during the last n days
double lowest_price(int time_period, StockData* sd[], int sd_size, char type) {
  int start = sd_size - time_period;
  double lowest = get_price(type, sd, start);
  for (int i = start + 1; i <= sd_size; i++) {
    double current_price = get_price(type, sd, i);
    if (current_price < lowest) {
      lowest = current_price;
    }
  }
  return lowest;
}

void print_status(int status) {
  if (status) {
    printf(GREEN "==> PASSED\n" RESET);
  } else {
    printf(RED "==> FAILED\n" RESET);
  }
}

void print_usage(void) {
  printf("Usage: sa [options]\n\t-s ");
  printf(UNST "[symbol]" UNEN);
  printf(": returns general information about the given stock (current price, hot news, analysis...).\n");
  printf("\tThis feature is only available for US stocks.\n");
  printf("\tThe ");
  printf(UNST "[symbol]" UNEN);
  printf(" given as a parameter must be a valid stock symbol listed in an exchange.\n");
  printf("\tTo see the supported exchanges run ");
  printf(UNST "sa -ls\n" UNEN);
  printf("\tFor getting the available stock symbols in a given exchange run ");
  printf(UNST "sa -se [exchange]\n\n" UNEN);
  printf("\t-h|--help: displays usage\n\n");
  printf("\t-se ");
  printf(UNST "[exchange]" UNEN);
  printf(": list all avaliable stock symbols in a given stock exchange\n\n");
  printf("\t-ls: list all available stock exchanges\n\n");
  printf("\t-ns: get latest market news (general, forex, crypto)\n\n");
  printf("\t-ic: get the 10 most recent and upcoming IPOs from the last 30 days\n\n");
  printf("\t-fe: list all available forex exchanges\n\n");
  printf("\t-fs ");
  printf(UNST "[exchange]" UNEN);
  printf(": list all available forex symbols in a given forex exchange\n\n");
  printf("\t-lc: list all available crypto exchanges\n\n");
  printf("\t-cs ");
  printf(UNST "[exchange]" UNEN);
  printf(": list all available crypto symbols in a given crypto exchange\n\n");
  printf("\t--graph ");
  printf(UNST "[symbol]" UNEN);
  printf(" ");
  printf(UNST "--from [date]" UNEN);
  printf(" ");
  printf(UNST "--to [date]" UNEN);
  printf(" ");
  printf(UNST "--type [type]" UNEN);
  printf(" ");
  printf(UNST "--indicators [indicators]" UNEN);
  printf(": graph a given stock \n\tlisted in an available exchange in a given time interval with optional indicators\n");
  printf("\tresult is saved as an image\n");
  printf(UNST "\t[symbol]" UNEN);
  printf(": must be a valid symbol of a stock listed in an exchange\n");
  printf(UNST "\t[date]" UNEN);
  printf(": must be a valid date up to present in format YYYY-MM-DD\n");
  printf(UNST "\t[type]" UNEN);
  printf(": describes the type of the chart to be graphed, must be one of: ohlc, candle, line, renko, pnf (optional)\n");
  printf(UNST "\t[indicators]" UNEN);
  printf(": must be some of the supported indicators \n\tand the parameters to them are given as comma separated values\n");
  printf("\te.g.: MACD,12,26,c means graph MACD with 12 and 26 days as parameters (values are always interpreted as days)\n");
  printf("\tthe last parameter is always one of: c, h, l, o (close, high, low, open)\n");
}

void exit_wrong_arg(char* arg) {
  printf("Invalid usage: please give an argument for ");
  printf(UNST "%s\n" UNEN, arg);
  exit(EXIT_FAILURE);
}

// If a chart type is given make sure it's supported by matplotlib
int chart_type_valid(char* chart_type) {
  for (int i = 0; i < CHART_TYPES_SIZE; i++) {
    if (strcmp(chart_type, CHART_TYPES[i]) == 0) {
      return 1;
    }
  }
  return 0;
}

// Extract the name of the indicator
char* extract_indname(char* param) {
  int len = strlen(param);
  char* res = malloc(len + 1);
  int k = 0;
  for (int i = 0; i < len; i++) {
    if (param[i] == ',') {
      break;
    } else {
      res[k++] = param[i];
    }
  }
  res[k] = '\0';
  return res;
}

// Checks if the indicators given as an argument are valid
int is_ind_valid(int argc, char* argv[], int ipos) {
  for (int i = ipos + 1; i < argc; i++) {
    char* current_parameter = argv[i];
    if (current_parameter[0] == '-' && current_parameter[1] == '-') {
      return 1;
    } else {
      int found = 0;
      char* indicator_name = extract_indname(current_parameter);
      for (int j = 0; j < INDICATOR_SIZE; j++) {
        if (strcmp(indicator_name, SUPPORTED_INDICATORS[j]) == 0) {
          found = 1;
          break;
        }
      }
      
      free(indicator_name);

      char lc = current_parameter[strlen(current_parameter) - 1];
      if (!found || (lc != 'c' && lc != 'h' && lc != 'o' && lc != 'l')) {
        return 0;
      }
    }
  } 
  return 1;
}

// Put all the indicators given as an argument in an array
int get_indicators(char* inds[], char* argv[], int ipos, int argc) {
  int k = 0;
  for (int i = ipos + 1; i < argc; i++) {
    if (argv[i][0] == '-' && argv[i][1] == '-') {
      return k;
    } else {
      inds[k++] = argv[i];
    }
  }
  return k;
}

int perform_indicator(int time_intervals[], StockData* sd[], size_t sd_size, double output[], double out2[], char* ind_name, char type) {
  if (strcmp(ind_name, "SMA") == 0) {
    return SMA(time_intervals[0], sd, sd_size, output, type);
  } else if (strcmp(ind_name, "EMA") == 0) {
    return EMA(time_intervals[0], sd, sd_size, output, type);
  } else if (strcmp(ind_name, "WMA") == 0) {
    return WMA(time_intervals[0], sd, sd_size, output, type);
  } else if (strcmp(ind_name, "DEMA") == 0) {
    return DEMA(time_intervals[0], sd, sd_size, output, type);
  } else if (strcmp(ind_name, "TEMA") == 0) {
    return TEMA(time_intervals[0], sd, sd_size, output, type);
  } else if (strcmp(ind_name, "KAMA") == 0) {
    return KAMA(time_intervals[0], time_intervals[1], sd, sd_size, output);
  } else if (strcmp(ind_name, "MACD") == 0) {
    return MACD(time_intervals[0], time_intervals[1], sd, sd_size, output, out2, type);
  } else if (strcmp(ind_name, "STOCH") == 0) {
    return STOCH(time_intervals[0], sd, sd_size, output, out2, type);
  } else if (strcmp(ind_name, "RSI") == 0) {
    return RSI(time_intervals[0], sd, sd_size, output);
  }
  return 0;
}
