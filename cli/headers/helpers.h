#ifndef HELPERS_H
#define HELPERS_H

void free_sd(StockData* sd[], int n);
void fill_closing_prices(double numbers[], int max, StockData* sd[]);
char* arr_to_str(double nums[], int len);
double get_price(char type, StockData* sd[], int i);
double highest_price(int time_period, StockData* sd[], int sd_size, char type);
double lowest_price(int time_period, StockData* sd[], int sd_size, char type);
void print_status(int status);
void print_usage(void);
void exit_wrong_arg(char* arg);
int chart_type_valid(char* chart_type);
int is_ind_valid(int argc, char* argv[], int ipos);
char* extract_indname(char* param);
int get_indicators(char* inds[], char* argv[], int ipos, int argc);
int perform_indicator(int time_intervals[], StockData* sd[], size_t sd_size, double output[], double out2[], char* ind_name, char type);

#endif
