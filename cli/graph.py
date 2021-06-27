import pandas as pd
import mplfinance as mpf
import time
from PIL import ImageDraw
from PIL import ImageFont
from PIL import Image

def get_stock_data(indicators, j):
  indname = list(indicators[j].keys())[0]
  indicator = indicators[j][indname]
  ind_data = []
  for price in indicator:
    ind_data.append([price]) 
  return ind_data

def draw_graph(dataset, chart_type, indicators, time_intervals):
  """
    Create a candlestick chart with optional indicators
    Arguments are passed in from C
  """
  print(indicators)
  # save data as a pandas data frame
  stock_data = []
  largest_price = 0
  largest_price_date = ""
  lowest_price = dataset[0]['low']
  lowest_price_date = dataset[0]['date'].split('T')[0]
  first_date = dataset[0]['date'].split('T')[0]
  last_date = dataset[len(dataset) - 1]['date'].split('T')[0]
  title = "\n\n" + dataset[0]['symbol'] + " (" + first_date + ", " + last_date + ")"
  symbol_name = dataset[0]['symbol']
  for entry in dataset:
    date = entry['date'].split('T')[0]
    volume = entry['volume']
    openp = entry['open']
    high = entry['high']
    low = entry['low']
    close = entry['close']
    stock_data.append([date, volume, openp, high, low, close])

    if high > largest_price:
      largest_price = high
      largest_price_date = date
    
    if low < lowest_price:
      lowest_price = low
      lowest_price_date = date
    
  df = pd.DataFrame(stock_data, columns=['Date', 'Volume', 'Open', 'High', 'Low', 'Close'])
  df['Date'] = pd.to_datetime(df['Date'], format='%Y-%m-%d')
  df.set_index('Date', inplace=True)

  colors = [
    (107,142,35), (124,252,0), (173,255,47), (0,255,255), (127,255,212), (176,224,230),
    (0,0,205), (138,43,226), (123,104,238), (139,0,139), (255,0,255), (255,182,193),
    (250,235,215), (210,105,30), (245,245,245), (230,230,250), (255,239,213), (255,215,0)
  ]

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
  while j < len(indicators): 
    indname = list(indicators[j].keys())[0]
    ind_data = get_stock_data(indicators, j)
    dfr = pd.DataFrame(ind_data, columns=['Price'])
    if indname == "MACD":
      pnum_before = pnum
      pnum = 0
      hist = []
      texts.append("● " + indname + ": signal line (" + str(time_intervals[j]) + " days)")
      texts.append("● " + indname + ": macd line (" + str(time_intervals[j]) + " days)")
      js = j

      for x in range(j, j + 2):
        ind_data = get_stock_data(indicators, x)
        dfm = pd.DataFrame(ind_data, columns=['Price'])
        hex_color = '#%02x%02x%02x' % colors[c_ind]
        apds.append(mpf.make_addplot(dfm, panel=pnum, color=hex_color))
        rgbs.append(colors[c_ind])
        j += 1
        c_ind += 1
        positions.append((x_pos, y_pos))
        y_pos += 40
        c_ind += 1

      pnum = pnum_before

      # add another panel for certain indicators
      for i in range(len(indicators[js + 1]['MACD'])):
        p1 = indicators[js]['MACD'][i]
        p2 = indicators[js + 1]['MACD'][i]
        hist.append([p1 - p2]) 
      dfh = pd.DataFrame(hist, columns=['Price'])
      apds.append(mpf.make_addplot(dfh, type='bar', width=0.7, panel=pnum, color='red', alpha=1, secondary_y=False))
      pnum += 1
    elif indname == "STOCH":
      texts.append("● " + indname + ": fast stoch (" + str(time_intervals[j]) + " days)")
      texts.append("● " + indname + ": slow stoch (" + str(time_intervals[j]) + " days)")
      for x in range(j, j + 2):
        ind_data = get_stock_data(indicators, x)
        dfs = pd.DataFrame(ind_data, columns=['Price'])
        hex_color = '#%02x%02x%02x' % colors[c_ind]
        apds.append(mpf.make_addplot(dfs, panel=pnum, color=hex_color))
        j += 1
        positions.append((x_pos, y_pos))
        rgbs.append(colors[c_ind])
        y_pos += 40
        c_ind += 1

      # add 2 horizontal lines at levels 20 and 80
      line20 = [20 for x in range(len(indicators[0][list(indicators[0].keys())[0]]))]
      line80 = [80 for x in range(len(indicators[0][list(indicators[0].keys())[0]]))]
      df20 = pd.DataFrame(line20, columns=['Price'])
      df80 = pd.DataFrame(line80, columns=['Price'])
      apds.append(mpf.make_addplot(df20, panel=pnum, color='#FF00FF'))
      apds.append(mpf.make_addplot(df80, panel=pnum, color='#FF00FF'))
      pnum += 1
    elif indname == "RSI":
      texts.append("● " + indname + " (" + str(time_intervals[j]) + " days)")
      positions.append((x_pos, y_pos))
      rgbs.append(colors[c_ind])
      y_pos += 40
      c_ind += 1
      hex_color = '#%02x%02x%02x' % colors[c_ind]
      apds.append(mpf.make_addplot(dfr, panel=pnum, color=hex_color))

      # add 2 horizontal lines at levels 30 and 70
      line30 = [30 for x in range(len(indicators[0][list(indicators[0].keys())[0]]))]
      line70 = [70 for x in range(len(indicators[0][list(indicators[0].keys())[0]]))]
      df30 = pd.DataFrame(line30, columns=['Price'])
      df70 = pd.DataFrame(line70, columns=['Price'])
      apds.append(mpf.make_addplot(df30, panel=pnum, color='#FF00FF'))
      apds.append(mpf.make_addplot(df70, panel=pnum, color='#FF00FF'))
      j += 1
      pnum += 1
    else:
      pnum = 0
      texts.append("● " + indname + " (" + str(time_intervals[j]) + " days)")
      positions.append((x_pos, y_pos))
      rgbs.append(colors[c_ind])
      y_pos += 40
      c_ind += 1
      hex_color = '#%02x%02x%02x' % colors[c_ind]
      apds.append(mpf.make_addplot(dfr, panel=pnum, color=hex_color))
      j += 1

  fname = symbol_name + '_' + str(int(time.time())) + '.png'
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
  draw.text((700, 110), 'Absolute low: ' + str(lowest_price) + "(" + lowest_price_date + ")", (255, 255, 255), font=font)

  img.save('graphs/' + fname)
