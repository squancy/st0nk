Currently under development
<pre style="white-space: pre-wrap;">
  St0nk
  ========

  A free stock analyzer that has more or less the same functionality as other tools.
  It has both a CLI and GUI interface and supports the most commonly used indicators and
  candlestick patterns.

  You can also backtest your trading strategy to see how well it performs on historical data.
  Note that stock data is only available from the last 365 days since the data is fetched
  from 3rd-party services with a free plan. (Marketstack and Finnhub)

  Compiling
  =========

  Make sure you have cJSON and curl installed on your system as shared libraries so that
  they can be referenced as lcjson and lcurl.
  Next install the python3-dev package so that you will have the <Python.h> header ready
  to be included in C. It is required for the communication between C and Python.
  You can test the path where the header resides by running python3-config --includes.
  You can then run make and it should auto compile for you. If not, you may need to
  manually give the location of the python header file.

  Features
  =========

  Both the CLI and the GUI has the same features:
  - Display a graph about a certain stock with given indicators
  - Candlestick pattern recognition
  - Backtesting your trading strategy
  - Fetching fresh market news, upcoming IPOs etc.
 
  3rd party data providers
  =========

  Data is fetched from Finnhub and Marketstack. You can create an account and subscribe to
  their free plans which is fair enough for personal use. Place your API keys in headers/api.h

  Most financial softwares are too corrupt and expensive so let's make a better, free version.
</pre>
