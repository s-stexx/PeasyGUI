# SPDX-License-Identifier: MIT
# Copyright (c) 2025 Stefan Stechert

# Simple “Hello” example.  
# Asks the user for their name, then greets them when pressing “Greet!”.  
# Also counts button clicks to show how fast event polling works.

import socket
import time

def request(command, sock): ##############################################################
  sock.sendall(command.encode('utf-8'))
  reply = sock.recv(100000).decode('utf-8')

  if reply.startswith('ERROR`'):
    raise ValueError(reply)
    
  if command.startswith('get`'):
    reply = reply.replace('OK`', '', 1)[:-1]

  return reply


def connect_to_gui(): ####################################################################
  connected_successful = False
  
  while not connected_successful:
    time.sleep(0.1)  
    
    try:
      sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
      sock.connect(('127.0.0.1', 10000))
      connected_successful = True
    except:
      print('Try to connect ...')
  
  return sock
  

##########################################################################################

sock = connect_to_gui()

clicks = 0

while True:
  events = request('events?', sock)

  if 'greet_button`Clicked' in events:
    clicks = clicks + events.count('greet_button`Clicked')
    
    user_name = request('get`user_input`Text`', sock)
    request('set`greet_text`Text`Hi, ' + user_name + '! 🤗 ' + str(clicks) + '`', sock)
  
  if 'Window`Closed`' in events:
    request('set`Window`QuitNow`True`', sock)
    sock.close()
    quit()
    
  time.sleep(0.1)
