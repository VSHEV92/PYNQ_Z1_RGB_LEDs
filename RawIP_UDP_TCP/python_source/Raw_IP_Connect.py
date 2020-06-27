import tkinter as tk
from tkinter import ttk
import socket

class RawIP_Connect:
    def Start_Button_Pressed(self):
        # обработчик нажатия Start_Button
        interface = 'enp6s0f1'
        self.dist_addr = self.Dist_Mac_Addr_var.get()
        self.dist_addr = [int(char, 16) for char in self.dist_addr.split(':')]
        self.dist_addr = bytes(self.dist_addr)
        self.src_addr = self.Source_Mac_Addr_var.get()
        self.src_addr = [int(char, 16) for char in self.src_addr.split(':')]
        self.src_addr = bytes(self.src_addr)        
        self.frame_size = b'\x00\x65'

        self.RawIPSocket = socket.socket(socket.AF_PACKET, socket.SOCK_RAW, socket.htons(3))
        self.RawIPSocket.bind((interface, 0))        
        self.Start_Button['state'] = 'disabled'
        self.Stop_Button['state'] = '!disabled'
            
    def Stop_Button_Pressed(self):
        # обработчик нажатия Stop_Button
        self.RawIPSocket.close()
        self.Start_Button['state'] = '!disabled'
        self.Stop_Button['state'] = 'disabled'
        
    def __init__(self, master):
        # создаем Frame
        self.f = ttk.LabelFrame(master, text = 'RawIP Connection', width = 40, height = 50)

        # создаем Entry для настройки Ethernet соединения
        self.Source_Mac_Addr_var = tk.StringVar()
        self.Source_Mac_Addr_var.set('98:28:a6:23:3c:70')
        self.Source_Mac_Addr_Entry = ttk.Entry(self.f, width = 20, textvariable = self.Source_Mac_Addr_var, justify = 'center')
        ttk.Label(self.f, text = 'Source MAC Address').grid(padx = 5, row = 0, column = 0, columnspan = 2)
        self.Source_Mac_Addr_Entry.grid(pady = 4, padx = 5, row = 1, column = 0, columnspan = 2)

        self.Dist_Mac_Addr_var = tk.StringVar()
        self.Dist_Mac_Addr_var.set('00:0a:35:01:02:03')
        self.Dist_Mac_Addr_Entry = ttk.Entry(self.f, width = 20, textvariable = self.Dist_Mac_Addr_var, justify = 'center')
        ttk.Label(self.f, text = 'Distination MAC Address').grid(padx = 5, row = 2, column = 0, columnspan = 2)
        self.Dist_Mac_Addr_Entry.grid(pady = 4, padx = 5, row = 3, column = 0, columnspan = 2)


        # создаем Buttons для начала и окончания соединения UART
        self.Start_Button = ttk.Button(self.f, text = '     Start\nConnection', state = '!disabled', command = self.Start_Button_Pressed)
        self.Start_Button.grid(pady = 12, padx = 8, row = 4, column = 0)
        
        self.Stop_Button = ttk.Button(self.f, text = '     Stop\nConnection', state = 'disabled', command = self.Stop_Button_Pressed)
        self.Stop_Button.grid(pady = 12, padx = 8, row = 4, column = 1)

    def send_data(self, led_data):
        # метод отправки данные по последовательному порту
        if led_data.rb_var.get() == 0:
            payload = bytes([255, led_data.red_val_1, led_data.green_val_1, led_data.blue_val_1])
            self.RawIPSocket.sendall(self.dist_addr + self.src_addr + self.frame_size + payload + bytes(97))
        else:  
            payload = bytes([254, led_data.red_val_2, led_data.green_val_2, led_data.blue_val_2])
            self.RawIPSocket.sendall(self.dist_addr + self.src_addr + self.frame_size + payload + bytes(97))
 
    def get_frame(self):
        # метод возвращает Frame
        return self.f
       
