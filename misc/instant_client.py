# SPDX-License-Identifier: MIT
# Copyright (c) 2025 Stefan Stechert

import tkinter as tk
from tkinter import messagebox
import os
import socket

FILENAME = "history_of_instant_client.txt"


sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(("127.0.0.1", 10000))

class CommandApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Instant Commands")

        # Listbox
        self.listbox = tk.Listbox(root, width=50, height=40)
        self.listbox.pack(padx=10, pady=5)
        self.listbox.bind("<Button-1>", self.on_click)

        # Textbox + Button Frame
        frame = tk.Frame(root)
        frame.pack(pady=5)

        self.entry = tk.Entry(frame, width=40)
        self.entry.pack(side=tk.LEFT, padx=(0, 5))

        self.add_button = tk.Button(frame, text="Send", command=self.send_command)
        self.add_button.pack(side=tk.LEFT)

        self.load_commands()

        self.root.protocol("WM_DELETE_WINDOW", self.on_close)

    def on_click(self, event):
        self.listbox.after(100, self.handle_selection)
        
    def handle_selection(self):    
        selection = self.listbox.curselection()
        if selection:
            index = selection[0]
            command = self.listbox.get(index)
            
            self.entry.delete(0, tk.END)
            self.entry.insert(0, command)

    def send_command(self):
        command = self.entry.get().strip()
        if command:
            sock.sendall(command.replace(r'\n', '\n').encode('utf-8'))
            print(sock.recv(100000).decode('utf-8'))
            command_history = self.make_list_from_list()
            if command not in command_history:
              self.listbox.insert(tk.END, command)
              self.entry.delete(0, tk.END)

    def load_commands(self):
        if os.path.exists(FILENAME):
            with open(FILENAME, "r", encoding="utf-8") as f:
                for line in f:
                    self.listbox.insert(tk.END, line.strip())

    def make_list_from_list(self):
        command_history = []
        for i in range(self.listbox.size()):
            command_history.append(self.listbox.get(i))
        return command_history
    
    def on_close(self):
        sock.close()
        try:
            with open(FILENAME, "w", encoding="utf-8") as f:
                for i in range(self.listbox.size()):
                    f.write(self.listbox.get(i) + "\n")
        except Exception as e:
            messagebox.showerror("Fehler beim Speichern", str(e))
        self.root.destroy()

if __name__ == "__main__":
    root = tk.Tk()
    fenster_breite = 500
    fenster_hoehe = 750
    bildschirm_breite = root.winfo_screenwidth()
    bildschirm_hoehe = root.winfo_screenheight()
    x_position = bildschirm_breite - fenster_breite - 10
    y_position = 300
    root.geometry(f"{fenster_breite}x{fenster_hoehe}+{x_position}+{y_position}")
    app = CommandApp(root)
    root.mainloop()
