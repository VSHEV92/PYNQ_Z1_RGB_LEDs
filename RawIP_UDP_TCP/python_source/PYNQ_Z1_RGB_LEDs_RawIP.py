import tkinter as tk
from RGB_LEDs import *
from Raw_IP_Connect import *

# создаем основное окно 
root = tk.Tk()
root.title('PYNQ Z1 RGB LEDs')

# создаем и настраиваем widget управления Ethernet соединением
Ethernet_Connect = RawIP_Connect(root)
Ethernet_Connect.get_frame().pack(side = 'bottom')

# создаем и настраиваем widget управления LEDs
Rgb_Leds_1 = Rgb_Leds(root)
Rgb_Leds_1.set_scale_func(Ethernet_Connect.send_data)
Rgb_Leds_1.get_frame().pack(side = 'bottom')

root.mainloop()
