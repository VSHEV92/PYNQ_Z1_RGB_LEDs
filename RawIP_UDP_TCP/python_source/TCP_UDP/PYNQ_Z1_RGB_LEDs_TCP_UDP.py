import tkinter as tk
from RGB_LEDs import *
from TCP_UDP_Connect import *

# создаем основное окно 
root = tk.Tk()
root.title('PYNQ Z1 RGB LEDs')

# создаем и настраиваем widget управления Ethernet соединением
Net_Connect = TCP_UDP_Connect(root)
Net_Connect.get_frame().pack(side = 'right')

# создаем и настраиваем widget управления LEDs
Rgb_Leds_1 = Rgb_Leds(root)
Rgb_Leds_1.set_scale_func(Net_Connect.send_data)
Rgb_Leds_1.get_frame().pack(side = 'right')

root.mainloop()
