#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../headers/analysis.h"
#include "../headers/colors.h"
#include "../headers/lib.h"
#include "../headers/helpers.h"
#include "../headers/const.h"

void perform_test(int time_periods[], StockData* sd[], int lengths[], double numbers[], double* results[], size_t size, char* type);

int main(void) {
  double numbers[] = {
    910.58, 890.4, 722.64, 431.47, 984.46, 345.11, 822.36, 670.2, 900.38, 300.37,
    587.37, 765.25, 69.47, 68.34, 949.74, 802.91, 118.09, 136.36, 518.69, 108.52 
  }; 

  StockData* sd[20];
  fill_closing_prices(numbers, 20, sd);

  double sma10[] = {
    697.797, 665.476, 652.961, 587.644, 551.331, 547.859, 593.639, 523.212, 469.828, 431.659, 412.474
  };

  double sma20[] = {
    694.216, 624.123, 555.618, 574.543, 605.497, 533.299, 515.903, 490.597, 443.791
  };

  double sma50[] = {
    627.876, 620.698, 569.211, 530.125, 535.94, 477.544
  };

  double* results[] = {sma10, sma20, sma50};
  int lengths[3] = {11, 9, 6}; 
  int time_periods[] = {10, 12, 15};

  printf(MAGENTA "Testing int SMA(int time_period, StockData* sd[], size_t sd_size, double output[], char type)\n" RESET);
  perform_test(time_periods, sd, lengths, numbers, results, 20, "sma");

  double numbers_ema[20] = {
    22.38, 67.88, 1.16, 41.91, 63.17, 34.36, 31.88, 16.15, 41.7, 17.79,
    75.81, 39.81, 36.74, 99.33, 60.81, 25.51, 72.35, 6.76, 91.83, 0.61
  };

  double ema3[] = {
    30.47333333333333, 36.19166666666666, 49.68083333333333, 42.02041666666666, 36.95020833333333,
    26.550104166666664, 34.12505208333333, 25.957526041666664, 50.883763020833335, 45.346881510416665,
    41.04344075520834, 70.18672037760416, 65.49836018880208, 45.50418009440104, 58.92709004720052,
    32.843545023600264, 62.336772511800135, 31.473386255900067
  };

  double ema5[] = {
    39.3, 37.653333333333336, 35.72888888888889, 29.202592592592595, 33.3683950617284,
    28.175596707818936, 44.053731138545956, 42.639154092363974, 40.672769394909324,
    60.22517959660622, 60.42011973107081, 48.78341315404721, 56.63894210269814,
    40.012628068465425, 57.28508537897695, 38.39339025265131
  };

  double ema10[] = {
    33.838, 41.469272727272724, 41.1675867768595, 40.36257099924868, 51.083921726658005, 52.852299594538366,
    47.88097239553139, 52.32988650543477, 44.044452595355715, 52.73273394165467, 43.25587322499018
  };

  free_sd(sd, 20);

  fill_closing_prices(numbers_ema, 20, sd);

  double* ema_res[] = {ema3, ema5, ema10};
  int lengths_ema[3] = {18, 16, 11}; 
  int time_periods_ema[] = {3, 5, 10};

  printf(MAGENTA "\nTesting int EMA(int time_period, StockData* sd[], size_t sd_size, double output[], char type)\n" RESET);
  perform_test(time_periods_ema, sd, lengths_ema, numbers_ema, ema_res, 20, "ema");

  free_sd(sd, 20);

  double numbers_wma[20] = {
    25.83, 83.88, 50.83, 63.95, 4.37, 33.11, 66.92, 8.72, 90.52, 16.54,
    77.81, 76.79, 64.83, 97.39, 81.18, 80.85, 6.42, 78.97, 75.25, 45.76
  };

  double wma5[] = {
    41.582000, 37.361333, 43.925333, 32.220000, 50.588667, 42.526000, 54.075333,
    62.304667, 65.889333, 76.586667, 81.422667, 81.839333, 57.243333, 61.522000, 63.618000, 57.360000
  };

  double wma7[] = {
    46.101071, 36.535000, 48.030000, 40.792857, 50.097857, 58.652857, 61.631429,
    71.617143, 76.462143, 78.636786, 62.549286, 64.960714, 66.400714, 60.523214
  };

  double wma10[] = {
    42.557636, 48.620000, 53.551818, 56.438000, 64.989636, 69.986000, 73.525818, 62.664909, 66.094909, 67.571273, 63.963455 
  };

  fill_closing_prices(numbers_wma, 20, sd);
  
  double* wma_res[] = {wma5, wma7, wma10};
  int lengths_wma[3] = {16, 14, 11}; 
  int time_periods_wma[] = {5, 7, 10};

  printf(MAGENTA "\nTesting int WMA(int time_period, StockData* sd[], size_t sd_size, double output[], char type)\n" RESET);
  perform_test(time_periods_wma, sd, lengths_wma, numbers_wma, wma_res, 20, "wma");

  double numbers_dema[20] = {
    6.75, 19.7, 54.59, 51.9, 18.05, 96.23, 4, 22.74, 27.84, 67.48, 15.86, 0.55, 60.33, 55.79, 72.12, 97.6, 17.49, 31.88, 35.74, 64.7
  };

  double dema2[] = {
    -0.5633333333333326, 40.46518518518518, 63.95493827160493, -3.6283950617283836, 108.98163237311385,
    25.5153543667124, 21.070054869684498, 8.233886094599399, 73.69925129976798, 39.12906907822317,
    -15.231648536714324, 40.897893300165656, 44.47766759307869, 53.09423469536739, 120.7608600643473,
    40.38121396563516, 32.35182485882928, 21.809970576371178
  };

  double dema5[] = {
    27.222746, 22.456239, 48.693765, 35.465581, 12.260212, 34.361136, 42.732531, 54.145092, 84.271220, 53.037142, 42.771202, 35.925095
  };

  double dema10[] = {
    48.504613, 44.199133
  };

  fill_closing_prices(numbers_dema, 20, sd);

  double* dema_res[] = {dema2, dema5, dema10};
  int lengths_dema[3] = {18, 12, 2}; 
  int time_periods_dema[] = {2, 5, 10};

  printf(MAGENTA "\nTesting int DEMA(int time_period, StockData* sd[], size_t sd_size, double output[], char type)\n" RESET);
  perform_test(time_periods_dema, sd, lengths_dema, numbers_dema, dema_res, 20, "dema");

  double numbers_tema[40] = {
    64.64, 82.72, 9.73, 81.62, 90.07, 68.19, 29.72, 67.02, 8.02, 36.59, 88.65, 10.81,
    30.12, 35.65, 6.7, 41.14, 58.7, 8.75, 84.43, 94.64, 70.09, 60.02, 52.34, 42.93, 57.12,
    80.68, 92.16, 12.4, 9.76, 94.09, 1.67, 82.44, 15.87, 71.36, 64.55, 25.03, 6.1, 42.48, 83.54, 43.97
  };

  double tema5[] = {
    27.190469, 24.961703, 27.860832, 10.261755, 28.772534, 48.841865, 24.383462, 68.328153, 94.383723,
    86.206490, 72.949691, 59.868499, 47.268962, 52.958352, 73.443570, 87.923103, 35.698736, 13.101430,
    62.111019, 21.290869, 59.158951, 30.698526, 57.797598, 63.917220, 36.772762, 11.965556, 29.599939, 66.974399
  };

  double tema10[] = {
    79.751931, 51.504511, 31.560723, 56.322240, 31.530723, 51.457366, 35.058289, 49.441225, 55.982760,
    42.209132, 24.492946, 29.664085, 52.046654
  };

  double tema12[] = {
    38.070767, 49.657830, 55.269761, 43.541670, 27.718653, 31.089386, 49.933775
  };

  print_h_line(80);
  fill_closing_prices(numbers_tema, 40, sd);

  double* tema_res[] = {tema5, tema10, tema12};
  int lengths_tema[3] = {28, 13, 7}; 
  int time_periods_tema[] = {5, 10, 12};

  printf(MAGENTA "\nTesting int TEMA(int time_period, StockData* sd[], size_t sd_size, double output[], char type)\n" RESET);
  perform_test(time_periods_tema, sd, lengths_tema, numbers_tema, tema_res, 40, "tema");

  printf(MAGENTA "\nTesting int MACD(int time1, int time2, StockData* sd[], size_t sd_size, double signal_line[], double macd[], char type)\n" RESET);
  double* numbers_macd = numbers_tema;
  fill_closing_prices(numbers_macd, 40, sd);

  double signal_line[14];
  double macd[14];

  int status = 1;
  MACD(12, 26, sd, 40, signal_line, macd, 'c');

  double macd_res[14] = {
    55.298533, 52.120864, 48.983022, 52.324280, 48.572111, 51.080843, 48.472633, 50.167993, 51.233327,
    49.292340, 46.092907, 45.825284, 48.618967, 48.274599
  };

  double signal_res[14] = {
    63.554354, 55.684454, 48.619153, 55.614668, 47.315488, 52.719259, 47.050143, 50.790121, 52.907025,
    48.618252, 42.076982, 42.138985, 48.508372, 47.810161
  };

  for (int i = 0; i < 14; i++) {
    if ((fabs(signal_line[i] - signal_res[i]) > EPSILON) || (fabs(macd[i] - macd_res[i]) > EPSILON)) {
      status = 0;
      break;
    }
  }

  printf("INPUT: %s\nOUTPUT: %s, %s\n", arr_to_str(numbers_macd, 40), arr_to_str(signal_line, 14), arr_to_str(macd, 14));
  print_status(status);

  printf(MAGENTA "\nTesting int RSI(int time_period, StockData* sd[], size_t sd_size, double output[])\n" RESET);

  double* numbers_rsi = numbers_tema;
  fill_closing_prices(numbers_rsi, 40, sd);

  double rsi8[] = {
    41.370481, 43.187122, 62.423651, 39.058009, 41.036988, 44.884935, 46.269769, 45.767092, 59.589041,
    45.126733, 49.317726, 67.346770, 58.095354, 54.846665, 59.915703, 50.436351, 49.608407, 70.510408,
    53.477283, 27.244051, 31.003212, 57.309590, 42.027754, 55.076449, 45.328743, 49.015756, 47.055185,
    51.473574, 49.588616, 43.498198, 61.847018
  };
  double rsi12[] = {
    46.587854, 45.228973, 49.662771, 45.084993, 46.273550, 43.380551, 55.626401, 53.008059, 57.308372,
    52.884402, 44.981618, 55.474876, 54.683922, 57.351837, 64.796731, 45.698893, 42.333480, 62.068673,
    38.825277, 48.616341, 44.385769, 51.073254, 51.157500, 48.394734, 45.463114, 46.678838, 49.287226
  };
  double rsi14[] = {
    44.638362, 46.265359, 54.884788, 42.398448, 49.484027, 52.472563, 53.874652, 49.291096, 55.009042,
    50.748984, 45.909126, 60.551512, 59.595991, 47.075398, 50.412199, 56.287553, 44.250197, 56.994779,
    43.377637, 47.932211, 49.491911, 46.955432, 46.053799, 49.963286, 52.064966
  };

  double* rsi_res[] = {rsi8, rsi12, rsi14};

  int time_periods_rsi[] = {8, 12, 14};
  int lengths_rsi[] = {31, 27, 25};
  perform_test(time_periods_rsi, sd, lengths_rsi, numbers_rsi, rsi_res, 40, "rsi");

  return 0;
}

void perform_test(int time_periods[], StockData* sd[], int lengths[], double numbers[], double* results[], size_t size, char* type) {
  for (int i = 0; i < 3; i++) {
    double res[size];
    if (strcmp(type, "sma") == 0) {
      SMA(time_periods[i], sd, size, res, 'c');
    } else if (strcmp(type, "ema") == 0) {
      EMA(time_periods[i], sd, size, res, 'c');
    } else if (strcmp(type, "wma") == 0) {
      WMA(time_periods[i], sd, size, res, 'c');
    } else if (strcmp(type, "dema") == 0) {
      DEMA(time_periods[i], sd, size, res, 'c');
    } else if (strcmp(type, "tema") == 0) {
      TEMA(time_periods[i], sd, size, res, 'c');
    } else if (strcmp(type, "rsi") == 0) {
      RSI(time_periods[i], sd, size, res);
    }

    int status = 1;
    for (int j = 0; j < lengths[i]; j++) {
      if (fabs(res[j] - results[i][j]) > EPSILON) {
        status = 0;
        break;
      }
    }

    char* inp = arr_to_str(numbers, size);
    char* out = arr_to_str(res, lengths[i]);
    printf("INPUT: %s\nOUTPUT: %s\n", inp, out);
    print_status(status);

    free(inp);
    free(out);

    if (i != 2) {
      print_h_line(80);
    }
  }
}
