import tkinter as tk
from tkinter import ttk
import socket

class TCP_UDP_Connect:
    def Start_Button_Pressed(self):
        # обработчик нажатия Start_Button
        if(self.rb_var.get() == 0):
            self.sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        else:
            self.sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
            
        self.sock.connect((self.IP_Addr_var.get(), self.Port_Num_var.get()))
        self.rb_1['state'] = 'disabled'
        self.rb_2['state'] = 'disabled'
        self.Start_Button['state'] = 'disabled'
        self.Stop_Button['state'] = '!disabled'
            
    def Stop_Button_Pressed(self):
        # обработчик нажатия Stop_Button
        self.sock.close()
        self.rb_1['state'] = 'normal'
        self.rb_2['state'] = 'normal'
        self.Start_Button['state'] = '!disabled'
        self.Stop_Button['state'] = 'disabled'
        
    def __init__(self, master):
        # создаем Frame
        self.f = ttk.LabelFrame(master, text = 'RawIP Connection', width = 40, height = 50)

        # содаем и инициализируем переменную для RadioButtons  
        self.rb_var = tk.IntVar()
        self.rb_var.set(0)
        # содаем и отображаем RadioButtons на Frame      
        self.rb_1 = tk.Radiobutton(self.f, text = "TCP", variable = self.rb_var, value = 0)
        self.rb_2 = tk.Radiobutton(self.f, text = "UDP", variable = self.rb_var, value = 1)
        self.rb_1.grid(row = 0, column = 0)
        self.rb_2.grid(row = 0, column = 1)
        
        # создаем Entry для настройки TCP/UDP соединения
        self.IP_Addr_var = tk.StringVar()
        self.IP_Addr_var.set('192.168.1.10')
        self.IP_Addr_Entry = ttk.Entry(self.f, width = 15, textvariable = self.IP_Addr_var, justify = 'center')
        ttk.Label(self.f, text = 'IP Address').grid(padx = 5, row = 1, column = 0, columnspan = 2)
        self.IP_Addr_Entry.grid(pady = 4, padx = 5, row = 2, column = 0, columnspan = 2)

        self.Port_Num_var = tk.IntVar()
        self.Port_Num_var.set(5001)
        self.Port_Num_Entry = ttk.Entry(self.f, width = 15, textvariable = self.Port_Num_var, justify = 'center')
        ttk.Label(self.f, text = 'Port Number').grid(padx = 5, row = 3, column = 0, columnspan = 2)
        self.Port_Num_Entry.grid(pady = 4, padx = 5, row = 4, column = 0, columnspan = 2)

        # создаем Buttons для начала и окончания соединения
        self.Start_Button = ttk.Button(self.f, text = '     Start\nConnection', state = '!disabled', command = self.Start_Button_Pressed)
        self.Start_Button.grid(pady = 12, padx = 8, row = 5, column = 0)
        
        self.Stop_Button = ttk.Button(self.f, text = '     Stop\nConnection', state = 'disabled', command = self.Stop_Button_Pressed)
        self.Stop_Button.grid(pady = 12, padx = 8, row = 5, column = 1)

    def send_data(self, led_data):
        # метод отправки данных
        if led_data.rb_var.get() == 0:
            self.sock.send(bytes([255, led_data.red_val_1, led_data.green_val_1, led_data.blue_val_1]))
        else:  
            self.sock.send(bytes([254, led_data.red_val_2, led_data.green_val_2, led_data.blue_val_2]))
 
    def get_frame(self):
        # метод возвращает Frame
        return self.f
       
