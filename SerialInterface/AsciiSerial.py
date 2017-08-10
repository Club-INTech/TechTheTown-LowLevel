import serial
import serial.tools.list_ports
import copy

import numpy as np
import math
import random


class AsciiSerial:
    def __init__(self):
        self._graphsChannels = {'graph1': None, 'graph2': None, 'graph3': None, 'graph4': None}
        self._enChannels = {'graph1': False, 'graph2': False, 'graph3': False, 'graph4': False}

        # Structure definition:
        # {'ChannelName': channelData('display', {'lineName': [lowLevelID, xFieldID, yFieldID(optional)], ... }), ... }
        self._channelsDataStructure = {
            'POSITION':     channelData('line-scatter', {'p': [0, 0, 1]}),
            'TRAJECTORY':   channelData('line-scatter', {'t': [1, 0, 1]}),
            'PID_V_G':      channelData('line',         {'setPoint': [2, 0], 'value': [2, 1], 'output': [2, 2]}),
            'PID_V_D':      channelData('line',         {'setPoint': [3, 0], 'value': [3, 1], 'output': [3, 2]}),
            'PID_TRANS':    channelData('line',         {'setPoint': [4, 0], 'value': [4, 1], 'output': [4, 2]}),
            'BLOCKING_M_G': channelData('line',         {'aimSpeed': [5, 0], 'realSpeed': [5, 1], 'isBlocked': [5, 2]}),
            'BLOCKING_M_D': channelData('line',         {'aimSpeed': [6, 0], 'realSpeed': [6, 1], 'isBlocked': [6, 2]}),
            'STOPPING_MGR': channelData('line',         {'speed': [7, 0], 'isStopped': [7, 1]}),
            'DIRECTION':    channelData('line',         {'aimDirection': [8, 0], 'realDirection': [8, 1]}),
            'SENSORS':      channelData('scatter',      {'sensorTest': [9, 0, 1]}),
            'PID_TRAJ':     channelData('scatter',      {'p': [0, 0, 1], 't': [1, 0, 1]}),
            'TRAJ_ERRORS':  channelData('line',         {'t': [10, 0], 'c': [10, 1], 'ac': [10, 2], 'ang': [10, 3], 'pos': [10, 4]}),
            'AX12_G':       channelData('line',         {'aim': [8, 4], 'real': [8, 2]}),
            'AX12_D':       channelData('line',         {'aim': [8, 5], 'real': [8, 3]})
        }

        self._shapeInitData = {
            'line': [],
            'line-scatter': [[], []],
            'scatter': [[], []]
        }

        self.linesToSend = []
        self.receivedLines_main = []
        self.receivedLines_warning = []
        self.receivedLines_error = []

        self.serial = serial.Serial()
        self.incomingLine = ""

        # Format des données :
        # {'graphN': {'data': {'lineName': lineData, ...}, 'shape': String}
        #
        # 'shape' peut être :
        #   "line"   : ligne continue reliant chaque point
        #   "scatter": nuage de points (x,y) indépendants
        #   "line-scatter: nuage de points (x,y) reliés entre eux
        #
        # Pour chaque 'shape', 'lineData' a une forme différente :
        #   "line"   : tableau à 1 dimension représentant les coordonnées y de chaque point
        #   "scatter": tableau t à 2 dimensions. t[0] est un tableau représentant x pour chaque point. t[1] représente y
        #   "line-scatter": idem que 'scatter'

        self.graphData = {'graph1': {'data': None, 'shape': None},
                          'graph2': {'data': None, 'shape': None},
                          'graph3': {'data': None, 'shape': None},
                          'graph4': {'data': None, 'shape': None}}

        self.phase = np.linspace(0, 10 * np.pi, 100)
        self.index = 0

    @staticmethod
    def scanPorts():
        return list(serial.tools.list_ports.comports())

    def open(self, port):
        self.serial.port = port.split(" ")[0]
        self.serial.open()

    def close(self):
        self.serial.close()

    def getChannelsList(self):
        channelsList = []
        for key in self._channelsDataStructure:
            channelsList.append(key)
        channelsList.sort()
        return channelsList

    def getChannelsFromID(self, identifier):
        channels = set()
        for channel, cData in self._channelsDataStructure.items():
            lines = cData.lineNames
            for lineName, lineIds in lines.items():
                if lineIds[0] == identifier:
                    channels.add(channel)
        return channels

    def getIDsFromChannel(self, channel):
        ids = set()
        lines = self._channelsDataStructure[channel].lineNames
        for lineName, lineIds in lines.items():
            ids.add(lineIds[0])
        return ids

    def communicate(self):
        if self.serial.is_open:
            for line in self.linesToSend:
                self.serial.write(line.encode('ascii'))
            self.linesToSend.clear()

            nbB = self.serial.in_waiting
            if nbB > 0:
                self.incomingLine += self.serial.read(nbB).decode(encoding='utf-8', errors='ignore')

            newLineIndex = self.incomingLine.find('\n')
            while newLineIndex != -1:
                self.addLineToProperList(self.incomingLine[0:newLineIndex+1])
                self.incomingLine = self.incomingLine[newLineIndex+1:]
                newLineIndex = self.incomingLine.find('\n')

    def addLineToProperList(self, line):
        if len(line) > 5 and line[0:6] == "_data_":
            try:
                self.addGraphData(line[6:])
            except ValueError:
                self.receivedLines_main.append(line)
        elif len(line) > 8 and line[0:9] == "_warning_":
            self.receivedLines_warning.append(line[9:])
        elif len(line) > 7 and line[0:7] == "_error_":
            splittedLine = line.split("_")
            errorLine = "#" + splittedLine[2] + "# "
            for s in splittedLine[3:]:
                errorLine += s
            self.receivedLines_error.append(errorLine)
        else:
            self.receivedLines_main.append(line)

    def addGraphData(self, strData):
        data = strData.split("_")
        idChannel = int(data[0])
        channels = self.getChannelsFromID(idChannel)

        values = []
        for strValue in data[1:]:
            values.append(float(strValue))

        for graph in ['graph1', 'graph2', 'graph3', 'graph4']:
            gChannel = self._graphsChannels[graph]
            if gChannel in channels and self._enChannels[graph]:
                lines = self._channelsDataStructure[gChannel].lineNames
                for lineName, ids in lines.items():
                    if ids[0] == idChannel:
                        if len(ids) == 2: # One dimension data
                            if len(values) <= 1:
                                raise ValueError
                            self.graphData[graph]['data'][lineName].append(values[ids[1]])
                        elif len(ids) == 3: # Two dimensions data
                            if len(values) <= 2:
                                raise ValueError
                            self.graphData[graph]['data'][lineName][0].append(values[ids[1]])
                            self.graphData[graph]['data'][lineName][1].append(values[ids[2]])


    def setEnabledChannels(self, competeConfig):
        newGraphsChannels = {'graph1': competeConfig['graph1']['channel'],
                           'graph2': competeConfig['graph2']['channel'],
                           'graph3': competeConfig['graph3']['channel'],
                           'graph4': competeConfig['graph4']['channel']}
        newEnabledList = {'graph1': competeConfig['graph1']['enable'],
                          'graph2': competeConfig['graph2']['enable'],
                          'graph3': competeConfig['graph3']['enable'],
                          'graph4': competeConfig['graph4']['enable']}

        commandLines = []
        graphs = ['graph1', 'graph2', 'graph3', 'graph4']
        for graph in graphs:
            if newGraphsChannels[graph] != self._graphsChannels[graph]:
                if self._enChannels[graph]:
                    commandLines += self.enableChannel(self._graphsChannels[graph], False)
            else:
                if newEnabledList[graph] != self._enChannels[graph]:
                    if not newEnabledList[graph]:
                        commandLines += self.enableChannel(self._graphsChannels[graph], False)

        for graph in graphs:
            if newGraphsChannels[graph] != self._graphsChannels[graph]:
                if newEnabledList[graph]:
                    self.resetGraphData(graph, newGraphsChannels[graph])
                    commandLines += self.enableChannel(newGraphsChannels[graph], True)
            else:
                if newEnabledList[graph] != self._enChannels[graph]:
                    if newEnabledList[graph]:
                        self.resetGraphData(graph, newGraphsChannels[graph])
                        commandLines += self.enableChannel(self._graphsChannels[graph], True)

        self._graphsChannels = newGraphsChannels
        self._enChannels = newEnabledList
        return commandLines

    def enableChannel(self, channel, enable):
        commandLines = []
        ids = self.getIDsFromChannel(channel)
        for i in ids:
            if enable:
                commandLine = "logon "
            else:
                commandLine = "logoff "
            commandLine += str(i)
            commandLine += '\n'
            self.addLinesToSend([commandLine])
            commandLines.append(commandLine)
        return commandLines

    def resetGraphData(self, graph, channel):
        cData = self._channelsDataStructure[channel]
        self.graphData[graph]['shape'] = cData.shape
        initData = self._shapeInitData[cData.shape]
        initDict = {}
        for name in cData.lineNames:
            initDict[name] = copy.deepcopy(initData)
        self.graphData[graph]['data'] = copy.deepcopy(initDict)

    def getLines_main(self):
        lines = copy.deepcopy(self.receivedLines_main)
        self.receivedLines_main.clear()
        return lines

    def getLines_warning(self):
        lines = copy.deepcopy(self.receivedLines_warning)
        self.receivedLines_warning.clear()
        return lines

    def getLines_error(self):
        lines = copy.deepcopy(self.receivedLines_error)
        self.receivedLines_error.clear()
        return lines

    def addLinesToSend(self, lines):
        self.linesToSend += lines

    def clearLinesToSend(self):
        self.linesToSend = []

    def getAllData(self):
        # y = np.multiply(np.sin(np.linspace(0, 6 * np.pi, 100) + self.phase[self.index]), self.index/20)
        # y2 = np.multiply(np.sin(np.linspace(0, 6 * np.pi, 100) + (self.phase[self.index] + 0.1)), self.index/30)
        # self.index = int(math.fmod((self.index + 1), len(self.phase)))
        # return {'graph1': {'data': {'pwm': y, 'bite': y2}, 'shape': 'line'},
        #         'graph2': {'data':
        #                        {'traj': [[0,1,5*random.random(),9,12,6,3],[0,2,3,6*random.random(),7,2,-3]],
        #                         'bite': [[0, 2, 4 * random.random(), 9, 12, 7, 3],
        #                                  [3, 2, 3, 5 * random.random(), 3, 2, -1]]},
        #                    'shape': 'scatter'},
        #         'graph3': {'data': {}, 'shape': 'line'},
        #         'graph4': {'data': {}, 'shape': 'line'}
        #         }

        for graph in ['graph1', 'graph2', 'graph3', 'graph4']:
            if self.graphData[graph]['data'] is not None:
                for key, value in self.graphData[graph]['data'].items():
                    if len(value) > 1000:
                        value = value[len(value) - 1000:]
                    #print(key, value)
        return self.graphData


class channelData:
    def __init__(self, shape, lineNames):
        self.shape = shape
        self.lineNames = lineNames