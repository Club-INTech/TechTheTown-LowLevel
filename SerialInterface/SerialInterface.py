from tkinter import *
from tkinter.ttk import *
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.animation import FuncAnimation
from time import time
from serial import Serial
import sys
import serial.tools.list_ports as ports
from collections import deque
import matplotlib.pyplot as plt
from time import time, sleep, clock

freqSerie = 1
freqPlot = 50
serialPort = None

try:
    serialPort = Serial(ports.comports()[0].device, 115200)
except Exception as e:
    print(e)

sleep(1.5)
plt.style.use('dark_background')
plotting = False

root = Tk()
root.wm_title("Interface LL")

root.grid_rowconfigure(0, weight=1)
root.grid_columnconfigure(0, weight=1)

content = Frame(root)
content.grid_columnconfigure(0, weight=1)
content.grid_columnconfigure(1, weight=3)
content.grid_rowconfigure(0, weight=100)
content.grid_rowconfigure(1, weight=1)
content.grid(row=0, column=0, sticky='news')


class GraphFrame(LabelFrame):
    def __init__(self, parent, *args, **kwargs):
        LabelFrame.__init__(self, parent, text="Graph", name="graph")
        self.container = Frame(self)
        self.fig = Figure((7, 5), dpi=100)
        self.ax1 = self.fig.add_subplot(211)
        self.ax2 = self.fig.add_subplot(212)
        self.nbpts = 100
        self.temp = deque([0], self.nbpts)
        self.humid = deque([0], self.nbpts)
        self.x = deque([0], self.nbpts)
        self.canvas = FigureCanvasTkAgg(self.fig, self.container)
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=1)
        self.grid_columnconfigure(0, weight=1)
        self.canvas.get_tk_widget().pack(side='top')
        self.container.grid(row=1, column=0, sticky='news')
        self.toolbar = NavigationToolbar2TkAgg(self.canvas, self.container)
        self.toolbar.pack(side='bottom')
        self.lastUpdate = 0
        self.origin = clock()

    def resetValues(self, *args):
        self.temp.clear()
        self.temp.append(0)
        self.humid.clear()
        self.humid.append(0)
        self.x.clear()
        self.x.append(0)
        self.origin = clock()

    def replot(self, data, *args):
        if data != 'ERROR' and data != 'NOREF':
            print("Replotting")
            if plotting:
                self.temp.append(data[0])
                self.humid.append(data[1])
                self.x.append(self.x[-1] + freqPlot / 1000)
                dt = clock() - self.lastUpdate
                if dt * 1000 > freqPlot:
                    print("insite freqplot")
                    self.ax1.clear()
                    self.ax2.clear()
                    self.ax1.plot(self.x, self.temp, label="temp")
                    self.ax2.plot(self.x, self.humid, label="humid")
                    self.lastUpdate = clock()


class SerialCom(LabelFrame):
    def __init__(self, parent):
        LabelFrame.__init__(self, parent, text="Serial port", name="com")
        self.grid_rowconfigure(0, weight=1)
        self.grid_columnconfigure(0, weight=1)
        self.grid_propagate(True)
        self.message = StringVar(self, name="message")
        self.comLog = Text(self, bg="black", fg="lightgreen", width=30, state=DISABLED)
        self.comLog.grid(row=0, column=0, sticky='news')
        self.textField = Entry(self, textvariable=self.message, name="entry")
        self.textField.grid(row=1, column=0, sticky='news')
        self.textField.bind("<Return>", self.sendData)
        self.lastData = [[], []]
        self.updated = False

    def sendData(self, *args):
        if serialPort is not None:

            toSend = self.textField.get()
            self.textField.delete(0, 'end')
            serialPort.write((toSend + '\r\n').encode())
            self.insertData("Sent: " + toSend)
        else:
            self.insertData("ERROR: No connexion !")

    def insertData(self, data):
        self.comLog.configure(state=NORMAL)
        self.comLog.insert(END, data)
        self.comLog.insert(END, '\n')
        self.comLog.see(END)
        self.comLog.configure(state=DISABLED)

    def logError(self, error):
        self.insertData("ERROR: " + error)

    def getData(self, *args):
        if serialPort.in_waiting > 0:
            readLine = serialPort.readline().decode('ascii').strip('\r\n')
            self.insertData("Received: " + readLine)
            readArray = readLine.split('\t')
            if readArray[0] == 'PLT':
                return readArray[1:]
            else:
                return 'NOREF'
        else:
            serialPort.reset_input_buffer()
            return 'ERROR'


class ButtonFrame(LabelFrame):
    def __init__(self, parent):
        LabelFrame.__init__(self, parent, text="Commandes")
        self.parent = parent
        self.startButton = Button(self, text="Start", command=self.startPlot)
        self.startButton.grid(row=0, column=0, sticky='ew')
        self.grid_propagate(True)

    def startPlot(self):
        if serialPort is not None:
            global plotting
            if not plotting:
                self.master.nametowidget("graph").ax1.clear()
                self.master.nametowidget("graph").ax2.clear()
            self.master.nametowidget("com").message.set("SP\r\nRT")
            self.master.nametowidget("com").sendData()
            serialPort.reset_input_buffer()
            serialPort.reset_output_buffer()
            self.master.nametowidget("graph").resetValues()

            plotting = not plotting
        else:
            self.master.nametowidget("com").logError("No connexion !")


serialCom = SerialCom(content)
serialCom.grid(row=0, column=0, sticky='news')
graph = GraphFrame(content)
graph.grid(row=0, column=1, sticky='news')
buttons = ButtonFrame(content)
buttons.grid(row=1, column=0, columnspan=2, sticky='ew')


def updatevalues(*args):
    data = serialCom.getData()
    return data


def replotgraph(*args):
    if serialPort is not None:
        graph.replot(updatevalues())


root.bind("<Escape>", exit)
graphAnim = FuncAnimation(graph.fig, replotgraph, interval=freqSerie, blit=False)
root.mainloop()
