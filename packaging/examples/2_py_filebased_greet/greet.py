# SPDX-License-Identifier: MIT
# Copyright (c) 2025 Stefan Stechert

# Same as the first example, but using the file-based protocol.
# Demonstrates identical code structure with much slower performance.
# Useful for restricted network environments.

import filebasedclient as gui
import time

gui.init('../../', ['--ini=examples/2_py_filebased_greet/PeasyGUI_greet.ini'])

clicks = 0

while True:
  events = gui.request('events?')

  if 'greet_button`Clicked' in events:
    clicks = clicks + events.count('greet_button`Clicked')
    
    user_name = gui.request('get`user_input`Text`')
    gui.request('set`greet_text`Text`Hi, ' + user_name + '! 🤗 ' + str(clicks) + '`')
  
  if 'Window`Closed`' in events:
    gui.request('set`Window`QuitNow`True`')
    gui.close()
    quit()
    
  time.sleep(0.1)