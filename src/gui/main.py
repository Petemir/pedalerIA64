#!/usr/bin/python3
# -*- coding: utf-8 -*-

import subprocess, sys, decimal
from PyQt5.QtWidgets import (QWidget, QAction, QInputDialog, QFileDialog, QDial, QComboBox, QVBoxLayout, QHBoxLayout, QMainWindow, QMessageBox, QPushButton, QLabel, QApplication)
from PyQt5.QtGui import (QFont)
from PyQt5.QtCore import (QFileInfo, QObject)
from PyQt5.QtMultimedia import (QSound)

from collections import OrderedDict

class Effects():
    def __init__(self):
        self.effectName = ''
        self.effectCmd = ''
        self.effectArgs = []
        self.mainFile = ''
        self.inputWAV = ''
        self.outputWAV = ''

        self.list = OrderedDict([
            ('Efecto', (self.noEffectWidget, '')),
            ('Copy', (self.copyWidget, 'c')),
            ('Delay', (self.delayWidget, 'd')),
            ('Flanger', (self.flangerWidget, 'f')),
            ('Vibrato', (self.vibratoWidget, 'v')),
            ('Bitcrusher', (self.bitcrusherWidget, 'b')),
            ('Wah Wah', (self.wahWahWidget, 'w')),
        ])

    def getEffects(self):
        return self.list.keys()

    def getEffectWidget(self, effectName):
        return self.list[effectName][0]

    def noEffectWidget(self):
        noEffectLabel = QLabel('Seleccione algún efecto para continuar.')
        return ([noEffectLabel], [], [])

    def copyWidget(self):
        copyLabel = QLabel('Duplica el archivo de entrada.')
        return ([copyLabel], [], [])

    def delayWidget(self):
        delayLabel = QLabel('Efecto delay. Delay simple.')

        delayDial = self.getDialWidget('delay_delay', 0, 50, 0.1)
        delayTitle = self.getTitleWidget('Delay (0.0-5.0s):')
        delayValue = self.getValueWidget(delayDial)

        decayDial = self.getDialWidget('delay_decay', 0, 100, 0.01)
        decayTitle = self.getTitleWidget('Decay (0-100%):')
        decayValue = self.getValueWidget(decayDial)

        return ([delayLabel], [delayTitle, delayValue, decayTitle, decayValue], [delayDial, decayDial])

    def flangerWidget(self):
        flangerLabel = QLabel('Efecto flanger. Vibracion.')

        delayDial = self.getDialWidget('flanger_delay', 0, 15, 0.001)
        delayTitle = self.getTitleWidget('Delay (0-15ms):')
        delayValue = self.getValueWidget(delayDial)

        rateDial = self.getDialWidget('flanger_rate', 10, 100, 0.01)
        rateTitle = self.getTitleWidget('Rate (0.1-5Hz):')
        rateValue = self.getValueWidget(rateDial)

        ampDial = self.getDialWidget('flanger_amp', 65, 75, 0.01)
        ampTitle = self.getTitleWidget('Amp (0.65-0.75):')
        ampValue = self.getValueWidget(ampDial)

        return ([flangerLabel], [delayTitle, delayValue, rateTitle, rateValue, ampTitle, ampValue], [delayDial, rateDial, ampDial])

    def vibratoWidget(self):
        vibratoLabel = QLabel('Efecto vibrato. Vibracion.')

        depthDial = self.getDialWidget('vibrato_depth', 0, 3, 0.001)
        depthTitle = self.getTitleWidget('Depth (0-3ms):')
        depthValue = self.getValueWidget(depthDial)

        modDial = self.getDialWidget('vibrato_mod', 10, 500, 0.01)
        modTitle = self.getTitleWidget('Mod Frequency (0.1-5Hz):')
        modValue = self.getValueWidget(modDial)

        return ([vibratoLabel], [depthTitle, depthValue, modTitle, modValue], [depthDial, modDial])

    def bitcrusherWidget(self):
        bitcrusherLabel = QLabel('Efecto Bitcrusher. Reduce bits.')

        bitsDial = self.getDialWidget('bitcrusher_bits', 1, 16, 1)
        bitsTitle = self.getTitleWidget('Bits (1-16):')
        bitsValue = self.getValueWidget(bitsDial)

        freqDial = self.getDialWidget('bitcrusher_freq', 1, 11025, 1)
        freqTitle = self.getTitleWidget('Freq (1, 11025Hz):')
        freqValue = self.getValueWidget(freqDial)

        return ([bitcrusherLabel], [bitsTitle, bitsValue, freqTitle, freqValue], [bitsDial, freqDial])

    def wahWahWidget(self):
        wahwahLabel = QLabel('Efecto WahWah. Efecto loco.')

        dampDial = self.getDialWidget('wahwah_damp', 1, 10, 0.01)
        dampTitle = self.getTitleWidget('Damping Factor (0.01-0.10):')
        dampValue = self.getValueWidget(dampDial)

        minfDial = self.getDialWidget('wahwah_minf', 400, 1000, 1)
        minfTitle = self.getTitleWidget('Min cutoff freq (400-1000):')
        minfValue = self.getValueWidget(minfDial)

        maxfDial = self.getDialWidget('wahwah_maxf', 2500, 3500, 1)
        maxfTitle = self.getTitleWidget('Max cutoff freq (2500-3500):')
        maxfValue = self.getValueWidget(maxfDial)

        freqDial = self.getDialWidget('wahwah_freq', 1000, 3000, 1)
        freqTitle = self.getTitleWidget('WahWah Frequency (1000-3000):')
        freqValue = self.getValueWidget(freqDial)

        return ([wahwahLabel], [dampTitle, dampValue, minfTitle, minfValue, maxfTitle, maxfValue, freqTitle, freqValue], [dampDial, minfDial, maxfDial, freqDial])

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
        if isinstance(dial.modifier, float):
            cantDec = round(abs(decimal.Decimal(str(dial.modifier)).as_tuple().exponent),2)
            formatString = "{0:."+str(cantDec)+"f}"
            dialValueWidget.setText(formatString.format((value*dial.modifier)))
        else:
            dialValueWidget.setText(str(value*dial.modifier))

class PedalerIA64(QWidget):
    def __init__(self, effects):
        super().__init__()
        self.effectWidgets = ([], [], []) # description text, label widgets, dial widgets
        self.initUI(effects)

    def initUI(self, effects):
        vbox = QVBoxLayout()

        ## code box layout ##
        self.codeBox = QHBoxLayout()
        # working directory button
        self.mainBtn = QPushButton('Archivo main')
        self.mainBtn.clicked.connect(self.showMainFileDialog)
        self.codeBox.addWidget(self.mainBtn)

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

        ## combo boxes layout ##
        self.selectBox = QHBoxLayout()
        # effect combo box
        self.effectSelect = QComboBox(self)
        self.effectSelect.addItems(effects.getEffects())
        # handler
        self.effectSelect.currentIndexChanged[str].connect(self.onIndexEffectChange)
        # language combo box
        self.languageSelect = QComboBox(self)
        self.languageSelect.addItems(['C', 'Assembler'])
        self.languageSelect.currentIndexChanged[str].connect(self.onIndexLanguageChange)

        self.selectBox.addWidget(self.effectSelect)
        self.selectBox.addWidget(self.languageSelect)

        ## effect arguments box layout ##
        self.effectsBox = QVBoxLayout()
        self.effectDescBox = QHBoxLayout();
        self.effectLabelsBox = QHBoxLayout();
        self.effectDialsBox = QHBoxLayout();
        self.effectsBox.addLayout(self.effectDescBox)
        self.effectsBox.addSpacing(20)
        self.effectsBox.addLayout(self.effectLabelsBox)
        self.effectsBox.addLayout(self.effectDialsBox)

        ## run effect ##
        self.runProgramBox = QHBoxLayout()
        self.runProgramBtn = QPushButton('Ejecutar programa.')
        self.runProgramBtn.clicked.connect(self.showRunProgramDialog)
        self.runProgramBox.addWidget(self.runProgramBtn)

        ## play audios ##
        self.playButtonsBox = QHBoxLayout()

        ## add boxes to main vertical layout ##
        vbox.addLayout(self.codeBox)
        vbox.addLayout(self.fileBox)
        vbox.addSpacing(10)
        vbox.addLayout(self.selectBox)
        vbox.addLayout(self.effectsBox)
        vbox.addStretch(3)
        vbox.addLayout(self.runProgramBox)
        vbox.addStretch(1)
        vbox.addLayout(self.playButtonsBox)

        # main window size
        self.setGeometry(250, 100, 800, 600)
        self.setWindowTitle('PedalerIA64')
        self.setLayout(vbox)
        self.show()

    def showMainFileDialog(self):
        mainFileName = QFileDialog.getOpenFileName(self, 'Open file', '', '')
        if mainFileName!='':
            effects.mainFile = QFileInfo(str(mainFileName[0]))

    def showFileInputDialog(self):
        inputFileName = QFileDialog.getOpenFileName(self, 'Open file', '', 'WAV File (*.wav)', options=QFileDialog.HideNameFilterDetails)
        if inputFileName!='':
            effects.inputWAV = QFileInfo(str(inputFileName[0]))
            self.lblInputFile.setText(effects.inputWAV.fileName())

    def showFileOutputDialog(self):
        outputFileName, ok = QInputDialog.getText(self, 'File Output Dialog',
            'Enter file output name:')
        if ok:
            effects.outputWAV = QFileInfo(str(outputFileName))
            self.lblOutputFile.setText(effects.outputWAV.fileName())

    def showRunProgramDialog(self):
        if(window.findChild(QPushButton, 'play_input') or window.findChild(QPushButton, 'play_output')):
            self.playButtonsBox.removeWidget(self.playInputBtn)
            self.playButtonsBox.removeWidget(self.playOutputBtn)
            self.playInputBtn.setParent(None)
            self.playOutputBtn.setParent(None)
        if (effects.mainFile=='' or effects.inputWAV=='' or effects.outputWAV=='' or effects.effectName==''):
            txt = []
            if effects.inputWAV=='':
                txt.append('Audio Input')
            if effects.outputWAV=='':
                txt.append('Audio Output')
            if effects.effectName=='':
                txt.append('Effect')
            if effects.mainFile== '':
                txt.append('Main file')
            QMessageBox.information(self, "Error", "Falta seleccionar el/los siguiente/s argumento/s: \n"+', '.join(txt))
        else:
            effects.effectArgs = []
            for dial in self.effectWidgets[2]:
                if isinstance(dial.modifier, float):
                    cantDec = round(abs(decimal.Decimal(str(dial.modifier)).as_tuple().exponent),2)
                    formatString = "{0:."+str(cantDec)+"f}"
                    value = formatString.format((dial.value()*dial.modifier))
                else:
                    value = dial.value()*dial.modifier
                effects.effectArgs.append(str(value))

            cmdToPrint = ['./'+effects.mainFile.fileName(), effects.inputWAV.fileName(), effects.outputWAV.fileName(), '-'+effects.effectCmd, ' '.join(effects.effectArgs)]
            cmdToExecute = [effects.mainFile.absoluteFilePath(), effects.inputWAV.absoluteFilePath(), effects.mainFile.absolutePath()+'/'+effects.outputWAV.fileName(), '-'+effects.effectCmd]+effects.effectArgs

            reply = QMessageBox.question(self, 'Aplicar efecto', '¿Desea ejecutar el siguiente comando?\n'+' '.join(cmdToPrint), QMessageBox.Yes, QMessageBox.No)

            if reply==QMessageBox.Yes:
                res = subprocess.call(cmdToExecute, stderr=subprocess.STDOUT)
                if res != 0:
                    QMessageBox.information(self, "Error", "Hubo un error en la ejecución del programa. Por favor, correr el siguiente comando desde consola para verificar qué ocurrió:\n"+" ".join(cmdToExecute))
                else:
                    self.showPlayAudioBtns()

    def showPlayAudioBtns(self):
        self.playInputBtn = QPushButton('Play Input Audio', self)
        self.playInputBtn.setObjectName('play_input')
        self.playInputBtn.clicked.connect(self.playAudioInput)

        self.playOutputBtn = QPushButton('Play Output Audio', self)
        self.playOutputBtn.setObjectName('play_output')
        self.playOutputBtn.clicked.connect(self.playAudioOutput)
        self.playButtonsBox.addWidget(self.playInputBtn)
        self.playButtonsBox.addWidget(self.playOutputBtn)

    def playAudioInput(self):
        QSound.play(effects.inputWAV.absoluteFilePath())

    def playAudioOutput(self):
        QSound.play(effects.mainFile.absolutePath()+'/'+effects.outputWAV.fileName())

    def onIndexEffectChange(self, i):
        if self.effectWidgets!=([],[],[]):
            effects.effectName = ''
            effects.effectCmd = ''
            effects.effectArgs = []
            self.effectsBox.removeWidget(self.effectWidgets[0][0])
            self.effectWidgets[0][0].setParent(None)
            for widget in self.effectWidgets[1]:
                self.effectsBox.removeWidget(widget)
                widget.setParent(None)
            for widget in self.effectWidgets[2]:
                self.effectsBox.removeWidget(widget)
                widget.setParent(None)
        self.effectWidgets = effects.getEffectWidget(i)()
        self.effectDescBox.addWidget(self.effectWidgets[0][0])  # description
        for labels in self.effectWidgets[1]:
            self.effectLabelsBox.addWidget(labels)              # title, value
        for dial in self.effectWidgets[2]:
            self.effectDialsBox.addWidget(dial)                 # dial
        if i != 'Efecto':
            effects.effectName = i
            effects.effectCmd = effects.list[i][1]

    def onIndexLanguageChange(self, i):
        if i=='C':
            effects.effectCmd = effects.effectCmd.lower()
        elif i=='Assembler':
            effects.effectCmd = effects.effectCmd.upper()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    effects = Effects()
    window = PedalerIA64(effects)

    sys.exit(app.exec_())
