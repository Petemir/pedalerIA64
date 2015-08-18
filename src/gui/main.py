#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys
from PyQt5.QtWidgets import (QWidget, QTextEdit,
    QAction, QInputDialog, QFileDialog, QComboBox, QVBoxLayout, QHBoxLayout, QPushButton, QSizePolicy, QLabel, QApplication, QLineEdit)
from PyQt5.QtGui import (QIcon)
from PyQt5.QtCore import (QFileInfo, QObject)

from collections import OrderedDict

class Effects():
    def __init__(self):
        self.effectName = ''
        self.effectArgs = []
        self.effectWorkDir = ''
        self.effectInputWAV = ''
        self.effectOutputWAV = ''

        self.effects = OrderedDict([
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
        return self.effects.keys()

    def noEffectWidget():
        print("no effect")

    def copyWidget():
        print("copyWidget")

    def delayWidget():
        print("delayWidget")

    def flangerWidget():
        print("flangerWidget")

    def vibratoWidget():
        print("vibratoWidget")

    def bitcrusherWidget():
        print("bitcrusherWidget")

    def wahWahWidget():
        print("wahWahWidget")

class PedalerIA64(QWidget):

    def __init__(self, effects):
        super().__init__()
        self.initUI(effects)

    def initUI(self, effects):
        vbox = QVBoxLayout()

        ## input file layout ##
        self.fileBox = QHBoxLayout()
        # input file button
        self.inputBtn = QPushButton('Input Audio', self)
        self.inputBtn.clicked.connect(self.showFileInputDialog)
        #self.inputBtn.setSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        #self.inputBtn.move(10, 10)
        self.fileBox.addWidget(self.inputBtn)

        # input file title
        self.lblInputFile = QLabel('', self)
        #self.lblInputFile.move(350, 10)
        #self.lblInputFile.setFixedWidth(500)
        self.fileBox.addStretch(1)
        self.fileBox.addWidget(self.lblInputFile)
        self.fileBox.addStretch(1)

        ## output file layout ##
        self.outputBox = QHBoxLayout()
        # output file button
        self.outputBtn = QPushButton('Output Audio', self)
        self.outputBtn.setSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        self.outputBtn.clicked.connect(self.showFileOutputDialog)
        #self.outputBtn.move(10, 50)

        # output file title
        self.lblOutputFile = QLabel('', self)
        #self.lblOutputFile.move(350, 50)
        #self.lblOutputFile.setFixedWidth(500)
        self.fileBox.addWidget(self.outputBtn)
        self.fileBox.addStretch(1)
        self.fileBox.addWidget(self.lblOutputFile)
        self.fileBox.addStretch(1)

        # effect combo box
        self.effectSelect = QComboBox(self)
        #self.effectSelect.move(10, 100)
        self.effectSelect.addItems(effects.getEffects())
        # handler
        self.effectSelect.currentIndexChanged[str].connect(self.onIndexEffectChange)

        # language combo box
        self.languageSelect = QComboBox(self)
        #self.languageSelect.move(200, 100)
        self.languageSelect.addItems(['C', 'Assembler'])

        vbox.addLayout(self.fileBox)
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
        effects.effectName = i
    #def noEffectWidget():

if __name__ == '__main__':
    app = QApplication(sys.argv)
    effects = Effects()
    window = PedalerIA64(effects)

    sys.exit(app.exec_())
