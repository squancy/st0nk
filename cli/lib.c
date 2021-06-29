#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <cjson/cJSON.h>
#include <curl/curl.h>
#include "headers/api.h"

// Appends the base api url with the query part
char* append_url(char* query, size_t len) {
  size_t url_len = strlen(API_URL);
  size_t api_key_len = strlen(API_KEY);
  size_t token_url_len = strlen(TOKEN_URL);
  size_t final_len = (url_len + len + api_key_len + token_url_len + 1) * sizeof(char);
  char res[final_len];
  snprintf(res, sizeof(res), "%s%s%s%s", API_URL, query, TOKEN_URL, API_KEY);
  char* final_res = malloc(final_len);
  memcpy(final_res, res, final_len);
  final_res[final_len] = '\0';

  return final_res;
}

void print_h_line(int n) {
  while (n-- >= 0) {
    printf("=");
  }
  printf("\n");
}

// Check if flag exists in argv
int flag_exists(int argc, char* argv[], char* flag) {
  int i;
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], flag) == 0) {
      return 1;
    }
  }
  return 0;
}

// Return the index of a flag in argv
int return_pos(int argc, char* argv[], char* flag) {
  int i;
  for (i = 0; i < argc; i++) {
    if (strcmp(argv[i], flag) == 0) {
      return i;
    }
  }
  return -1;
}

// Return current date minus n days in YYYY-MM-DD format
void today_minus_n_days(char date[64], int y, int m, int d, size_t size, int days) {
  struct tm tm = {.tm_year = y - 1900, .tm_mon = m - 1, .tm_mday = d};
  if (y == 0 && m == 0 && d == 0) {
    time_t t = time(NULL);
    tm = *localtime(&t);
  }
  tm.tm_mday -= days; 
  mktime(&tm);
  int year = tm.tm_year + 1900;
  int month = tm.tm_mon + 1;
  int day = tm.tm_mday;
  snprintf(date, size, "%d-%02d-%02d", year, month, day);
}

char* hist_url(char* url, char* url_param) {
  char param_url[strlen(MS_API_URL) + strlen(ACCESS_KEY_URL) + strlen(ACCESS_KEY) + strlen(url) + strlen(url_param) + 1];
  snprintf(param_url, sizeof(param_url), "%s%s%s%s%s", MS_API_URL, url, ACCESS_KEY_URL, ACCESS_KEY, url_param);
  char* final_url = malloc(strlen(param_url) + 1);
  memcpy(final_url, param_url, strlen(param_url));
  final_url[strlen(param_url)] = '\0';
  return final_url;
}

// Given a string in the form "YYYY-MM-DD" extract the year, month and day
void get_ymd(char* date_ro, int* y, int* m, int* d) {
  int l = (strlen(date_ro) + 10) * sizeof(char);
  char date[l];
  strncpy(date, date_ro, l);
  const char* hypen = "-";
  char* token;
  token = strtok(date, hypen);
  int i = 0;
  while (token != NULL) {
    char str_to_conv[5];
    if (token[0] == '0') {
      str_to_conv[0] = token[1];
      str_to_conv[1] = '\0';
    } else {
      strcpy(str_to_conv, token);
    }

    int conv = atoi(str_to_conv);
    if (conv == 0) {
      *y = *m = *d = 0;  
    } else {
      if (i == 0) {
        *y = conv;
      } else if (i == 1) {
        *m = conv;
      } else {
        *d = conv;
      }
    }
    token = strtok(NULL, hypen);
    i++;
  }
}

// Returns 1 if date1 is later than date2 or they are the same and 0 otherwise
int date_diff(char* date1_ro, char* date2_ro) {
  char* dates[] = {date1_ro, date2_ro};
  const char* hypen = "-";
  int ns[2][3];
  for (int i = 0; i < 2; i++) {
    int l = (strlen(dates[i]) + 1) * sizeof(char); 
    char date[l];
    strncpy(date, dates[i], l);
    char* token = strtok(date, hypen);
    int j = 0;
    while (token != NULL) {
      int conv = atoi(token);
      ns[i][j] = conv;
      token = strtok(NULL, hypen);
      j++;
    }
  }

  for (int i = 0; i < 3; i++) {
    if (ns[0][i] > ns[1][i]) {
      return 1;
    } else if (ns[0][i] < ns[1][i]) {
      return 0;
    }
  }

  return ns[0][0] == ns[1][0] && ns[0][1] == ns[1][1] && ns[0][2] == ns[1][2];
}

// Returns 1 if the symbol is supported by Marketstack, 0 otherwise
int graph_symbol_supported(char* symbol) {
  char url[256];
  snprintf(url, sizeof(url), "tickers/%s", symbol);
  char* final_url = hist_url(url, "\0");
  char* response = get_json(final_url, 0);
  cJSON* json = cJSON_Parse(response);
  char* error = cJSON_Print(cJSON_GetObjectItemCaseSensitive(json, "error"));
  if (error == NULL) {
    return 1;
  } 
  return 0;
}

// Validate date in form YYYY-MM-DD
int is_date_valid(char* date_ro) {
  int l = strlen(date_ro);
  char date[l + 1];
  strncpy(date, date_ro, l);
  char* token;
  token = strtok(date, "-");
  int i = 0, year, month, day;
  while (token != NULL) {
    if (i == 0) {
      year = atoi(token);
    } else if (i == 1) {
      month = atoi(token);
    } else {
      day = atoi(token);
    }
    token = strtok(NULL, "-");
    i++;
  }

  struct tm input = {
    .tm_mday = day,
    .tm_mon = month - 1,
    .tm_year = year - 1900,
  };

  time_t t = mktime(&input); 
  struct tm *output = localtime(&t);

  if (day != output->tm_mday || month != output->tm_mon + 1 || year != output->tm_year + 1900) {
    return 0;
  }

  char today[64];
  today_minus_n_days(today, 0, 0, 0, sizeof(today), 0);

  if (date_diff(date_ro, today) && strcmp(date_ro, today) != 0) {
    return 0;
  }

  return 1;
}
