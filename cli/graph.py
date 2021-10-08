import pandas as pd
import mplfinance as mpf
import pylib.colors as terminal_colors
import time
import numpy as np
from PIL import ImageDraw
from PIL import ImageFont
from PIL import Image

COLORS = [
  (107,142,35), (124,252,0), (173,255,47), (0,255,255), (127,255,212), (176,224,230),
  (0,0,205), (138,43,226), (123,104,238), (139,0,139), (255,0,255), (255,182,193),
  (250,235,215), (210,105,30), (245,245,245), (230,230,250), (255,239,213), (255,215,0)
]

def add_horizontal_line(val, indicators, pnum, apds, color='#FF00FF'):
  line = [val for x in range(len(indicators[0][list(indicators[0].keys())[0]]))]
  df = pd.DataFrame(line, columns=['Price'])
  apds.append(mpf.make_addplot(df, panel=pnum, color=color, secondary_y=False))

def get_stock_data(indicators, j):
  indname = list(indicators[j].keys())[0]
  indicator = indicators[j][indname]
  ind_data = []
  for price in indicator:
    ind_data.append([price]) 
  return ind_data

def get_price_type(tp):
  if tp == 'c':
    return 'close'
  elif tp == 'o':
    return 'open'
  elif tp == 'h':
    return 'high'
  return 'low'

def get_ind_type(indicators):
  if len(indicators) == 0:
    return ""
  ind_str = list(indicators[0].keys())[0]
  if ind_str in ['SMA', 'EMA', 'WMA', 'DEMA', 'TEMA', 'KAMA']:
    return 'MA'
  elif ind_str == 'MACD':
    return 'MACD'
  return 'LVL'

def get_entry_index(trade_signals):
  for i in range(len(trade_signals)):
    if trade_signals[i]['type'] == 'buy':
      return i 

def get_exit_index(trade_signals):
  for i in range(len(trade_signals) - 1, 0, -1):
    if trade_signals[i]['type'] == 'sell':
      return i 

def format_change(change):
  if change > 0:
    return terminal_colors.GREEN + '+' + str(change) + '%' + terminal_colors.ENDC
  return terminal_colors.RED + str(change) + '%' + terminal_colors.ENDC

def get_oscillator_bounds(indname):
  if indname == 'STOCH':
    return [20, 80]
  elif indname == 'RSI':
    return [30, 70]
  return [-80, -20]

def draw_graph(dataset, chart_type, indicators, time_intervals, is_backtest, money, tp):
  """
    Create a candlestick chart with optional indicators
    Arguments are passed in from C
  """
  # invariant: when backtesting we can assume that exactly one indicator is passed in
  # save data as a pandas data frame
  price_type = get_price_type(tp)
  ind_type = get_ind_type(indicators)
  stock_data = []
  largest_price = 0
  largest_price_date = ""
  lowest_price = dataset[0]['low']
  lowest_price_date = dataset[0]['date'].split('T')[0]
  first_date = dataset[0]['date'].split('T')[0]
  last_date = dataset[len(dataset) - 1]['date'].split('T')[0]
  title = "\n\n" + dataset[0]['symbol'] + " (" + first_date + ", " + last_date + ")"
  symbol_name = dataset[0]['symbol']
  i = 0
  trade_signals = []
  ind_type = get_ind_type(indicators)
  signal_points = [np.nan, np.nan, np.nan]
  for entry in dataset:
    date = entry['date'].split('T')[0]
    volume = entry['volume']
    openp = entry['open']
    high = entry['high']
    low = entry['low']
    close = entry['close']
    stock_data.append([date, volume, openp, high, low, close])
    
    # generate sell and buy signals when backtesting
    if is_backtest:
      cp = entry[price_type]
      indb = indicators[0][list(indicators[0].keys())[0]]

      stype = None
      # backtesting a moving average: if MA crosses the price and stays above/below it for at least 2 time periods then it's a signal
      if i >= 3 and ind_type == 'MA':
        if (cp > indb[i] and dataset[i - 1][price_type] > indb[i - 1] and dataset[i - 2][price_type] < indb[i - 2]
          and dataset[i - 3][price_type] < indb[i - 3]):
          stype = 'buy'
        elif (cp < indb[i] and dataset[i - 1][price_type] < indb[i - 1] and dataset[i - 2][price_type] > indb[i - 2]
          and dataset[i - 3][price_type] > indb[i - 3]): 
          stype = 'sell'
      # backtesting MACD: if MACD line crosses the signal line and stays above/below it for at least 2 time periods then it's a signal
      # basically implements MACD crossovers
      elif i >= 3 and ind_type == 'MACD':
        indb2 = indicators[1][list(indicators[1].keys())[0]] # signal line
        if indb[i] > indb2[i] and indb[i - 1] > indb2[i - 1] and indb[i - 2] < indb2[i - 2] and indb[i - 3] < indb2[i - 3]:
          stype = 'buy'
        elif indb[i] < indb2[i] and indb[i - 1] < indb2[i - 1] and indb[i - 2] > indb2[i - 2] and indb[i - 3] > indb2[i - 3]:
          stype = 'sell'
      # backtesting oscillators: if the indicator crosses the lower/upper bound a signal is generated
      elif i >= 3 and ind_type == 'LVL':
        indname = list(indicators[0].keys())[0]
        lower_bound, upper_bound = get_oscillator_bounds(indname)

        # make sure both lines (%K and %D) are above/below the threshold level
        if indname == 'STOCH': 
          indb2 = indicators[1][list(indicators[1].keys())[0]] # %D
          if (indb[i] > lower_bound and indb2[i] > lower_bound and indb[i - 1] > lower_bound and indb2[i - 1] > lower_bound
            and indb[i - 2] < lower_bound and indb2[i - 2] < lower_bound and indb[i - 3] < lower_bound and indb2[i - 3] < lower_bound):
            stype = 'buy'
          elif (indb[i] < upper_bound and indb2[i] < upper_bound and indb[i - 1] < upper_bound and indb2[i - 1] < upper_bound
            and indb[i - 2] > upper_bound and indb2[i - 2] > upper_bound and indb[i - 3] > upper_bound and indb2[i - 3] > upper_bound):
            stype = 'sell'
        else:
          if indb[i] > lower_bound and indb[i - 1] > lower_bound and indb[i - 2] < lower_bound and indb[i - 3] < lower_bound:
            stype = 'buy'
          elif indb[i] < upper_bound and indb[i - 1] < upper_bound and indb[i - 2] > upper_bound and indb[i - 3] > upper_bound:
            stype = 'sell'

      signal_price = indb[i]
      if ind_type == 'LVL':
        signal_price = cp

      if i >= 3:
        if stype:
          trade_signals.append({'date': date, 'price': close, 'type': stype})
          signal_points.append(signal_price * 0.99)
        else:
          signal_points.append(np.nan)

    if high > largest_price:
      largest_price = high
      largest_price_date = date
    
    if low < lowest_price:
      lowest_price = low
      lowest_price_date = date

    i += 1
    
  df = pd.DataFrame(stock_data, columns=['Date', 'Volume', 'Open', 'High', 'Low', 'Close'])
  df['Date'] = pd.to_datetime(df['Date'], format='%Y-%m-%d')
  df.set_index('Date', inplace=True)

  # also add every indicator to the graph
  apds = []
  c_ind = 0
  y_pos = 150
  x_pos = 10
  positions = []
  texts = []
  rgbs = []
  j = 0
  pnum = 2
  
  # when backtesting add a new plot that illustrates the buy and sell signals
  if len(signal_points) > 4:
    apds.append(mpf.make_addplot(signal_points, type='scatter', markersize=200, marker='^'))

  while j < len(indicators): 
    indname = list(indicators[j].keys())[0]
    ind_data = get_stock_data(indicators, j)
    dfr = pd.DataFrame(ind_data, columns=[indname])
    if indname == "MACD":
      pnum_before = pnum
      pnum = 0
      hist = []
      texts.append("● " + indname + ": macd line (" + str(time_intervals[j]) + " days)")
      texts.append("● " + indname + ": signal line (" + str(time_intervals[j + 1]) + " days)")
      js = j

      for x in range(j, j + 2):
        ind_data = get_stock_data(indicators, x)
        dfm = pd.DataFrame(ind_data, columns=[indname])
        hex_color = '#%02x%02x%02x' % COLORS[c_ind]
        apds.append(mpf.make_addplot(dfm, panel=pnum, color=hex_color))
        rgbs.append(COLORS[c_ind])
        positions.append((x_pos, y_pos))
        j += 1
        c_ind += 1
        y_pos += 40

      pnum = pnum_before

      # add another panel for certain indicators
      for i in range(len(indicators[js + 1]['MACD'])):
        p1 = indicators[js]['MACD'][i]
        p2 = indicators[js + 1]['MACD'][i]
        hist.append([p1 - p2]) 
      dfh = pd.DataFrame(hist, columns=['MACD Hist'])
      apds.append(mpf.make_addplot(dfh, type='bar', width=0.7, panel=pnum, color='red', alpha=1, secondary_y=False))
      pnum += 1
    elif indname == "STOCH":
      texts.append("● " + indname + ": fast stoch (" + str(time_intervals[j]) + " days)")
      texts.append("● " + indname + ": slow stoch (" + str(time_intervals[j + 1]) + " days)")
      for x in range(j, j + 2):
        ind_data = get_stock_data(indicators, x)
        dfs = pd.DataFrame(ind_data, columns=[indname])
        hex_color = '#%02x%02x%02x' % COLORS[c_ind]
        apds.append(mpf.make_addplot(dfs, panel=pnum, color=hex_color))
        positions.append((x_pos, y_pos))
        rgbs.append(COLORS[c_ind])
        y_pos += 40
        c_ind += 1
        j += 1

      # add 2 horizontal lines at levels 20 and 80
      add_horizontal_line(20, indicators, pnum, apds)
      add_horizontal_line(80, indicators, pnum, apds)
      pnum += 1
    elif indname == "RSI":
      texts.append("● " + indname + " (" + str(time_intervals[j]) + " days)")
      positions.append((x_pos, y_pos))
      rgbs.append(COLORS[c_ind])
      hex_color = '#%02x%02x%02x' % COLORS[c_ind]
      apds.append(mpf.make_addplot(dfr, panel=pnum, color=hex_color, secondary_y=False))
      y_pos += 40
      c_ind += 1

      # add 2 horizontal lines at levels 30 and 70
      add_horizontal_line(30, indicators, pnum, apds)
      add_horizontal_line(70, indicators, pnum, apds)
      j += 1
      pnum += 1
    elif indname == "WillR":
      texts.append("● " + indname + " (" + str(time_intervals[j]) + " days)")
      positions.append((x_pos, y_pos))
      rgbs.append(COLORS[c_ind])
      hex_color = '#%02x%02x%02x' % COLORS[c_ind]
      apds.append(mpf.make_addplot(dfr, panel=pnum, color=hex_color))

      # add 2 horizontal lines at levels -20 and -80
      add_horizontal_line(-20, indicators, pnum, apds)
      add_horizontal_line(-80, indicators, pnum, apds)

      y_pos += 40
      c_ind += 1
      j += 1
      pnum += 1
    else:
      pnum_before = pnum
      pnum = 0
      texts.append("● " + indname + " (" + str(time_intervals[j]) + " days)")
      positions.append((x_pos, y_pos))
      rgbs.append(COLORS[c_ind])
      hex_color = '#%02x%02x%02x' % COLORS[c_ind]
      apds.append(mpf.make_addplot(dfr, panel=pnum, color=hex_color))
      y_pos += 40
      c_ind += 1
      j += 1
      pnum = pnum_before
  
  postfix = '_backtest' if is_backtest else ''
  fname = symbol_name + '_' + str(int(time.time())) + postfix + '.png'
  s = mpf.make_mpf_style(base_mpf_style='mike', rc={'font.size': 18})
  
  mpf.plot(df, addplot=apds, show_nontrading=True, style=s, type=chart_type, volume=True, ylabel_lower='Volume',
    title=title, figscale=2, returnfig=True, block=False, savefig='graphs/' + fname, volume_panel=1)
  
  # add text to the generated graph
  img = Image.open('graphs/' + fname) 
  draw = ImageDraw.Draw(img) 
  font = ImageFont.truetype(r'fonts/arial.ttf', 18)

  for i in range(len(positions)):
    current_pos = positions[i]
    current_text = texts[i]
    current_rgb = rgbs[i]
    draw.text(current_pos, current_text, current_rgb, font=font)

  draw.text((290, 110), 'Absolute high: ' + str(largest_price) + " (" + largest_price_date + ")", (255, 255, 255), font=font)
  draw.text((700, 110), 'Absolute low: ' + str(lowest_price) + " (" + lowest_price_date + ")", (255, 255, 255), font=font)

  img.save('graphs/' + fname)

  # if backtesting, display all the buy and sell signals
  if is_backtest:
    initial_money = money
    print(terminal_colors.UNDERLINE + 'Initial money:' + terminal_colors.ENDC, '$' + str(money))
    print()

    # calculate entry and exit point indexes
    entry_index = get_entry_index(trade_signals)
    exit_index = get_exit_index(trade_signals)

    num_of_stocks = 0
    is_entry = 1
    buy_sell_toggle = 1
    for i in range(len(trade_signals)):
      if trade_signals[i]['type'] == 'buy':
        entry_str = ''
        change = '+0.00%'
        if i == entry_index:
          entry_str = ' <-- ' + terminal_colors.UNDERLINE + 'Entry point' + terminal_colors.ENDC
          is_entry = 0
        if buy_sell_toggle:
          num_of_stocks = money / trade_signals[i]['price']
          buy_sell_toggle = 0
        print(terminal_colors.GREEN + 'Buy signal:' + terminal_colors.ENDC + entry_str)
      else:
        if not is_entry and not buy_sell_toggle:
          prev_money = money
          money = trade_signals[i]['price'] * num_of_stocks
          change = format_change(round((money / prev_money - 1) * 100, 2))
          buy_sell_toggle = 1
        exit_str = ''
        if i == exit_index:
          exit_str = ' <-- ' + terminal_colors.UNDERLINE + 'Exit point' + terminal_colors.ENDC
        print(terminal_colors.RED + 'Sell signal:' + terminal_colors.ENDC + exit_str)
      print('\t- Date:', trade_signals[i]['date'])
      print('\t- Price:', '$' + str(trade_signals[i]['price']))
      if not is_entry:
        print('\t- Current money:', '$' + str(round(money, 2)), '(' + change + ')')
    

    print('Total gain:', '$' + str(round(money, 2)), '(' + format_change(round((money / initial_money - 1) * 100, 2)) + ')')
