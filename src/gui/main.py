#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys
from PyQt5.QtWidgets import (QWidget, QTextEdit,
    QAction, QInputDialog, QFileDialog, QDial, QComboBox, QVBoxLayout, QHBoxLayout, QMainWindow, QPushButton, QSizePolicy, QLabel, QApplication, QLineEdit)
from PyQt5.QtGui import (QIcon, QFont)
from PyQt5.QtCore import (QFileInfo, QObject)

from collections import OrderedDict

class Effects():
    def __init__(self):
        self.effectName = ''
        self.effectArgs = []
        self.effectWorkDir = ''
        self.effectInputWAV = ''
        self.effectOutputWAV = ''

        self.list = OrderedDict([
            ('Efecto', self.noEffectWidget),
            ('Copy', self.copyWidget),
            ('Delay', self.delayWidget),
            ('Flanger', self.flangerWidget),
            ('Vibrato', self.vibratoWidget),
            ('Bitcrusher', self.bitcrusherWidget),
            ('Wah Wah', self.wahWahWidget),
        ])

    #def runEffect(self):
    def getEffects(self):
        return self.list.keys()

    def getEffectWidget(self, effectName):
        return self.list[effectName]

    def noEffectWidget(self):
        print("no effect")
        return ([], [])

    def copyWidget(self):
        print("copyWidget")
        return ([], [])

    def delayWidget(self):
        print("delayWidget")
        return ([], [])

    def flangerWidget(self):
        delayDial = self.getDialWidget('delay', 0, 15, 1)
        delayTitle = self.getTitleWidget('Delay (0-15ms):')
        delayValue = self.getValueWidget(delayDial)

        rateDial = self.getDialWidget('rate', 10, 500, 0.01)
        rateTitle = self.getTitleWidget('Rate (0.1-5Hz):')
        rateValue = self.getValueWidget(rateDial)

        ampDial = self.getDialWidget('amp', 65, 75, 0.01)
        ampTitle = self.getTitleWidget('Amp (0.65-0.75):')
        ampValue = self.getValueWidget(ampDial)

        return ([delayTitle, delayValue, rateTitle, rateValue, ampTitle, ampValue], [delayDial, rateDial, ampDial])

    def vibratoWidget(self):
        depthDial = self.getDialWidget('depth', 0, 3, 1)
        depthTitle = self.getTitleWidget('Depth (0-3ms):')
        depthValue = self.getValueWidget(depthDial)

        modDial = self.getDialWidget('mod', 10, 500, 0.01)
        modTitle = self.getTitleWidget('Mod Frequency (0.1-5Hz):')
        modValue = self.getValueWidget(modDial)

        return ([depthTitle, depthValue, modTitle, modValue], [depthDial, modDial])

    def bitcrusherWidget(self):
        print("bitcrusherWidget")
        return ([], [])

    def wahWahWidget(self):
        print("wahWahWidget")
        return ([], [])

    def getDialWidget(self, name, minimum, maximum, mod):
        dial = QDial(window)
        dial.setObjectName(name)
        dial.setWrapping(0)
        dial.setNotchesVisible(1)
        dial.setMinimum(minimum)
        dial.setMaximum(maximum)
        dial.modifier=mod
        dial.valueChanged[int].connect(self.changeDialValue)
        return dial

    def getTitleWidget(self, label):
        font = QFont()
        font.setUnderline(1)
        font.setWeight(75)
        title = QLabel(label)
        title.setFont(font)
        return title

    def getValueWidget(self, dial):
        value = QLabel(str(dial.minimum()*dial.modifier))
        value.setObjectName(dial.objectName()+'Value')
        return value

    def changeDialValue(self, value):
        dial = window.sender()
        dialValueWidget = window.findChild(QLabel, dial.objectName()+'Value')
        dialValueWidget.setText(str(value*dial.modifier))

class PedalerIA64(QWidget):
    def __init__(self, effects):
        super().__init__()
        self.initUI(effects)

    def initUI(self, effects):
        vbox = QVBoxLayout()
        self.effectWidgets = ([], [])

        ## file box layout ##
        self.fileBox = QHBoxLayout()

        # input file button
        self.inputBtn = QPushButton('Input Audio', self)
        self.inputBtn.clicked.connect(self.showFileInputDialog)
        # input file title
        self.lblInputFile = QLabel('', self)
        self.lblInputFile.setFixedWidth(350)

        # output file button
        self.outputBtn = QPushButton('Output Audio', self)
        self.outputBtn.clicked.connect(self.showFileOutputDialog)
        # output file title
        self.lblOutputFile = QLabel('', self)
        self.lblOutputFile.setFixedWidth(350)

        self.fileBox.addWidget(self.inputBtn)
        self.fileBox.addStretch(1)
        self.fileBox.addWidget(self.lblInputFile)
        self.fileBox.addStretch(1)
        self.fileBox.addWidget(self.outputBtn)
        self.fileBox.addStretch(1)
        self.fileBox.addWidget(self.lblOutputFile)
        self.fileBox.addStretch(1)

        ## combo boxes ##
        self.selectBox = QHBoxLayout()

        # effect combo box
        self.effectSelect = QComboBox(self)
        self.effectSelect.addItems(effects.getEffects())
        # handler
        self.effectSelect.currentIndexChanged[str].connect(self.onIndexEffectChange)

        # language combo box
        self.languageSelect = QComboBox(self)
        self.languageSelect.addItems(['C', 'Assembler'])

        self.selectBox.addWidget(self.effectSelect)
        self.selectBox.addWidget(self.languageSelect)

        # effect arguments box
        self.effectsBox = QVBoxLayout()
        self.effectLabelsBox = QHBoxLayout();
        self.effectDialsBox = QHBoxLayout();
        self.effectsBox.addLayout(self.effectLabelsBox)
        self.effectsBox.addLayout(self.effectDialsBox)

        vbox.addLayout(self.fileBox)
        vbox.addLayout(self.selectBox)
        vbox.addLayout(self.effectsBox)
        vbox.addStretch(1)
        # add all widgets
#        vbox.addWidget(self.inputBtn)
#        vbox.addWidget(self.outputBtn)
#        vbox.addWidget(self.lblInputFile)
#        vbox.addWidget(self.lblOutputFile)
#        vbox.addWidget(self.effectSelect)

        # main window size
        self.setGeometry(250, 100, 800, 600)
        self.setWindowTitle('PedalerIA64')
        self.setLayout(vbox)
        self.show()

    def showFileInputDialog(self):
        inputFileName = QFileDialog.getOpenFileName(self, 'Open file', '', 'WAV File (*.wav)', options=QFileDialog.HideNameFilterDetails)
        if inputFileName!='':
            inputFile = QFileInfo(str(inputFileName[0]))
            effects.effectInputWAV = inputFile.fileName()
            effects.effectWorkDir = inputFile.canonicalPath()
            self.lblInputFile.setText(inputFile.fileName())

    def showFileOutputDialog(self):
        outputFileName, ok = QInputDialog.getText(self, 'File Output Dialog',
            'Enter file output name:')
        if ok:
            outputFile = QFileInfo(str(outputFileName))
            effects.effectOutputWAV = outputFile.fileName()
            self.lblOutputFile.setText(outputFile.fileName())

    def onIndexEffectChange(self, i):
        if (self.effectWidgets!=([],[])):
            for widget in self.effectWidgets[0]:
                self.effectsBox.removeWidget(widget)
                widget.setParent(None)
            for widget in self.effectWidgets[1]:
                self.effectsBox.removeWidget(widget)
                widget.setParent(None)
        effects.effectName = i
        self.effectWidgets = effects.getEffectWidget(i)()
        for labels in self.effectWidgets[0]:
            self.effectLabelsBox.addWidget(labels)
        for dials in self.effectWidgets[1]:
            self.effectDialsBox.addWidget(dials)

    #def noEffectWidget():

if __name__ == '__main__':
    app = QApplication(sys.argv)
    effects = Effects()
    window = PedalerIA64(effects)

    sys.exit(app.exec_())
