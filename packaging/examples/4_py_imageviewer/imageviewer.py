# SPDX-License-Identifier: MIT
# Copyright (c) 2025 Stefan Stechert

# Small image viewer demonstrating multiple controls and layout resizing.  
# Lets you choose a folder, lists images, and shows the selected one.  
# Zoom with a slider, switch images using buttons, ↑↓ keys, or clicks.  
# Window and layout automatically adjust when resized.

import socket
import time
import os

current_img = 0
current_image_pathes = []

# Geometry = [left, top, width, height]
control_geometry = {'Window': [], 'file_list': [], 'image_view': []}

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
def get_geometry(control_name):
  result = request(f'get`{control_name}`Geometry`')
  
  return int_list(result)

##########################################################################################
def set_geometry(control_name, geom):
  request(f'set`{control_name}`Geometry`{geom[0]},{geom[1]},{geom[2]},{geom[3]}`')

##########################################################################################
def update_control_geometries():
  for name in control_geometry:
    control_geometry[name] = get_geometry(name)

##########################################################################################
def resize_controls_to_fit_window():
  global control_geometry
  
  LEFT = 0
  TOP = 1
  WIDTH = 2
  HEIGHT = 3

  old_window = control_geometry['Window']
  new_window = get_geometry('Window')

  delta_width = new_window[WIDTH] - old_window[WIDTH]
  delta_height = new_window[HEIGHT] - old_window[HEIGHT]

  # Update image_view
  image_view = control_geometry['image_view']
  new_image = [
      image_view[LEFT],
      image_view[TOP],
      max(image_view[WIDTH] + delta_width, 10),
      max(image_view[HEIGHT] + delta_height, 10)
  ]
  set_geometry('image_view', new_image)

  # Update file_list
  file_list = control_geometry['file_list']
  new_file_list = [
      file_list[LEFT],
      file_list[TOP],
      file_list[WIDTH],
      max(file_list[HEIGHT] + delta_height, 10)
  ]
  set_geometry('file_list', new_file_list)
  
  set_zoom_slider_to_current_shrink()

##########################################################################################
def set_zoom_slider_to_current_shrink():
  scaling_str = request('get`image_view`Scaling`')
  
  current_zoom = float(scaling_str) * 100
  
  request('set`zoom_slider`Value`' + str(int(current_zoom)) + '`')

##########################################################################################
def set_image_zoom_by_slider(event):
  zoom_str = ev_val_str(event)
  
  scal = str(int(zoom_str) / 100)
  
  request('set`image_view`Scaling`' + scal + '`')

##########################################################################################
def show_image(idx_up_down):
  global current_img
  global current_image_pathes
  
  # Only do stuff, if these vars are not empty
  if current_image_pathes and idx_up_down:
    if idx_up_down == 'up':
      if current_img == 0:
        current_img = len(current_image_pathes) - 1
      else:
        current_img = current_img - 1
      
    elif idx_up_down == 'down':
      if current_img == (len(current_image_pathes) - 1):
        current_img = 0
      else:
        current_img = current_img + 1
      
    else:
      current_img = int(idx_up_down)
        
    request('set`image_view`Image`' + current_image_pathes[current_img] + '`\n' + \
            'set`file_list`Selected`' + str(current_img) + '`\n')
    
    set_zoom_slider_to_current_shrink()

##########################################################################################
def show_image_by_click_pos(event):
  TOP = 1
  HEIGHT = 3

  image_view_geometry = int_list(request('get`image_view`Geometry`'))
  
  vertical_center = image_view_geometry[HEIGHT] / 2
  
  # clicked into the upper half of the image_view?
  if ev_val_int_list(event)[TOP] < vertical_center:
    show_image('up')
  else:
    show_image('down')

##########################################################################################
def list_images_in_folder(event):
  global current_image_pathes
  current_image_pathes = []
  
  image_files = []

  image_dir = ev_val_str(event)
  
  files_n_folders = os.listdir(image_dir)
  
  for file_or_folder in files_n_folders:
    abs_path = os.path.join(image_dir, file_or_folder)
    
    if os.path.isfile(abs_path) and abs_path.lower().endswith(('.png', '.jpg', '.jpeg')):
      image_files.append(file_or_folder)
      current_image_pathes.append(abs_path)

  request('set`file_list`EntireTexts`' + '`'.join(image_files) + '`')
  
  show_image('0')

##########################################################################################

connect_to_gui()

update_control_geometries()

while True:
  events = get_events_list()
  
  for event in events:
    if event.startswith('open_dir`Chosen'):
      list_images_in_folder(event)
      
    elif event.startswith('zoom_slider`'):
      set_image_zoom_by_slider(event)
      
    elif event.startswith(('Window`KeyPressed`↑',
                           'Window`KeyPressed`Up',
                           'up_button`Clicked`')):
      show_image('up')
      
    elif event.startswith(('Window`KeyPressed`↓',
                           'Window`KeyPressed`Down',
                           'down_button`Clicked`')):
      show_image('down')
      
    elif event.startswith('image_view`ClickedAtPos'):
      show_image_by_click_pos(event)
      
    elif event.startswith('file_list`SelectionChanged'):
      show_image(ev_val_str(event))
      
    elif event.startswith('Window`Resized`'):
      resize_controls_to_fit_window()
        
    elif event.startswith('Window`Closed`'):
      request('set`Window`QuitNow`True`')
      sock.close()
      quit()
      
    else:
      print('Your event is not captured: ' + event)
    
    
  time.sleep(0.05)