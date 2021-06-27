#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../headers/lib.h"
#include "../headers/colors.h"
#include "../headers/api.h"
#include "../headers/type.h"
#include "../headers/analysis.h"
#include "../headers/helpers.h"

int main(void) {
  char* q1 = "stock/profile?symbol=AAPL"; 
  char* q2 = "stock/profile?isin=US5949181045"; 
  char* q3 = "news?category=forex&minId=10"; 
  char* q4 = "company-news?symbol=AAPL&from=2021-03-01&to=2021-03-09"; 
  char* q5 = "press-releases?symbol=AAPL"; 

  char* q1s = "https://www.finnhub.io/api/v1/stock/profile?symbol=AAPL&token=" API_KEY;
  char* q2s = "https://www.finnhub.io/api/v1/stock/profile?isin=US5949181045&token=" API_KEY;
  char* q3s = "https://www.finnhub.io/api/v1/news?category=forex&minId=10&token=" API_KEY;
  char* q4s = "https://www.finnhub.io/api/v1/company-news?symbol=AAPL&from=2021-03-01&to=2021-03-09&token=" API_KEY;
  char* q5s = "https://www.finnhub.io/api/v1/press-releases?symbol=AAPL&token=" API_KEY;

  char* queries[] = {q1, q2, q3, q4, q5};
  char* expected_outputs[] = {q1s, q2s, q3s, q4s, q5s};

  printf(MAGENTA "Testing char* append_url(char* query, size_t len)\n" RESET);
  for (int i = 0; i < sizeof(queries) / sizeof(queries[0]); i++) {
    int status = 0; 
    char* url = append_url(queries[i], strlen(queries[i]));
    if (strcmp(url, expected_outputs[i]) == 0) {
      status = 1;
      free(url);
    }

    printf("INPUT: %s\nOUTPUT: %s\n", queries[i], url);
    print_status(status);
    print_h_line(80);
  }

  char* p1 = "--abc";
  char* p2 = "TSLA";
  char* p3 = "-h";
  
  char* inp[][4] = {
    {"sa", "-a",  "-n",  "--abc"},
    {"sa", "--help",  "x",  "AAPL"},
    {"sa", "--list", "-x", "y"}
  };

  char* params[] = {p1, p2, p3};
  int ans[] = {1, 0, 0};

  printf(MAGENTA "\nTesting int flag_exists(int argc, char* argv[], char* flag)\n" RESET);
  for (int i = 0; i < sizeof(ans) / sizeof(ans[0]); i++) {
    int res = flag_exists(4, inp[i], params[i]);
    int status = res == ans[i]; 
    printf("INPUT: [%s, %s, %s, %s]\nOUTPUT: %d\n", inp[i][0], inp[i][1], inp[i][2], inp[i][2], res);
    print_status(status);
    print_h_line(80);
  }

  char* f1 = "--list";
  char* f2 = "-e";
  char* f3 = "-x";
  
  char* list_of_flags[][4] = {
    {"sa", "-e",  "-list",  "--list"},
    {"sa", "--help",  "e",  "MSFT"},
    {"sa", "x", "-e", "--list"}
  };

  char* flags[] = {f1, f2, f3};
  int outs[] = {3, -1, -1};

  printf(MAGENTA "\nTesting int return_pos(int argc, char* argv[], char* flag)\n" RESET);
  for (int i = 0; i < sizeof(outs) / sizeof(outs[0]); i++) {
    int res = return_pos(4, list_of_flags[i], flags[i]);
    int status = res == outs[i];

    printf("INPUT: [%s, %s, %s, %s]\nOUTPUT: %d\n", list_of_flags[i][0], list_of_flags[i][1], list_of_flags[i][2],
      list_of_flags[i][3], res);
    print_status(status);
    print_h_line(80);
  }

  char* url1 = "https://jsonplaceholder.typicode.com/posts/1";
  char* url2 = "https://jsonplaceholder.typicode.com/posts/2";
  char* url3 = "https://jsonplaceholder.typicode.com/posts/3";
  char* url4 = "https://jsonplaceholder.typicode.com/posts/does_not_exist1";
  char* url5 = "https://jsonplaceholder.typicode.com/posts/does_not_exist2";
  char* url6 = "https://abcdefgh.abcdefghijklmnxxxxx";

  char* responses[][1] = {
    {"{\n  \"userId\": 1,\n  \"id\": 1,\n  \"title\": \"sunt aut facere repellat provident occaecati excepturi optio reprehenderit\",\n  \"body\": \"quia et suscipit\\nsuscipit recusandae consequuntur expedita et cum\\nreprehenderit molestiae ut ut quas totam\\nnostrum rerum est autem sunt rem eveniet architecto\"\n}"},
    {"{\n  \"userId\": 1,\n  \"id\": 2,\n  \"title\": \"qui est esse\",\n  \"body\": \"est rerum tempore vitae\\nsequi sint nihil reprehenderit dolor beatae ea dolores neque\\nfugiat blanditiis voluptate porro vel nihil molestiae ut reiciendis\\nqui aperiam non debitis possimus qui neque nisi nulla\"\n}"},
    {"{\n  \"userId\": 1,\n  \"id\": 3,\n  \"title\": \"ea molestias quasi exercitationem repellat qui ipsa sit aut\",\n  \"body\": \"et iusto sed quo iure\\nvoluptatem occaecati omnis eligendi aut ad\\nvoluptatem doloribus vel accusantium quis pariatur\\nmolestiae porro eius odio et labore et velit aut\"\n}"},
		{"{}"},
		{"{}"},
		{"Curl perform error"}
  };

	char* urls[] = {url1, url2, url3, url4, url5, url6};

  printf(MAGENTA "\nTesting char* get_json(char* url, int include_headers)\n" RESET);
	for (int i = 0; i < 6; i++) {
		char* res = get_json(urls[i], 0);
		int status = strcmp(res, responses[i][0]) == 0;
    printf("INPUT: %s\nOUTPUT: %s\n", urls[i], res);
    print_status(status);
    print_h_line(80);
	}
  
  printf(MAGENTA "\nTesting void today_minus_n_days(char date[64], int y, int m, int d, size_t size, int days)\n" RESET);
  char date[64];
  today_minus_n_days(date, 2021, 3, 12, sizeof(date), 2);
  printf("INPUT: date[64]\nOUTPUT: %s\n", date);
  printf(GREEN "==> PASSED\n" RESET);
  print_h_line(80);

  char* params_[][2] = {
    {"intraday", "&symbol=AAPL"},
    {"eod", "&symbol=MSFT"},
    {"asbdb", "&asdh=abc&xy=ooo"}
  };
  
  char* results[] = {
    "http://api.marketstack.com/v1/intraday?access_key=d8c359234548676a4856cf13f019e3ed&symbol=AAPL",
    "http://api.marketstack.com/v1/eod?access_key=d8c359234548676a4856cf13f019e3ed&symbol=MSFT",
    "http://api.marketstack.com/v1/asbdb?access_key=d8c359234548676a4856cf13f019e3ed&asdh=abc&xy=ooo"
  };

  printf(MAGENTA "\nTesting char* hist_url(char* url, char* url_param)\n" RESET);

  for (int i = 0; i < 3; i++) {
    char* res = hist_url(params_[i][0], params_[i][1]);
    int status = strcmp(res, results[i]) == 0;
    printf("INPUT: %s, %s\nOUTPUT: %s\n", params_[i][0], params_[i][1], res);
    print_status(status);
    print_h_line(80);
    free(res);
  }

  printf(MAGENTA "\nTesting void get_ymd(char* date, int* y, int* m, int* d)\n" RESET);
  char* dates[] = {
    "2021-01-01",
    "2020-12-11",
    "1997-02-12",
    "1996-11-01"
  }; 

  int ymds[][3] = {
    {2021, 1, 1},
    {2020, 12, 11},
    {1997, 2, 12},
    {1996, 11, 1}
  };

  for (int i = 0; i < 4; i++) {
    int y, m, d;
    get_ymd(dates[i], &y, &m, &d);
    int status = 0;
    if (y == ymds[i][0] && m == ymds[i][1] && d == ymds[i][2]) {
      status = 1;
    }

    printf("INPUT: %s\nOUTPUT: %d, %d, %d\n", dates[i], y, m, d);
    print_status(status);
    print_h_line(80);
  }

  char* dates1[] = {
    "2021-02-14",
    "2020-04-03",
    "2020-11-30"
  };

  char* dates2[] = {
    "2021-02-06",
    "2019-01-03",
    "2021-01-03"
  };

  int diffs[] = {1, 1, 0};

  printf(MAGENTA "\nTesting int date_diff(char* date1_ro, char* date2_ro)\n" RESET);

  for (int i = 0; i < 3; i++) {
    int res = date_diff(dates1[i], dates2[i]);
    int status = res == diffs[i];
    printf("INPUT: %s, %s\nOUTPUT: %d\n", dates1[i], dates2[i], res);
    print_status(status);
    print_h_line(80);
  }

  char* tdates[] = {
    "2020-19-02",
    "1987-01-01",
    "0111-02-12",
    "1983-00-30",
    "1999-99-99",
    "1987-03-31",
    "1988-02-30",
    "1995-03-00",
    "2033-03-03"
  };

  int tdates_res[] = {0, 1, 0, 0, 0, 1, 0, 0, 0};

  printf(MAGENTA "\nTesting int is_date_valid(char* date_ro)\n" RESET);

  for (int i = 0; i < 9; i++) {
    int res = is_date_valid(tdates[i]); 
    int status = res == tdates_res[i];
    printf("INPUT: %s\nOUTPUT: %d\n", tdates[i], res);
    print_status(status);
    print_h_line(80);
  }
}
