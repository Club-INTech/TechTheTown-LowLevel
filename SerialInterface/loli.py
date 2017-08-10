import tkinter as tk
import tkinter.scrolledtext as scroll
from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import copy
import time
import numpy as np
import sys
import os

from AsciiSerial import AsciiSerial

BG_LOOP_PERIOD = 10  # ms
GRAPH_REFRESH_PERIOD = 0.1  # secondes
DATA_RECEIVE_PERIOD = 1  # ms


root = tk.Tk()
root.title("INTech Senpaï - Moon Rover LowLevel GUI")


class LowLevelGUI(tk.Frame):
    """
        Interface graphique de l'application de déboguage du bas niveau
    """
    def __init__(self, master=None):
        super().__init__(master)

        self._master = master

        self.grid(column=0, row=0, sticky='NSEW')
        self.pack(fill='both', expand=True)

        self.asciiSerial = AsciiSerial()

        self.toolbar = ToolBar(self.asciiSerial, self)
        self.toolbar.grid(column=0, row=0, sticky='NSEW')

        self.displayArea = DisplayArea(self)
        self.displayArea.grid(column=0, row=1, sticky='NSEW')

        self.columnconfigure(0, weight=1)
        self.rowconfigure(1, weight=1)

        master.bind("<Control-space>", self.ctrl_space)
        master.bind("<Control-plus>", self.ctrl_plus)
        master.bind("<Control-minus>", self.ctrl_minus)
        master.bind("<Control-Delete>", self.ctrl_delete)
        master.bind("<Up>", self.up_key)
        master.bind("<Down>", self.down_key)
        master.bind("<Return>", self.get_focus)

        self._scheduledRoutine = None

        self.lastRePlotTime = time.time()

        self.backgroundLoop()

    def backgroundLoop(self):
        global BG_LOOP_PERIOD, GRAPH_REFRESH_PERIOD

        try:
            self.asciiSerial.communicate()
        except IOError:
            self.toolbar.serialSetup.openClosePort(closeWithRed=True)

        state = self.toolbar.getState()
        commandLines = self.asciiSerial.setEnabledChannels(state)
        self.displayArea.mainConsole.addLines(commandLines, useAltColor=True)

        mainLines = self.asciiSerial.getLines_main()
        self.displayArea.mainConsole.addLines(mainLines)
        if len(mainLines) > 0 and not state['console']:
            self.toolbar.highlightB_console()

        warningLines = self.asciiSerial.getLines_warning()
        self.displayArea.warningConsole.addLines(warningLines)
        if len(warningLines) > 0 and not state['warning']:
            self.toolbar.highlightB_warning()

        errorLines = self.asciiSerial.getLines_error()
        self.displayArea.errorConsole.addLines(errorLines)
        if len(errorLines) > 0 and not state['error']:
            self.toolbar.highlightB_error()

        self.asciiSerial.addLinesToSend(self.displayArea.mainConsole.getUserInputLines())
        self.displayArea.mainConsole.computeUserInput()

        self.displayArea.updateDisplay(state)
        if time.time() - self.lastRePlotTime > GRAPH_REFRESH_PERIOD:
            self.displayArea.rePlotData(self.asciiSerial.getAllData())
            self.lastRePlotTime = time.time()
        self._scheduledRoutine = self.after(BG_LOOP_PERIOD, self.backgroundLoop)

    def on_exit(self):
        self.after_cancel(self._scheduledRoutine)
        self._master.destroy()
        sys.exit(0)

    def ctrl_space(self, tk_event):
        self.toolbar.allOnOff()

    def ctrl_plus(self, tk_event):
        self.toolbar.zoomButton.unZoom()

    def ctrl_minus(self, tk_event):
        self.toolbar.zoomButton.zoom()

    def ctrl_delete(self, tk_event):
        self.displayArea.mainConsole.clear()
        self.asciiSerial.clearLinesToSend()

    def up_key(self, tk_event):
        self.displayArea.mainConsole.goBackInTime()
        self.get_focus(tk_event)

    def down_key(self, tk_event):
        self.displayArea.mainConsole.backToTheFuture()
        self.get_focus(tk_event)

    def get_focus(self, tk_event):
        self.displayArea.mainConsole.get_focus()


class ToolBar(tk.Frame):
    """
        Barre d'outils permettant de contrôler l'affichage des données et la connexion au robot
    """
    def __init__(self, asciiSerial, master=None):
        super().__init__(master)

        channelsList = asciiSerial.getChannelsList()

        self.serialSetup = SerialSetup(asciiSerial, self)
        self.serialSetup.grid(column=0, row=0, sticky='NSEW')

        self.b_console = tk.Button(self, command=self.debugConsole)
        self.b_console["text"] = "Debug & Display\nConsole"
        self.b_console.grid(column=1, row=0, sticky='NSEW')

        self.b_warning = tk.Button(self, command=self.warningConsole)
        self.b_warning["text"] = "Warnings\nConsole"
        self.b_warning.grid(column=2, row=0, sticky='NSEW')

        self.b_error = tk.Button(self, command=self.errorConsole)
        self.b_error["text"] = "Errors\nConsole"
        self.b_error.grid(column=3, row=0, sticky='NSEW')

        self.b_allOnOff = tk.Button(self, command=self.allOnOff)
        self.b_allOnOff["text"] = "Global\nOn/Off"
        self.b_allOnOff.grid(column=4, row=0, sticky='NSEW')

        self.zoomButton = ZoomButton(self)
        self.zoomButton.grid(column=5, row=0, sticky='NSEW')

        self.controlGraph1 = ButtonGroup(self, "Graph 1", channelsList)
        self.controlGraph1.grid(column=6, row=0, sticky='NSEW')

        self.controlGraph2 = ButtonGroup(self, "Graph 2", channelsList)
        self.controlGraph2.grid(column=7, row=0, sticky='NSEW')
        self.controlGraph2.show_hide()

        self.controlGraph3 = ButtonGroup(self, "Graph 3", channelsList)
        self.controlGraph3.grid(column=8, row=0, sticky='NSEW')
        self.controlGraph3.show_hide()

        self.controlGraph4 = ButtonGroup(self, "Graph 4", channelsList)
        self.controlGraph4.grid(column=9, row=0, sticky='NSEW')
        self.controlGraph4.show_hide()

        self._state = {
            'console': True,    # Bool
            'warning': False,   # Bool
            'error'  : False,   # Bool
            'graphScale': self.zoomButton.getScale(),   # Integer
            'graph1' : self.controlGraph1.getState(),   # { 'show': Bool, 'enable': Bool, 'channel': String }
            'graph2' : self.controlGraph2.getState(),   # { 'show': Bool, 'enable': Bool, 'channel': String }
            'graph3' : self.controlGraph3.getState(),   # { 'show': Bool, 'enable': Bool, 'channel': String }
            'graph4' : self.controlGraph4.getState()    # { 'show': Bool, 'enable': Bool, 'channel': String }
        }

    def debugConsole(self):
        if not self._state['console']:
            self.highlightB_console(undo=True)
            self._state['console'] = True
        else:
            self._state['console'] = False

    def warningConsole(self):
        if not self._state['warning']:
            self.highlightB_warning(undo=True)
            self._state['warning'] = True
        else:
            self._state['warning'] = False

    def errorConsole(self):
        if not self._state['error']:
            self.highlightB_error(undo=True)
            self._state['error'] = True
        else:
            self._state['error'] = False

    def allOnOff(self):
        self._state = self.getState()
        oneWasTurnedOn = False
        for i in [1,2,3,4]:
            visible = self._state['graph' + str(i)]['show']
            enabled = self._state['graph' + str(i)]['enable']
            if visible and not enabled:
                exec("self.controlGraph" + str(i) + ".toggleEnable()")
                self._state = self.getState()
                if self._state['graph' + str(i)]['enable']:
                    oneWasTurnedOn = True
        if not oneWasTurnedOn:
            for i in [1,2,3,4]:
                if self._state['graph' + str(i)]['enable']:
                    exec("self.controlGraph" + str(i) + ".toggleEnable()")
        self._state = self.getState()

    def getState(self):
        self._state['graphScale'] = self.zoomButton.getScale()
        self._state['graph1'] = self.controlGraph1.getState()
        self._state['graph2'] = self.controlGraph2.getState()
        self._state['graph3'] = self.controlGraph3.getState()
        self._state['graph4'] = self.controlGraph4.getState()

        graphs = [('graph1', self.controlGraph1),
                  ('graph2', self.controlGraph2),
                  ('graph3', self.controlGraph3),
                  ('graph4', self.controlGraph4)]
        for g1, fg1 in graphs:
            for g2, fg2 in graphs:
                if g1 != g2 and self._state[g1]['channel'] == self._state[g2]['channel']:
                    if self._state[g1]['enable'] and self._state[g2]['enable']:
                        fg2.toggleEnable()
                        return self.getState()
        return self._state

    def highlightB_console(self, undo=False):
        if undo:
            self.b_console['bg'] = 'gray95'
            self.b_console['fg'] = 'black'
        else:
            self.b_console['bg'] = 'gray75'
            self.b_console['fg'] = 'black'

    def highlightB_warning(self, undo=False):
        if undo:
            self.b_warning['bg'] = 'gray95'
            self.b_warning['fg'] = 'black'
        else:
            self.b_warning['bg'] = 'orange'
            self.b_warning['fg'] = 'black'

    def highlightB_error(self, undo=False):
        if undo:
            self.b_error['bg'] = 'gray95'
            self.b_error['fg'] = 'black'
        else:
            self.b_error['bg'] = 'red'
            self.b_error['fg'] = 'black'


class ButtonGroup(tk.Frame):
    """
        Groupe de boutons associé à un graphique pyplot
        Afficher/Cacher
        Activer/Désactiver l'acquisition des données
        Choix du canal de données à afficher
    """
    def __init__(self, master=None, name='', optionsList=None):
        super().__init__(master)

        self._state = {
            'show'   : True,
            'enable' : False,
            'channel': None
        }

        self.label = tk.Label(self, text=name)
        self.label.grid(column=0, row=0, columnspan=2, sticky='NSEW')

        self.combobox = ttk.Combobox(self)
        self.combobox.bind('<<ComboboxSelected>>', self.updateChannel)
        self.combobox['state'] = 'readonly'
        self.combobox['values'] = optionsList
        self.combobox.grid(column=0, row=1, columnspan=2, sticky='NSEW')

        self.b_show_hide = tk.Button(self, text="show", command=self.show_hide)
        self.b_show_hide.grid(column=0, row=2, sticky='NSEW')

        self.b_enable = tk.Button(self, text="on/off", command=self.toggleEnable)
        self.b_enable.grid(column=1, row=2, sticky='NSEW')

        self.updateColors()

    def show_hide(self):
        self._state['show'] = not self._state['show']
        self.updateColors()

    def toggleEnable(self):
        if self._state['channel'] is not None:
            self._state['enable'] = not self._state['enable']
            self.updateColors()

    def updateChannel(self, tk_event):
        self._state['channel'] = self.combobox.get()

    def getState(self):
        return self._state

    def updateColors(self):
        if self._state['show']:
            self.b_show_hide['bg'] = 'pale green'
        else:
            self.b_show_hide['bg'] = 'gray95'
        if self._state['enable']:
            self.b_enable['bg'] = 'chartreuse'
        else:
            self.b_enable['bg'] = 'red'


class SerialSetup(tk.Frame):
    """
        Gestion de la connexion à un port série
        Choix du port, ouverture/fermeture
    """
    def __init__(self, asciiSerial, master=None):
        super().__init__(master)

        self.asciiSerial = asciiSerial
        self.portOpen = False

        self.label = tk.Label(self, text="Serial port")
        self.label.grid(column=0, row=0, sticky='NSEW')

        self.combobox = ttk.Combobox(self)
        self.combobox['values'] = self.asciiSerial.scanPorts()
        self.combobox.grid(column=0, row=1, sticky='NSEW')

        self.b_connect = tk.Button(self, text="Connect", command=self.openClosePort)
        self.b_connect.grid(column=0, row=2, sticky='NSEW')

    def getSerialPort(self):
        return self.combobox.get()

    def openClosePort(self, closeWithRed=False):
        if self.portOpen:
            self.asciiSerial.close()
            self.portOpen = False
            self.b_connect['text'] = "Connect"
            if closeWithRed:
                self.b_connect['bg'] = 'red'
            else:
                self.b_connect['bg'] = 'gray95'
        else:
            try:
                self.asciiSerial.open(self.getSerialPort())
                self.portOpen = True
                self.b_connect['text'] = "Disconnect"
                self.b_connect['bg'] = 'chartreuse'
            except IOError:
                self.combobox['values'] = self.asciiSerial.scanPorts()
                self.b_connect['bg'] = 'red'


class ZoomButton(tk.Frame):
    """
        Boutons permettant de changer l'échelle temporelle des graphiques
    """
    def __init__(self, master=None):
        super().__init__(master)

        self.scale_list = [100, 200, 300, 500, 700, 1000, 1500, 2000, 2500, 3000, 4000, 5000, 7000, 10000, 15000, 20000, 30000]
        self.scale_index = 9

        self.scaleDisplay = tk.Label(self)
        self.scaleDisplay.grid(column=0, row=1, sticky='NSEW')

        self.b_plus = tk.Button(self, command=self.unZoom, text="+")
        self.b_plus.grid(column=0, row=0, sticky='NSEW')

        self.b_minus = tk.Button(self, command=self.zoom, text="-")
        self.b_minus.grid(column=0, row=2, sticky='NSEW')

        self.updateLabel()

    def zoom(self):
        if self.scale_index > 0:
            self.scale_index -= 1
            self.updateLabel()

    def unZoom(self):
        if self.scale_index < len(self.scale_list) - 1:
            self.scale_index += 1
            self.updateLabel()

    def updateLabel(self):
        self.scaleDisplay["text"] = str(self.getScale()) + " ms"

    def getScale(self):
        return self.scale_list[self.scale_index]


class DisplayArea(tk.Frame):
    """
        Zone de dessin dédiée aux consoles et aux graphes
    """
    def __init__(self, master=None):
        super().__init__(master)

        self.mainConsole = Console(self, readonly=False, bgColor='black', fgColor='gray95')
        self.warningConsole = Console(self, readonly=True, bgColor='black', fgColor='#ff7800')
        self.errorConsole = Console(self, readonly=True, bgColor='black', fgColor='red')

        self.graph1 = Graph(self)
        self.graph2 = Graph(self)
        self.graph3 = Graph(self)
        self.graph4 = Graph(self)

        self.columnconfigure(0, weight=1)
        self.columnconfigure(1, weight=1)
        self.columnconfigure(2, weight=1)
        self.columnconfigure(3, weight=1)
        self.rowconfigure(0, weight=1)
        self.rowconfigure(1, weight=1)

        self.oneLastPlot_g1 = False
        self.oneLastPlot_g2 = False
        self.oneLastPlot_g3 = False
        self.oneLastPlot_g4 = False

        self._lastState = None
        self.graphData = None

    def updateDisplay(self, state):
        if state != self._lastState:
            if state['console']:
                self.mainConsole.grid(column=0, row=0, rowspan=2, sticky='NSEW')
            else:
                self.mainConsole.grid_forget()
            if state['warning']:
                if state['error']:
                    self.warningConsole.grid(column=1, row=0, rowspan=1, sticky='NSEW')
                    self.errorConsole.grid(column=1, row=1, rowspan=1, sticky='NSEW')
                else:
                    self.errorConsole.grid_forget()
                    self.warningConsole.grid(column=1, row=0, rowspan=2, sticky='NSEW')
            else:
                self.warningConsole.grid_forget()
                if state['error']:
                    self.errorConsole.grid(column=1, row=0, rowspan=2, sticky='NSEW')
                else:
                    self.errorConsole.grid_forget()

            if self._lastState is None or state['graphScale'] != self._lastState['graphScale']:
                self.graph1.setScale(state['graphScale'])
                self.graph2.setScale(state['graphScale'])
                self.graph3.setScale(state['graphScale'])
                self.graph4.setScale(state['graphScale'])

            if self._lastState is None or state['graph1']['channel'] != self._lastState['graph1']['channel']:
                self.graph1.setLabel("graph1", state['graph1']['channel'])
            if self._lastState is None or state['graph2']['channel'] != self._lastState['graph2']['channel']:
                self.graph2.setLabel("graph2", state['graph2']['channel'])
            if self._lastState is None or state['graph3']['channel'] != self._lastState['graph3']['channel']:
                self.graph3.setLabel("graph3", state['graph3']['channel'])
            if self._lastState is None or state['graph4']['channel'] != self._lastState['graph4']['channel']:
                self.graph4.setLabel("graph4", state['graph4']['channel'])

            if state['graph1']['show']:
                if state['graph2']['show']:
                    self.graph1.grid(column=2, row=0, rowspan=1, sticky='NSEW')
                    self.graph2.grid(column=2, row=1, rowspan=1, sticky='NSEW')
                else:
                    self.graph2.grid_forget()
                    self.graph1.grid(column=2, row=0, rowspan=2, sticky='NSEW')
            else:
                self.graph1.grid_forget()
                if state['graph2']['show']:
                    self.graph2.grid(column=2, row=0, rowspan=2, sticky='NSEW')
                else:
                    self.graph2.grid_forget()

            if state['graph3']['show']:
                if state['graph4']['show']:
                    self.graph3.grid(column=3, row=0, rowspan=1, sticky='NSEW')
                    self.graph4.grid(column=3, row=1, rowspan=1, sticky='NSEW')
                else:
                    self.graph4.grid_forget()
                    self.graph3.grid(column=3, row=0, rowspan=2, sticky='NSEW')
            else:
                self.graph3.grid_forget()
                if state['graph4']['show']:
                    self.graph4.grid(column=3, row=0, rowspan=2, sticky='NSEW')
                else:
                    self.graph4.grid_forget()

            self._lastState = copy.deepcopy(state)

    def rePlotData(self, data):
        self.graphData = data
        if self._lastState['graph1']['enable']:
            self.oneLastPlot_g1 = True
            self.graph1.updateDataContent(self.graphData['graph1'])
            self.graph1.plot()
        elif self.oneLastPlot_g1:
            self.oneLastPlot_g1 = False
            self.graph1.updateDataStructure()
            self.graph1.plot()

        if self._lastState['graph2']['enable']:
            self.oneLastPlot_g2 = True
            self.graph2.updateDataContent(self.graphData['graph2'])
            self.graph2.plot()
        elif self.oneLastPlot_g2:
            self.oneLastPlot_g2 = False
            self.graph2.updateDataStructure()
            self.graph2.plot()

        if self._lastState['graph3']['enable']:
            self.oneLastPlot_g3 = True
            self.graph3.updateDataContent(self.graphData['graph3'])
            self.graph3.plot()
        elif self.oneLastPlot_g3:
            self.oneLastPlot_g3 = False
            self.graph3.updateDataStructure()
            self.graph3.plot()

        if self._lastState['graph4']['enable']:
            self.oneLastPlot_g4 = True
            self.graph4.updateDataContent(self.graphData['graph4'])
            self.graph4.plot()
        elif self.oneLastPlot_g4:
            self.oneLastPlot_g4 = False
            self.graph4.updateDataStructure()
            self.graph4.plot()


class Console(tk.Frame):
    """
        Console d'affichage des informations de débug
        Peut être en lecture seule ou proposer une zone d'écriture pour envoyer des commandes
    """
    def __init__(self, master=None, readonly=False, bgColor='gray95', fgColor='black', altFgColor='chartreuse'):
        super().__init__(master)

        self.alt = altFgColor
        self.altColoredTextCount = 0

        self.outputText = scroll.ScrolledText(self, bg=bgColor, fg=fgColor)
        self.outputText.config(state='disabled')
        self.outputText.grid(column=0, row=0, sticky='NSEW')

        self.inputText = tk.Text(self, height=1)
        if not readonly:
            self.inputText.grid(column=0, row=1, sticky='NSEW')

        self.userInputLines = []
        self.userInputHistory = []
        self.historyIndex = 0

        self.columnconfigure(0, weight=1)
        self.rowconfigure(0, weight=1)

    def addLines(self, lines, useAltColor=False):
        position = self.outputText.vbar.get()
        self.outputText['state'] = 'normal'
        for line in lines:
            self.outputText.insert('end', line)
        self.outputText['state'] = 'disabled'
        if position[1] == 1:
            self.outputText.see('end')

        if useAltColor:
            self.outputText.tag_add(str(self.altColoredTextCount), "end -" + str(len(lines)+1) + " l", "end -1 c")
            self.outputText.tag_configure(str(self.altColoredTextCount), foreground=self.alt)
            self.altColoredTextCount += 1

    def clear(self):
        self.outputText['state'] = 'normal'
        self.outputText.delete(1.0, 'end')
        self.outputText['state'] = 'disabled'

    def get_focus(self):
        self.inputText.focus_set()

    def getUserInputLines(self):
        inputLines = copy.deepcopy(self.userInputLines)
        self.userInputLines.clear()
        return inputLines

    def computeUserInput(self):
        userInput = self.inputText.get(1.0, 'end')
        if '\n\n' in userInput:
            if len(userInput) > 2:
                userInput = userInput[0:len(userInput)-1]
                self.userInputLines.append(userInput)
                self.userInputHistory.append(userInput[0:len(userInput)-1])
                self.historyIndex = 0
                self.addLines([userInput], useAltColor=True)
            self.inputText.delete(1.0, 'end')

    def goBackInTime(self):
        if self.historyIndex < len(self.userInputHistory):
            self.historyIndex += 1
        if self.historyIndex > 0:
            self.inputText.delete(1.0, 'end')
            self.inputText.insert('end', self.userInputHistory[-self.historyIndex])

    def backToTheFuture(self):
        if self.historyIndex > 0:
            self.inputText.delete(1.0, 'end')
            self.historyIndex -= 1
        if self.historyIndex > 0:
            self.inputText.insert('end', self.userInputHistory[-self.historyIndex])


class Graph(tk.Frame):
    """
        Graphique pyplot pour l'affichage des courbes
    """
    def __init__(self, master=None):
        super().__init__(master)

        self._label = ""
        self._data = {}
        self._dataShape = None
        self._scale = 0

        self._lines = []
        self._xIndex = []

        self.figure = plt.figure()
        self.subplot = self.figure.add_subplot(111)

        self.canvas = FigureCanvasTkAgg(self.figure, master=self)
        self.canvas.get_tk_widget().grid(column=0, row=0, sticky='NSEW')

        self.columnconfigure(0, weight=1)
        self.rowconfigure(0, weight=1)

        self.autoUpdatePeriod = 1  # sec
        self.lastUpdateTime = time.time()
        self.title = None
        self.updateDataStructure()
        self.plot()

    def setScale(self, scale):
        self._scale = scale
        self._xIndex = np.linspace(1, self._scale, self._scale / DATA_RECEIVE_PERIOD)
        self.updateDataStructure()
        self.plot()

    def setLabel(self, graphName, channel):
        self._label = "[" + graphName + "] "
        if channel is not None:
            self._label += channel
        self.plot()

    def updateDataContent(self, data):
        structureChanged = False
        if len(data['data']) != len(self._data):
            structureChanged = True
        for lineName in data['data']:
            if lineName not in self._data:
                structureChanged = True
        if data['shape'] != self._dataShape:
            structureChanged = True
        self._data = data['data']
        self._dataShape = data['shape']
        if structureChanged or time.time() - self.lastUpdateTime > self.autoUpdatePeriod:
            self.updateDataStructure()

    def updateDataStructure(self):
        self._lines.clear()
        self.subplot.clear()
        for lineName, lineData in self._data.items():
            if self._dataShape == 'line':
                if len(lineData) < len(self._xIndex):
                    y = np.concatenate([np.zeros(len(self._xIndex) - len(lineData)), lineData])
                else:
                    y = lineData[-len(self._xIndex):len(lineData)]
                line, = self.subplot.plot(self._xIndex, y, label=lineName)
            elif self._dataShape == 'scatter':
                line, = self.subplot.plot(lineData[0], lineData[1], 'o', label=lineName)
                self.subplot.set_aspect('equal')
            elif self._dataShape == 'line-scatter':
                line, = self.subplot.plot(lineData[0], lineData[1], label=lineName)
            else:
                print("Warning: unsupported data shape (", self._dataShape, ")")
                line, = self.subplot.plot([], [], label=lineName)
            self._lines.append((lineName, line))
        self.title = self.subplot.set_title(self._label)
        self.subplot.legend(handles=[entry[1] for entry in self._lines], loc=2)
        self.lastUpdateTime = time.time()

    def plot(self):
        self.title.set_text(self._label)
        for lineName, line in self._lines:
            lineData = self._data[lineName]
            if self._dataShape == 'line':
                if len(lineData) < len(self._xIndex):
                    y = np.concatenate([np.zeros(len(self._xIndex) - len(lineData)), lineData])
                else:
                    y = lineData[-len(self._xIndex):len(lineData)]
                line.set_ydata(y)
            elif self._dataShape == 'scatter' or self._dataShape == 'line-scatter':
                line.set_xdata(lineData[0])
                line.set_ydata(lineData[1])
        self.canvas.draw()


app = LowLevelGUI(master=root)
root.protocol("WM_DELETE_WINDOW", app.on_exit)
root.mainloop()