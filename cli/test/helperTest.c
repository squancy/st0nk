#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "../headers/colors.h"
#include "../headers/type.h"
#include "../headers/helpers.h"
#include "../headers/const.h"
#include "../headers/lib.h"

int main(void) {
  StockData* sd[10];  
  double numbers[10] = {
    44.82, 73.62, 44.24, 43.59, 33.4, 1.47, 97.96, 87.23, 50.02, 5.6
  };

  fill_closing_prices(numbers, 10, sd);
  int status;
  
  printf(MAGENTA "Testing double highest_price(int time_period, StockData* sd[], int sd_size, char type)\n" RESET);
  double highest = highest_price(2, sd, 10, 'c');
  status = fabs(highest - 50.02) < EPSILON;
  printf("INPUT: %s\nOUTPUT: %f\n", arr_to_str(numbers, 10), highest);
  print_status(status);
  print_h_line(80);

  printf(MAGENTA "Testing double lowest_price(int time_period, StockData* sd[], int sd_size, char type)\n" RESET);
  double lowest = lowest_price(3, sd, 5, 'c');
  status = fabs(lowest - 33.4) < EPSILON;
  printf("INPUT: %s\nOUTPUT: %f\n", arr_to_str(numbers, 10), lowest);
  print_status(status);
  print_h_line(80);

  printf(MAGENTA "Testing char* extract_indname(char* param)\n" RESET);
  char* params[] = {"RSI,12", "SMA,50", "EMA,12", "MACD,12,24"};
  char* outputs[] = {"RSI", "SMA", "EMA", "MACD"};
  for (int i = 0; i < 4; i++) {
    char* res = extract_indname(params[i]);
    status = strcmp(res, outputs[i]) == 0;
    printf("INPUT: %s\nOUTPUT: %s\n", params[i], res);
    print_status(status);
    free(res);
  }
  print_h_line(80);

  printf(MAGENTA "int is_ind_valid(int argc, char* argv[], int ipos)\n" RESET);
  int counts[] = {3, 4, 4, 5, 2};
  char* args1[] = {"--indicator", "RSI,2", "EMA,12"};
  char* args2[] = {"--indicator", "DEMA,12", "--from 2021-01-01", "--to 2021-03-01"};
  char* args3[] = {"--indicator", "TEMA,24", "--from 2021-01-01", "abcd"};
  char* args4[] = {"--indicator", "SMA,24", "MACD,12,26", "hello", "--from"};
  char* args5[] = {"--indicator", "SMA,11"};
  char** args[] = {args1, args2, args3, args4, args5};
  int ans[] = {1, 1, 1, 0, 1};
  for (int i = 0; i < 5; i++) {
    int res = is_ind_valid(counts[i], args[i], 0);
    status = res == ans[i];
    char buf[256];
    int k = 0;
    for (int j = 0; j < counts[i]; j++) {
      strcat(buf, args[i][j]);
      if (j != counts[i] - 1) {
        strcat(buf, ", ");
      }
    }
    printf("INPUT: %s\nOUTPUT: %d\n", buf, res);
    print_status(status);
    memset(buf, 0, sizeof(buf));
  }

  return 0;
}
