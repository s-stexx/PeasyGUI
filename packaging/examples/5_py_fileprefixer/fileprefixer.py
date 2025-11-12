# SPDX-License-Identifier: MIT
# Copyright (c) 2025 Stefan Stechert

# Practical example for renaming files by dragging and dropping them.
# Each row lets you drop files and define a prefix for renaming.
# Click “Add more prefixes” to add new rows dynamically.
# Renames files directly - no extra warning or dialog!
# Try it safely with the example/images/ folder included.


import socket
import time
import os

# Geometry = [left, top, width, height]

row_count = 0

##########################################################################################
###### GENERAL FUNCTIONS (could be copied to any project with PeasyGUI)
##########################################################################################

sock = None
##########################################################################################
def request(command):
  global sock
  
  sock.sendall(command.encode('utf-8'))
  reply = sock.recv(100000).decode('utf-8')

  if reply.startswith('ERROR`'):
    raise ValueError(reply)
    
  if command.startswith('get`'):
    reply = reply.replace('OK`', '', 1)[:-1]

  return reply

##########################################################################################
def connect_to_gui():
  global sock
  
  connected_successful = False
  
  while not connected_successful:
    time.sleep(0.1)  
    
    try:
      sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
      sock.connect(('127.0.0.1', 10000))
      connected_successful = True
    except:
      print('Try to connect ...')

##########################################################################################
def get_events_list():
  global sock
  
  events = []
  events_str = request('events?')
  if not events_str == 'NONE':
    events = events_str.strip().split('\n')
  
  return events

##########################################################################################
def ev_val_str(event):
  parts = event.split('`')
  
  return '`'.join(parts[2:-1])

##########################################################################################
def int_list(numbers_str):
  number_str_list = numbers_str.split(',')
  
  return [int(x) for x in number_str_list]

##########################################################################################
def ev_val_int_list(event):
  val_str = ev_val_str(event)
  
  return int_list(val_str)

##########################################################################################
###### SPECIFIC FUNCTIONS (for this example)
##########################################################################################

##########################################################################################
def create_add_button_and_set_window_props():
  request('new`add_button`Type`Button`' + \
          'set`add_button`Geometry`10,10,140,32`' + \
          'set`add_button`Text`Add more prefixes`' + \
          '' + \
          'set`Window`Events`Closed`' + \
          'set`Window`AboutHeading`NorthEast`')

##########################################################################################
def create_another_drop_prefix(prefix):
  global row_count
  
  request_str = f"""
    set`Window`Geometry`50,50,340,{130 + row_count * 80}`
    
    new`DropArea_{row_count}`Type`DropArea`
    set`DropArea_{row_count}`Geometry`10,{50 + row_count * 80},80,70`
    set`DropArea_{row_count}`Text`Drop file(s)
    here`
    
    new`Label_{row_count}`Type`Label`
    set`Label_{row_count}`Geometry`100,{50 + row_count * 80},120,70`
    set`Label_{row_count}`Text`... to rename
    them with prefix ...`
    
    new`TextBox_{row_count}`Type`TextBox`
    set`TextBox_{row_count}`Geometry`220,{70 + row_count * 80},100,32`
    set`TextBox_{row_count}`Text`{prefix}`
    """
  
  # remove indentation
  request_str = request_str.replace('    ', '')
  
  request(request_str)
  
  row_count = row_count + 1
  
##########################################################################################
def rename_files(event):
  dropped_paths = ev_val_str(event).split('`')

  prefix = get_prefix_from_row(event)
  
  path_errors = []
  
  for path in dropped_paths:
    if os.path.isfile(path):
      rename_with_prefix(path, prefix)
    
    else:
      path_errors.append(path)
  
  if path_errors:
    error_msg = 'The following dropped items were no files: \n'
    error_str = '|\n|'.join(path_errors)
    request(f'set`Window`MessageBoxNow`{error_msg}|{error_str}|`')

##########################################################################################
def rename_with_prefix(path, prefix):
  directory = os.path.dirname(path)
  file = os.path.basename(path)
  new_name = prefix + file
  new_path = os.path.join(directory, new_name)
  os.rename(path, new_path)
  
  return new_path

##########################################################################################
def get_prefix_from_row(event):
  num_str = event.split('`')[0].replace('DropArea_', '')
  
  return request(f'get`TextBox_{num_str}`Text`')
 
##########################################################################################
connect_to_gui()
create_add_button_and_set_window_props()

create_another_drop_prefix('holiday_')
create_another_drop_prefix('delete_')

while True:
  events = get_events_list()
  
  for event in events:
    if event.startswith('DropArea_'):
      rename_files(event)
      
    elif event.startswith('add_button`Clicked'):
      create_another_drop_prefix('placeholder_')
        
    elif event.startswith('Window`Closed`'):
      request('set`Window`QuitNow`True`')
      sock.close()
      quit()
    
    
  time.sleep(0.05)