#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include "headers/api.h"
#include "headers/colors.h"
#include "headers/lib.h"
#include "headers/fonts.h"
#include "headers/const.h"
#include "headers/type.h"
#include "headers/pyc.h"
#include "headers/helpers.h"
#include "headers/candlestick.h"

#define SIZE 10000

static void fetch_data_no_param(char* url_param, char* main_text, char* format_string, char type);
static void print_json_data(char* final_url, char* text, char* props[], char* format_strings[], int size, int max, char ps[]);
static void print_symbols_in_exchange(char* exchange, char* url_param, char* format_string);
static void graph_stock(int is_backtest, long money, int argc, char** argv, int ipos, char* from, char* to, char* symbol, char* chart_type);
const char* comma = ",";

int main(int argc, char* argv[]) {
  if (argc == 1 || ((flag_exists(argc, argv, "-h") || flag_exists(argc, argv, "--help")) && argc == 2)) {
    print_usage();
  } else if (argc == 2 && flag_exists(argc, argv, "-ls")) {
    printf(MAGENTA "Currently supported exchanges:" RESET);
    for (int i = 0; i < EXCHANGE_SIZE; i++) {
      if (i % 10 == 0) printf("\n");
      printf("%s\t", EXCHANGE_CODES[i]);
    }
    printf("\n");
  } else if (argc == 3 && flag_exists(argc, argv, "-se")) {
    // Return available stocks in a given stock exchange
    int pos = return_pos(argc, argv, "-se");
    if (pos == 2) {
      exit_wrong_arg("-e");
    }

    char* exchange = argv[pos + 1];
    print_symbols_in_exchange(exchange, "stock", "Stock symbols in exchange %s");
  } else if (argc == 3 && flag_exists(argc, argv, "-s")) {
    // Return useful information (current price, hot news, technical analysis etc.) for a given stock
    int pos = return_pos(argc, argv, "-s");
    if (pos == 2) {
      exit_wrong_arg("-s");
    }
    char* stock = argv[pos + 1]; 
    char* base_url;
    char param_url[512];
    char* final_url;

    // Get quote
    base_url = "quote?symbol=";
    snprintf(param_url, sizeof(param_url), "%s%s", base_url, stock);
    final_url = append_url(param_url, strlen(param_url));
    char* text = "Price information";
    char* props_[5] = {"c", "h", "l", "o", "pc"};
    char* format_strings_[] = {
      BOST "Current price:" BOEN " $%.2f\n",
      BOST "Highest price of the day:" BOEN " $%.2f\n",
      BOST "Lowest price of the day:" BOEN " $%.2f\n",
      BOST "Open price of the day:" BOEN " $%.2f\n",
      BOST "Previous close price:" BOEN " $%.2f\n\n"
    };

    char ps[5] = {'d', 'd', 'd', 'd', 'd'};
    print_json_data(final_url, text, props_, format_strings_, 5, 1, ps);
    print_h_line(80);
    free(final_url);

    // Get recent news
    base_url = "company-news?symbol=";
    char today[64];
    today_minus_n_days(today, 0, 0, 0, sizeof(today), 3);
    snprintf(param_url, sizeof(param_url), "%s%s&from=%s&to=%s", base_url, stock, today, today);
    final_url = append_url(param_url, strlen(param_url)); 
    char txt[256];
    snprintf(txt, sizeof(txt), "Recent hot news for %s", stock);
    char* props[3] = {"headline", "summary", "url"};
    char* format_strings[] = {
      BOST "%s\n" BOEN,
      "%s\n",
      BOST "Read more:" BOEN " %s\n\n"
    };
    char ps_n[3] = {'s', 's', 's'};

    print_json_data(final_url, txt, props, format_strings, 3, 3, ps_n);
    print_h_line(80);
    free(final_url);

    // Get basic financial information: 52 week low/high, P/E ratio etc.
    base_url = "stock/metric?symbol=";
    snprintf(param_url, sizeof(param_url), "%s%s&metric=all", base_url, stock);
    final_url = append_url(param_url, strlen(param_url));
    text = "Financial information";
    char* props_f[5] = {"52WeekHigh", "52WeekLow", "52WeekHighDate", "52WeekLowDate", "peBasicExclExtraTTM"};
    char* format_strings_f[] = {
      BOST "52 week high:" BOEN " $%.2f\n",
      BOST "52 week low:" BOEN " $%.2f\n",
      BOST "52 week high date:" BOEN " %s\n",
      BOST "52 week low date:" BOEN " %s\n",
      BOST "P/E ratio:" BOEN " %.2f\n\n"
    };
    char ps_f[5] = {'d', 'd', 's', 's', 'd'};

    print_json_data(final_url, text, props_f, format_strings_f, 5, 1, ps_f);
    print_h_line(80);
    free(final_url);

    // Perform a technical analysis on the given stock
    // Fetch end of day price data for the last 90 days
    char from[64], to[64];
    today_minus_n_days(from, 0, 0, 0, sizeof(from), 2);
    today_minus_n_days(to, 0, 0, 0, sizeof(to), 0);   

    char url[512];  
    snprintf(url, sizeof(url), "&symbols=%s&date_from=%s&date_to=%s&limit=1000", stock, from, to);
    final_url = hist_url("eod", url);
    char* response = get_json(final_url, 0);
    cJSON* json = cJSON_Parse(response);
    cJSON* data = cJSON_GetObjectItemCaseSensitive(json, "data");
    cJSON* obj;
    StockData sd[90];
    int i = 0;
    cJSON_ArrayForEach(obj, data) {
      StockData current_stock_data; 
      current_stock_data.open = cJSON_GetObjectItemCaseSensitive(obj, "open")->valuedouble;
      current_stock_data.high = cJSON_GetObjectItemCaseSensitive(obj, "high")->valuedouble;
      current_stock_data.low = cJSON_GetObjectItemCaseSensitive(obj, "low")->valuedouble;
      current_stock_data.close = cJSON_GetObjectItemCaseSensitive(obj, "close")->valuedouble;
      current_stock_data.volume = cJSON_GetObjectItemCaseSensitive(obj, "volume")->valuedouble;
      current_stock_data.symbol = cJSON_GetObjectItemCaseSensitive(obj, "symbol")->valuestring;
      current_stock_data.exchange = cJSON_GetObjectItemCaseSensitive(obj, "exchange")->valuestring;
      current_stock_data.date = cJSON_GetObjectItemCaseSensitive(obj, "date")->valuestring;
      sd[i++] = current_stock_data;
    }
  } else if (argc == 2 && flag_exists(argc, argv, "-ns")) {
    // Get latest market news (general, forex, crypto) 
    char* types[3] = {"general", "forex", "crypto"};
    for (int i = 0; i < 3; i++) {
      char* current_type = types[i];
      char param_url[256];
      snprintf(param_url, sizeof(param_url), "news?category=%s", current_type);
      char* final_url = append_url(param_url, strlen(param_url));
      char* props[3] = {"headline", "summary", "url"};
      char txt[128];
      snprintf(txt, sizeof(txt), "%c%s market news", toupper(current_type[0]), current_type + 1);
      char* format_strings[] = {
        BOST "%s\n" BOEN,
        "%s\n",
        BOST "Read more:" BOEN " %s\n\n"
      };
      char ps_n[3] = {'s', 's', 's'};

      print_json_data(final_url, txt, props, format_strings, 3, 3, ps_n);
      print_h_line(80);
      free(final_url);
    }
  } else if (argc == 2 && flag_exists(argc, argv, "-ic")) {
    // Get IPO calendar
    char from[64], to[64];
    today_minus_n_days(from, 0, 0, 0, sizeof(from), 30);
    today_minus_n_days(to, 0, 0, 0, sizeof(to), 0);
    char param_url[256];
    snprintf(param_url, sizeof(param_url), "calendar/ipo?from=%s&to=%s", from, to);
    char* final_url = append_url(param_url, strlen(param_url));
    char* props[7] = {"date", "exchange", "name", "numberOfShares", "price", "symbol", "totalSharesValue"};
    char* text = "IPO calendar for the last 30 days";
    char* format_strings[] = {
      BOST "Date of IPO:" BOEN " %s\n",
      BOST "Exchange:" BOEN " %s\n",
      BOST "Name:" BOEN " %s\n",
      BOST "Number of shares:" BOEN " %d\n",
      BOST "Estimated price:" BOEN " $%s\n",
      BOST "Symbol:" BOEN " %s\n",
      BOST "Total shares value:" BOEN " $%d\n\n"
    };
    char ps[7] = {'s', 's', 's', 'i', 's', 's', 'i'};

    print_json_data(final_url, text, props, format_strings, 7, 10, ps);
    free(final_url);
  } else if (argc == 2 && flag_exists(argc, argv, "-fe")) {
    // List all available forex exchanges
    char* url_param = "https://finnhub.io/api/v1/forex/exchange";
    char* main_text = "Available forex exchanges:";
    fetch_data_no_param(url_param, main_text, "%s\n", 's');
  } else if (argc == 3 && flag_exists(argc, argv, "-fs")) {
    int pos = return_pos(argc, argv, "-fs");
    if (pos == 2) {
      exit_wrong_arg("-fs");
    }

    char* exchange = argv[pos + 1];
    print_symbols_in_exchange(exchange, "forex", "Forex symbols in exchange %s");
  } else if (argc == 2 && flag_exists(argc, argv, "-lc")) {
    // List all available crypto exchanges
    char* url_param = "https://finnhub.io/api/v1/crypto/exchange";
    char* main_text = "Available crypto exchanges:";
    fetch_data_no_param(url_param, main_text, "%s\n", 's');
  } else if (argc == 3 && flag_exists(argc, argv, "-cs")) {
    // List all available symbols in a given crypto exchange
    int pos = return_pos(argc, argv, "-cs");
    if (pos == 2) {
      exit_wrong_arg("-cs");
    }

    char* exchange = argv[pos + 1];
    print_symbols_in_exchange(exchange, "crypto", "Crypto symbols in exchange %s");
  } else if (flag_exists(argc, argv, "--candlestick") && argc == 7) {
    int cpos = return_pos(argc, argv, "--candlestick");
    int fpos = return_pos(argc, argv, "--from");
    int tpos = return_pos(argc, argv, "--to");
    if (cpos == argc - 1 || fpos == argc - 1 || tpos == argc - 1) {
      exit_wrong_arg("--candlestick");
    }

    char* symbol = argv[cpos + 1];

    // Make sure symbol is supported
    if (!graph_symbol_supported(symbol)) {
      printf("Symbol given is not supported\n");
      exit(EXIT_FAILURE);
    }

    // Make sure 'from' and 'to' are valid dates
    char* from = argv[fpos + 1];
    char* to = argv[tpos + 1];
    if (!is_date_valid(from) || !is_date_valid(to) || date_diff(from, to)) {
      printf("Invalid date given\n");
      exit(EXIT_FAILURE);
    }

    candlestick_recognition(from, to, symbol);
  } else if (flag_exists(argc, argv, "--backtest") && argc == 7) {
    // Graph a given stock + calculate the performance of the given trading strategy
    int bpos = return_pos(argc, argv, "--backtest");
    int ipos = return_pos(argc, argv, "--indicator");
    int mpos = return_pos(argc, argv, "--money");
    if (bpos == argc - 1 || ipos == argc - 1 || mpos == argc - 1) {
      exit_wrong_arg("--backtest");
    }

    char* symbol = argv[bpos + 1];

    // Make sure symbol is supported
    if (!graph_symbol_supported(symbol)) {
      printf("Symbol given is not supported\n");
      exit(EXIT_FAILURE);
    }

    // If given, make sure that the indicators are valid
    if (!is_ind_valid(argc, argv, ipos)) {
      printf("Invalid indicator given\n");
      exit(EXIT_FAILURE);
    }
    
    // Make sure initial money given is a valid number
    char* end;
    char buf[SIZE];
    strncpy(buf, argv[mpos + 1], strlen(argv[mpos + 1]));
    long money = strtol(buf, &end, 10);
    if (buf == end) {
      printf("Invalid initial money given\n");
      exit(EXIT_FAILURE);
    }
    
    char from[100];
    char to[100];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int year = tm.tm_year + 1900;
    int month = tm.tm_mon + 1;
    int day = tm.tm_mday;
    int from_month = (month <= 10) ? month + 2 : (month + 2) % 13 + 1;
    sprintf(from, "%d-%02d-%02d", year - 1, from_month, day);
    sprintf(to, "%d-%02d-%02d", year, month, day);
   
    // tp must be a null terminated string since it's later converted into a Python object
    graph_stock(1, money, argc, argv, ipos, from, to, symbol, "candle");
  } else if (flag_exists(argc, argv, "--graph")) {
    // Graph a given stock + optional indicators and save image
    int gpos = return_pos(argc, argv, "--graph");
    int fpos = return_pos(argc, argv, "--from");
    int tpos = return_pos(argc, argv, "--to");
    int tppos = return_pos(argc, argv, "--type");
    int ipos = return_pos(argc, argv, "--indicators");
    if (gpos == argc - 1 || fpos == argc - 1 || tpos == argc - 1 || tppos == argc - 1 || ipos == argc - 1) {
      exit_wrong_arg("--graph");
    }

    // Fetch historical data with the given parameters
    char* symbol = argv[gpos + 1];
    char* from = argv[fpos + 1];
    char* to = argv[tpos + 1];
    char* chart_type = "candle";
    if (tppos > -1) {
      chart_type = argv[tppos + 1];
    }
    
    // Make sure the chart type is valid
    if (!chart_type_valid(chart_type)) {
      printf("Invalid chart type given\n");
      exit(EXIT_FAILURE);
    }

    // If given, make sure that the indicators are valid
    if (ipos > -1 && !is_ind_valid(argc, argv, ipos)) {
      printf("Invalid indicator given\n");
      exit(EXIT_FAILURE);
    }

    // Make sure symbol is supported
    if (!graph_symbol_supported(symbol)) {
      printf("Symbol given is not supported\n");
      exit(EXIT_FAILURE);
    }

    // Make sure 'from' and 'to' are valid dates
    if (!is_date_valid(from) || !is_date_valid(to) || date_diff(from, to)) {
      printf("Invalid date given\n");
      exit(EXIT_FAILURE);
    }

    int len = strlen(argv[ipos + 1]);
    char tp = argv[ipos + 1][len - 1];


    graph_stock(0, 0, argc, argv, ipos, from, to, symbol, chart_type);
  } else {
    printf("Invalid usage: please consult the docs by running ");
    printf(UNST "sa -h\n" UNEN);
    exit(EXIT_FAILURE);
  }
}

// Create a graph about the stock, also illustrate indicators
static void graph_stock(int is_backtest, long money, int argc, char** argv, int ipos, char* from, char* to, char* symbol, char* chart_type) {
  int len = strlen(argv[ipos + 1]);
  char tpc = argv[ipos + 1][len - 1];
  char tp[2] = {tpc, '\0'};

  char* inds[100];
  int ind_len = ipos == -1 ? 0 : get_indicators(inds, argv, ipos, argc);
  int time_intervals[256][3];
  char types[256];
  int time_interval_len = 0;
  int types_len = 0;
  int max_time_interval = 0;
  int tis[256];
  int tis_cnt = 0;
  for (int i = 0; i < ind_len; i++) {
    char* ind_name = extract_indname(inds[i]); 
    char param_buf[32];
    strncpy(param_buf, inds[i] + strlen(ind_name) + 1, strlen(inds[i]) - strlen(ind_name) - 1);
    char* token;
    token = strtok(param_buf, comma);
    int j = 1;
    char type;
    while (token != NULL) {
      int t = atoi(token);
      if (t > 0) {
        time_intervals[time_interval_len][j++] = t;
        tis[tis_cnt++] = t;
        if (t > max_time_interval) {
          max_time_interval = t;
        }
        /*
        if (strcmp(ind_name, "STOCH") == 0) {
          tis[tis_cnt++] = t;
        }
        */
      } else if (*token == 'o' || *token == 'c' || *token == 'h' || *token == 'l') {
        types[types_len++] = *token;
      }
      token = strtok(NULL, comma);
    }
    time_intervals[time_interval_len][0] = j;
    time_interval_len++;
  }

  // Look back for more days in order to get enough data to plot the indicators with the price
  int lookback_days = max_time_interval * 5;
  int y, m, d;
  get_ymd(from, &y, &m, &d);

  char lookback_from[64];
  today_minus_n_days(lookback_from, y, m, d, sizeof(lookback_from), lookback_days);
  
  char url[256];
  snprintf(url, sizeof(url), "&symbols=%s&sort=ASC&date_from=%s&date_to=%s&limit=1000", symbol, lookback_from, to);
  char* final_url = hist_url("eod", url);
  char* response = get_json(final_url, 0);
  cJSON* json = cJSON_Parse(response);
  json = cJSON_GetObjectItemCaseSensitive(json, "data");
  cJSON* obj;
  int has_content = 0;

  // Build list of dictionaries that will be passed to Python
  // If indicators are also passed in build a StockData struct
  int k = 0;
  int pl = 0;
  StockData* sd[SIZE];
  setenv("PYTHONPATH", ".", 1);
  Py_Initialize();
  PyObject* py_tuple = PyTuple_New(7);
  PyObject* py_list = PyList_New(0);
  PyObject* py_indlist = PyList_New(0);
  PyObject* py_tis = PyList_New(0);
  PyErr_Print();
  PyObject* py_value;
  cJSON_ArrayForEach(obj, json) {
    double open = cJSON_GetObjectItemCaseSensitive(obj, "open")->valuedouble;
    double high = cJSON_GetObjectItemCaseSensitive(obj, "high")->valuedouble;
    double low = cJSON_GetObjectItemCaseSensitive(obj, "low")->valuedouble;
    double close = cJSON_GetObjectItemCaseSensitive(obj, "close")->valuedouble;
    double volume = cJSON_GetObjectItemCaseSensitive(obj, "volume")->valuedouble;
    char* symbol = cJSON_GetObjectItemCaseSensitive(obj, "symbol")->valuestring;
    char* exchange = cJSON_GetObjectItemCaseSensitive(obj, "exchange")->valuestring;
    char* date = cJSON_GetObjectItemCaseSensitive(obj, "date")->valuestring;
    py_value = Py_BuildValue("{s:d, s:d, s:d, s:d, s:d, s:s, s:s, s:s}",
                              "open", open, "high", high, "low", low, "close", close, "volume", volume,
                              "symbol", symbol, "exchange", exchange, "date", date);

    // Price data is only displayed when it reaches the actual 'from' parameter given by the user
    // The extra lookback is only needed for indicators
    char date_formed[16];
    strncpy(date_formed, date, 10 * sizeof(char));
    if (date_diff(date_formed, from)) {
      int status = PyList_Append(py_list, py_value); 
      if (status == -1) {
        printf("An error occurred during the process.\n");
        exit(EXIT_FAILURE);
      }
      pl++;
    }
    
    StockData* el = malloc(sizeof(StockData));
    el->open = open;
    el->high = high;
    el->low = low;
    el->close = close;
    el->volume = volume;
    el->symbol = symbol;
    el->exchange = exchange;
    el->date = date;
    sd[k++] = el;
    has_content = 1;
  }

  if (!has_content) {
    printf("No data is available with the given parameters.\n");
    Py_FinalizeEx();
    exit(EXIT_FAILURE);
  }

  int l = 0;
  for (int i = 0; i < ind_len; i++) {
    char* ind_name = extract_indname(inds[i]); 
    double output[k];
    double output2[k];
    int ta[3] = {time_intervals[i][1], time_intervals[i][2], time_intervals[i][3]};
    int dataset_len = perform_indicator(ta, sd, k, output, output2, ind_name, types[i]);

    // Convert the dataset into a Python array
    PyObject* l_ind = PyList_New(0);
    PyObject* l_ind2 = PyList_New(0);

    for (int j = dataset_len - pl; j < dataset_len; j++) {
      PyObject* d_value = Py_BuildValue("d", output[j]);
      PyList_Append(l_ind, d_value);
    }

    if (strcmp(ind_name, "MACD") == 0 || strcmp(ind_name, "STOCH") == 0) {
      for (int j = dataset_len - pl; j < dataset_len; j++) {
        PyObject* d_value = Py_BuildValue("d", output2[j]);
        PyList_Append(l_ind2, d_value);
      }
    }

    PyObject* final_value = Py_BuildValue("{s:O}", ind_name, l_ind);
    PyList_Append(py_indlist, final_value);
    if (strcmp(ind_name, "MACD") == 0 || strcmp(ind_name, "STOCH") == 0) {
      PyObject* val = Py_BuildValue("{s:O}", ind_name, l_ind2);
      PyList_Append(py_indlist, val);
    }
  }

  for (int i = 0; i < tis_cnt; i++) {
    PyObject* val = Py_BuildValue("i", tis[i]);
    PyList_Append(py_tis, val);
  }
  
  // Pass in arguments as a Python tuple
  PyTuple_SetItem(py_tuple, 0, py_list);
  PyTuple_SetItem(py_tuple, 1, Py_BuildValue("s", chart_type));
  PyTuple_SetItem(py_tuple, 2, py_indlist);
  PyTuple_SetItem(py_tuple, 3, py_tis);
  PyTuple_SetItem(py_tuple, 4, Py_BuildValue("i", is_backtest));
  PyTuple_SetItem(py_tuple, 5, Py_BuildValue("i", money));
  PyTuple_SetItem(py_tuple, 6, Py_BuildValue("s", tp));

  Py_FinalizeEx();

  call_python_func(py_tuple, "graph", "draw_graph");
  free_sd(sd, k);
  cJSON_Delete(json);
  cJSON_Delete(obj);
}

// Fetches stock data from server and prints it to stdout
static void print_json_data(char* final_url, char* text, char* props[], char* format_strings[], int size, int max, char ps[]) {
  char* response = get_json(final_url, 0);

  // If response is not an array of JSON objects then wrap it in an array even if it's only 1 object
  // So that we can iterate over the array
  char resp[strlen(response) + 4];
  if (response[0] != '[') {
    snprintf(resp, sizeof(resp), "[%s]", response);
  } else {
    snprintf(resp, sizeof(resp), "%s", response);
  }

  cJSON* json = cJSON_Parse(resp);
  if (json == NULL) {
    printf(RED "ERROR: could not parse response" RESET);
    exit(EXIT_FAILURE);
  }

  if (strcmp(text, "Financial information") == 0) {
    json = cJSON_GetObjectItemCaseSensitive(cJSON_GetArrayItem(json, 0), "metric");
    char str[1024 * 1024];
    snprintf(str, sizeof(str), "[%s]", cJSON_Print(json));
    json = cJSON_Parse(str);
  } else if (strcmp(text, "IPO calendar for the last 30 days") == 0) {
    json = cJSON_GetObjectItemCaseSensitive(cJSON_GetArrayItem(json, 0), "ipoCalendar");
  }
  
  int count = 0;
  printf(MAGENTA "%s\n\n" RESET, text);
  cJSON* obj;
  cJSON_ArrayForEach(obj, json) {
    if (count < max) {
      for (int i = 0; i < size; i++) {
        if (ps[i] == 's') {
          char* current_prop = cJSON_GetObjectItemCaseSensitive(obj, props[i])->valuestring;
          printf(format_strings[i], current_prop);
        } else if (ps[i] == 'd') {
          double current_prop = cJSON_GetObjectItemCaseSensitive(obj, props[i])->valuedouble;
          printf(format_strings[i], current_prop);
        } else {
          int current_prop = cJSON_GetObjectItemCaseSensitive(obj, props[i])->valueint;
          printf(format_strings[i], current_prop);
        }
      }
    }
    count++;
  }

  cJSON_Delete(json);
  cJSON_Delete(obj);
}

static void fetch_data_no_param(char* url_param, char* main_text, char* format_string, char type) {
  char final_url[256];
  snprintf(final_url, sizeof(final_url), "%s?token=%s", url_param, API_KEY);
  char* response = get_json(final_url, 0);
  cJSON* json = cJSON_Parse(response);
  cJSON* obj;
  printf(MAGENTA "%s\n\n" RESET, main_text);
  cJSON_ArrayForEach(obj, json) {
    if (type == 's') {
      printf(format_string, obj->valuestring); 
    } else if (type == 'd') {
      printf(format_string, obj->valuedouble); 
    } else {
      printf(format_string, obj->valueint); 
    }
  }

  cJSON_Delete(json);
  cJSON_Delete(obj);
}

static void print_symbols_in_exchange(char* exchange, char* url_param, char* format_string) {
  char param_url[256];
  snprintf(param_url, sizeof(param_url), "%s/symbol?exchange=%s", url_param, exchange);
  char* final_url = append_url(param_url, strlen(param_url));
  char* props[2] = {"symbol", "description"};
  char text[64];
  snprintf(text, sizeof(text), format_string, exchange);
  char* format_strings[] = {
    BOST "Symbol:" BOEN " %s\n",
    BOST "Description:" BOEN " %s\n\n"
  };
  char ps[2] = {'s', 's'};

  print_json_data(final_url, text, props, format_strings, 2, INT_MAX, ps);
  free(final_url);
}
