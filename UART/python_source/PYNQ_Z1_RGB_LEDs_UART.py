import tkinter as tk
from RGB_LEDs import *
from UART_Port import *

# создаем основное окно 
root = tk.Tk()
root.title('PYNQ Z1 RGB LEDs')

# создаем и настраиваем widget управления UART соединением
UART_Connect = UART_Port(root)
UART_Connect.get_frame().pack(side = 'right')

# создаем и настраиваем widget управления LEDs
Rgb_Leds_1 = Rgb_Leds(root)
Rgb_Leds_1.set_scale_func(UART_Connect.send_data)
Rgb_Leds_1.get_frame().pack(side = 'right')

root.mainloop()
