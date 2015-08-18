#!/usr/bin/python3
# -*- coding: utf-8 -*-

"""
ZetCode PyQt5 tutorial

In this example, we select a file with a
QFileDialog and display its contents
in a QTextEdit.

author: Jan Bodnar
website: zetcode.com
last edited: January 2015
"""

import sys
from PyQt5.QtWidgets import (QMainWindow, QTextEdit,
    QAction, QInputDialog, QFileDialog, QComboBox, QVBoxLayout, QPushButton, QSizePolicy, QLabel, QApplication, QLineEdit)
from PyQt5.QtGui import (QIcon)
from PyQt5.QtCore import (QFileInfo)

effectsDict = {
    'Efecto': {},
    'Copy': {},
    'Delay': {},
    'Flanger': {},
    'Vibrato': {},
    'Bitcrusher': {},
    'Wah Wah': {},
    }

class Example(QMainWindow):

    def __init__(self):
        super().__init__()

        self.initUI()


    def initUI(self):
        self.statusBar()

        vbox = QVBoxLayout()

        # input file button
        self.inputBtn = QPushButton('Input Audio', self)
        self.inputBtn.setSizePolicy(QSizePolicy.Fixed,
            QSizePolicy.Fixed)
        self.inputBtn.clicked.connect(self.showFileInputDialog)
        self.inputBtn.move(10, 10)

        # input file title
        self.lblInputFile = QLabel('', self)
        self.lblInputFile.move(350, 10)
        self.lblInputFile.setFixedWidth(500)

        # output file button
        self.outputBtn = QPushButton('Output Audio', self)
        self.outputBtn.setSizePolicy(QSizePolicy.Fixed,
            QSizePolicy.Fixed)
        self.outputBtn.clicked.connect(self.showFileOutputDialog)
        self.outputBtn.move(10, 50)

        # output file title
        self.lblOutputFile = QLabel('', self)
        self.lblOutputFile.move(350, 50)
        self.lblOutputFile.setFixedWidth(500)

        self.effectSelect = QComboBox(self)
        self.effectSelect.move(10, 100)
        self.effectSelect.addItems(effectsDict.keys())

        self.languageSelect = QComboBox(self)
        self.languageSelect.move(200, 100)
        self.languageSelect.addItems(['C', 'Assembler'])

        vbox.addWidget(self.inputBtn)
        vbox.addWidget(self.outputBtn)
        vbox.addWidget(self.lblInputFile)
        vbox.addWidget(self.lblOutputFile)
        vbox.addWidget(self.effectSelect)

        self.setGeometry(250, 100, 800, 600)
        self.setWindowTitle('PedalerIA64')
        self.show()


    def showFileInputDialog(self):
        inputFileName = QFileDialog.getOpenFileName(self, 'Open file', '', 'WAV File (*.wav)', options=QFileDialog.HideNameFilterDetails)
        if inputFileName!='':
            inputFile = QFileInfo(str(inputFileName[0]))
            self.lblInputFile.setText(inputFile.fileName())
#    def showFileOutputDialog(self):
#        outputFileName = QFileDialog.getOpenFileName(self, 'Open file', '', 'Audio WAV File (*.wav)');

    def showFileOutputDialog(self):
        outputFileName, ok = QInputDialog.getText(self, 'File Output Dialog',
            'Enter file output name:')
        if ok:
            outputFile = QFileInfo(str(outputFileName))
            self.lblOutputFile.setText(outputFile.fileName())

if __name__ == '__main__':

    app = QApplication(sys.argv)
    ex = Example()
    sys.exit(app.exec_())
