# SPDX-License-Identifier: MIT
# Copyright (c) 2025 Stefan Stechert

"""
filebasedclient.py – Helper module for communicating with PeasyGUI via files.

This module lets your program talk to a PeasyGUI executable *without* using TCP sockets.
This is much slower, but works in restricted network environments.
It creates a temporary exchange directory (usually in the OS temp folder, often in RAM)
and communicates by writing and or reading small request/reply/event files.

Main functions:
  • init(dir_of_gui_exe, additional_arguments=None)
      Creates a temporary exchange directory,
      launches the PeasyGUI executable with access to that directory.

  • request(command)
      Sends a request and returns the GUI’s reply.
      - For 'events?': reads and merges all event files, then deletes them.
      - For other commands: writes request.txt, waits for reply.txt, then deletes it.
      Typical commands: get`, set`, new`, events?

  • close(timeout_seconds=3.0)
      Terminates the PeasyGUI process and removes the temporary exchange directory.

Notes:
  - All file operations are short-lived and cleaned up automatically.
  - No permanent files or registry entries are created.
  - Works on all major OSes (Windows, macOS, Linux).
"""

import os
import sys
import stat
import shutil
import subprocess
import tempfile
import time
import glob

exchange_dir = None
gui_process = None

def _select_exchange_dir():  #############################################################
    if sys.platform.startswith('linux') and os.path.isdir('/dev/shm'):
        return tempfile.mkdtemp(dir='/dev/shm')

    if sys.platform == 'darwin':
        # macOS exposes a per‑user volatile temp area via TMPDIR
        base = os.environ.get('TMPDIR', '/tmp')
        return tempfile.mkdtemp(dir=base)

    # Windows: default temp location
    return tempfile.mkdtemp()


def _read_combine_and_delete_all_event_files():  #########################################
    global exchange_dir
    
    event_pattern = os.path.join(exchange_dir, '*_events.txt')
    event_files = sorted(glob.glob(event_pattern))
    
    if not event_files:
        return 'NONE'

    combined = []
    for event_file in event_files:
        payload = _read_file(event_file)
        combined.append(payload)
        os.remove(event_file)
        
    # All deleted should be gone
    for event_file in event_files:
        while os.path.exists(event_file):
            time.sleep(0.02)

    return ''.join(combined)


def _remove_file_and_wait_till_gone(path):  ##############################################
    os.remove(path)
    
    while os.path.exists(path):
        time.sleep(0.02)


def _write_request_file(payload_utf8):
    global exchange_dir

    draft_path = os.path.join(exchange_dir, 'request_draft.txt')
    request_path = os.path.join(exchange_dir, 'request.txt')
    
    with open(draft_path, 'w', encoding='utf-8') as f:
        f.write(payload_utf8)
        f.flush()
        os.fsync(f.fileno())
        
    os.rename(draft_path, request_path)


def _read_and_delete_reply_file():  ######################################################
    global exchange_dir
    
    reply_path = os.path.join(exchange_dir, 'reply.txt')
    
    while not os.path.exists(reply_path):
        time.sleep(0.02)
    
    reply_payload = _read_file(reply_path)
    
    _remove_file_and_wait_till_gone(reply_path)
    
    return reply_payload


def _read_file(path): ####################################################################
    payload = None

    while not payload:
        try:
            with open(path, 'r', encoding='utf-8') as f:
                payload = f.read()
        except:
            time.sleep(0.02)
            
    return payload


def _dir_contains_anything(path): ########################################################
    # Anything, that is not hidden
    for entry in os.scandir(path):
        if entry.name.startswith('.'):
            continue
        if os.name == 'nt' and (entry.stat().st_file_attributes & stat.FILE_ATTRIBUTE_HIDDEN):
            continue
        
        return True
        
    return False  


def init(dir_of_gui_exe, additional_arguments=None): #####################################
    """
    Start the GUI.
    additional_arguments – list of extra CLI arguments for the GUI.
    """
    global exchange_dir
    global gui_process

    exchange_dir = _select_exchange_dir()
    
    if _dir_contains_anything(exchange_dir):
        raise RuntimeError('"' + exchange_dir + '" is not empty')
        
    plat = sys.platform

    if plat.startswith('win'):
        exe_name = r'bin\PeasyGUI.exe'
    elif plat == 'darwin':
        exe_name = 'PeasyGUI.app/Contents/MacOS/PeasyGUI'
    elif plat.startswith('linux'):
        exe_name = 'start_PeasyGUI.sh'
    else:
        raise RuntimeError('Unsupported platform: ' + plat)

    exe_path = os.path.normpath(os.path.join(dir_of_gui_exe, exe_name))

    args = [exe_path, '--exchange_dir=' + exchange_dir, '--protocol=filebased']
    
    if additional_arguments:
        args.extend(additional_arguments)

    gui_process = subprocess.Popen(args)


def close(timeout_seconds=3.0): ##########################################################
    """Terminate GUI (if still alive) and delete the exchange directory."""
    global exchange_dir
    global gui_process
    if gui_process is None:
        return

    gui_process.terminate()
    try:
        gui_process.wait(timeout_seconds)
    except subprocess.TimeoutExpired:
        gui_process.kill()

    shutil.rmtree(exchange_dir, ignore_errors=True)
    exchange_dir = None
    gui_process = None


def request(command): ####################################################################
    """
    Send a request (string, already UTF-8) and return the reply.
    Special case: if payload is exactly 'events?' the function
    concatenates every *_events.txt file and returns the combined text
    or the string 'NONE' if no new events are present.
    """
    global exchange_dir
    
    if exchange_dir is None:
        raise RuntimeError('call init() first')

    if command == 'events?':
        reply = _read_combine_and_delete_all_event_files()

    else:
        _write_request_file(command)

        reply = _read_and_delete_reply_file()
        
        if reply.startswith('ERROR`'):
            raise ValueError(reply)
    
        if command.startswith('get`'):
            reply = reply.replace('OK`', '', 1)[:-1]

    return reply

