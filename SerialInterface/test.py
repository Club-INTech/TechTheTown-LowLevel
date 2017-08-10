from tkinter import *
from tkinter.ttk import *
from serial import Serial
from serial.tools import list_ports

def foo(event):#function called when '<<ComboboxSelected>>' event is triggered
    print(v.get())#how to access to combobox selected item


root = Tk()
root.wm_geometry("400x300")
v = StringVar()#a string variable to hold user selection
options=list_ports.comports()
frame = Frame(root)
frame.pack()
combo = Combobox(root,textvariable=v, values=options)
combo.bind('<<ComboboxSelected>>',foo)#binding of user selection with a custom callback
combo.pack()
root.mainloop()