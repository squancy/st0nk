#ifndef LIB_H
#define LIB_H

char* append_url(char* query, size_t len);
void print_h_line(int n);
int flag_exists(int argc, char* argv[], char* flag);
int return_pos(int argc, char* argv[], char* flag);
char* hist_url(char* url, char* url_param);
void today_minus_n_days(char date[64], int y, int m, int d, size_t size, int days);
void get_ymd(char* date, int* y, int* m, int* d);
int date_diff(char* date1_ro, char* date2_ro);
int graph_symbol_supported(char* symbol);
int is_date_valid(char* date_ro);

#endif
