import tkinter as tk
from tkinter import ttk

class Rgb_Leds:

    def radiobutton_command(self):
        # обработчик события для Radiobutton
        if self.rb_var.get() == 0:
           self.sr_var.set(self.red_val_1)
           self.sg_var.set(self.green_val_1)
           self.sb_var.set(self.blue_val_1)
           self.lr['text'] = hex(int(self.sr_var.get()/self.max_scale*255))
           self.lg['text'] = hex(int(self.sg_var.get()/self.max_scale*255))
           self.lb['text'] = hex(int(self.sb_var.get()/self.max_scale*255))
        else:
           self.sr_var.set(self.red_val_2)
           self.sg_var.set(self.green_val_2)
           self.sb_var.set(self.blue_val_2)
           self.lr['text'] = hex(int(self.sr_var.get()/self.max_scale*255))
           self.lg['text'] = hex(int(self.sg_var.get()/self.max_scale*255))
           self.lb['text'] = hex(int(self.sb_var.get()/self.max_scale*255))
        
    def __init__(self, master):
        # создаем Frame
        self.f = ttk.LabelFrame(master, text = 'LEDs Colors', width = 20, height = 50)
        # содаем и инициализируем переменную для RadioButtons  
        self.rb_var = tk.IntVar()
        self.rb_var.set(0)
        # содаем и отображаем RadioButtons на Frame      
        self.rb_1 = tk.Radiobutton(self.f, text = "LED 1", variable = self.rb_var, value = 0, command = self.radiobutton_command)
        self.rb_2 = tk.Radiobutton(self.f, text = "LED 2", variable = self.rb_var, value = 1, command = self.radiobutton_command)
        self.rb_1.pack(pady = 10)
        self.rb_2.pack()
        # создаем Frame для Scales
        self.scale_f = ttk.Frame(self.f, width = 40, height = 200)
        self.scale_f.pack(padx = 20, pady = 10)
        # содаем и инициализируем переменные для Scales  
        self.sr_var = tk.IntVar()
        self.sr_var.set(0)
        self.sg_var = tk.IntVar()
        self.sg_var.set(0)
        self.sb_var = tk.IntVar()
        self.sb_var.set(0)

        self.red_val_1 = 0
        self.red_val_2 = 0
        self.green_val_1 = 0
        self.green_val_2 = 0
        self.blue_val_1 = 0
        self.blue_val_2 = 0 

        # создаем Scales для красного, зеленого и синого светодиодов 
        self.max_scale = 120
        self.sr = ttk.Scale(self.scale_f, from_ = 0, to = self.max_scale, orient = 'vertical', variable = self.sr_var)
        self.sg = ttk.Scale(self.scale_f, from_ = 0, to = self.max_scale, orient = 'vertical', variable = self.sg_var)
        self.sb = ttk.Scale(self.scale_f, from_ = 0, to = self.max_scale, orient = 'vertical', variable = self.sb_var)
        self.lr = ttk.Label(self.scale_f, text = '0x0', width = 4)
        self.lg = ttk.Label(self.scale_f, text = '0x0', width = 4)
        self.lb = ttk.Label(self.scale_f, text = '0x0', width = 4)
        self.lr.grid(padx = 10, row = 0, column = 0)
        self.lg.grid(padx = 10, row = 0, column = 1)
        self.lb.grid(padx = 10, row = 0, column = 2)
        self.sr.grid(padx = 10, row = 1, column = 0)
        self.sg.grid(padx = 10, row = 1, column = 1)
        self.sb.grid(padx = 10, row = 1, column = 2)
        ttk.Label(self.scale_f, text = 'R').grid(padx = 10, row = 2, column = 0)
        ttk.Label(self.scale_f, text = 'G').grid(padx = 10, row = 2, column = 1)
        ttk.Label(self.scale_f, text = 'B').grid(padx = 10, row = 2, column = 2)

        # создаем обработчик движения ползунком
        self.command = None
        
    def scale_command(self, *args):
         # обработчик команд Scale 
         self.lr['text'] = hex(int(self.sr_var.get()/self.max_scale*255))
         self.lg['text'] = hex(int(self.sg_var.get()/self.max_scale*255))
         self.lb['text'] = hex(int(self.sb_var.get()/self.max_scale*255))
         if self.rb_var.get() == 0:
             self.red_val_1 = self.sr_var.get()
             self.green_val_1 = self.sg_var.get()
             self.blue_val_1 = self.sb_var.get()  
         else:
             self.red_val_2 = self.sr_var.get()
             self.green_val_2 = self.sg_var.get()
             self.blue_val_2 = self.sb_var.get()  
         self.command(self)
           
    def set_scale_func(self, func):
        # устанавливем обработчик событий для Scale
        self.sr['command'] = self.scale_command
        self.sg['command'] = self.scale_command
        self.sb['command'] = self.scale_command
        self.command = func
        
    def get_frame(self):
        # метод возвращает Frame
        return self.f
    
