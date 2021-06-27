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

  printf(MAGENTA "Testing void SMA(int time_period, StockData* sd[], size_t sd_size, double output[], char type)\n" RESET);
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

  printf(MAGENTA "\nTesting void EMA(int time_period, StockData* sd[], size_t sd_size, double output[], char type)\n" RESET);
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

  printf(MAGENTA "\nTesting void WMA(int time_period, StockData* sd[], size_t sd_size, double output[], char type)\n" RESET);
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
    24.273264197530864, 66.08325157750343, 35.45844023776865, 31.22869739368999, 27.62895641924503,
    49.373631939575596, 26.120861955325232, -4.6426871438221795, 20.23270071698144, 35.87812857545177,
    58.15852667083283, 87.42331174979854
  };

  double dema10[] = {
    34.0001414416059, 25.10274665334503
  };

  fill_closing_prices(numbers_dema, 20, sd);

  double* dema_res[] = {dema2, dema5, dema10};
  int lengths_dema[3] = {18, 12, 2}; 
  int time_periods_dema[] = {2, 5, 10};

  printf(MAGENTA "\nTesting void DEMA(int time_period, StockData* sd[], size_t sd_size, double output[], char type)\n" RESET);
  perform_test(time_periods_dema, sd, lengths_dema, numbers_dema, dema_res, 20, "dema");

  double numbers_tema[40] = {
    64.64, 82.72, 9.73, 81.62, 90.07, 68.19, 29.72, 67.02, 8.02, 36.59, 88.65, 10.81,
    30.12, 35.65, 6.7, 41.14, 58.7, 8.75, 84.43, 94.64, 70.09, 60.02, 52.34, 42.93, 57.12,
    80.68, 92.16, 12.4, 9.76, 94.09, 1.67, 82.44, 15.87, 71.36, 64.55, 25.03, 6.1, 42.48, 83.54, 43.97
  };

  double tema5[] = {
    76.44519810597978, 93.30109271463812, 48.112361158886856,
    69.79406480373171, 29.48826812587744, 30.990705004967978, 95.5871457839458,
    58.743579154120184, 46.49820985704389, 55.63894996309667,
    14.978754680985254, 6.406156776224961, 15.934033226127909,
    -24.910511566122636, 28.988386425873074, 79.6479092291328,
    85.55300350047253, 71.4116499327304, 43.08719422901825, 35.526782177498944,
    57.995201659338115, 78.43645500172008, 122.78872473247047,
    57.37519884319218, 21.715121747303762, 71.2129338466388, 8.462060576336953,
    58.06839357857881
  };

  double tema10[] = {
    75.46308469355401, 90.49687678948368, 58.79811881759638, 42.8077169499612,
    33.24076848656781, 10.74352506115585, 10.784949561774745,
    17.86866112438502, -6.956968080898179, 18.907244421306004,
    49.91826984547274, 57.295877923391245, 63.21048764981412
  };

  double tema12[] = {
    65.73647253784958, 52.65343543642561, 43.82593115903596, 20.90531688544784,
    14.824490530966962, 21.413339864110746, 7.176760300884624
  };

  print_h_line(80);
  fill_closing_prices(numbers_tema, 40, sd);

  double* tema_res[] = {tema5, tema10, tema12};
  int lengths_tema[3] = {28, 13, 7}; 
  int time_periods_tema[] = {5, 10, 12};

  printf(MAGENTA "\nTesting void TEMA(int time_period, StockData* sd[], size_t sd_size, double output[], char type)\n" RESET);
  perform_test(time_periods_tema, sd, lengths_tema, numbers_tema, tema_res, 40, "tema");

  printf(MAGENTA "\nTesting void MACD(int time1, int time2, StockData* sd[], size_t sd_size, double signal_line[], double macd[], char type)\n" RESET);
  double* numbers_macd = numbers_tema;
  fill_closing_prices(numbers_macd, 40, sd);

  double signal_line[14];
  double macd[14];

  int status = 1;
  MACD(12, 26, sd, 40, signal_line, macd, 'c');

  double macd_res[14] = {
    52.349615, 55.298533, 52.120864, 48.983022, 52.324280, 48.572111, 51.080843,
    48.472633, 50.167993, 51.233327, 49.292340, 46.092907, 45.825284, 48.618967
  };

  double signal_res[14] = {
    48.274599, 63.554354, 55.684454, 48.619153, 55.614668, 47.315488, 52.719259,
    47.050143, 50.790121, 52.907025, 48.618252, 42.076982, 42.138985, 48.508372
  };

  for (int i = 0; i < 14; i++) {
    if ((fabs(signal_line[i] - signal_res[i]) > EPSILON) || (fabs(macd[i] - macd_res[i]) > EPSILON)) {
      status = 0;
      break;
    }
  }

  printf("INPUT: %s\nOUTPUT: %s, %s\n", arr_to_str(numbers_macd, 40), arr_to_str(signal_line, 14), arr_to_str(macd, 14));
  print_status(status);

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
