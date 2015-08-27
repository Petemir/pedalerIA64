#!/usr/bin/python3
from collections import OrderedDict
import random, subprocess, sys

Effects = OrderedDict([
     ('delay',  {'cmd' : 'd',
                 'args' : OrderedDict([('delay',{'min' : 0, 'max' : 50, 'mod' : 0.1}),
                                        ('decay', {'min' : 70, 'max' : 100, 'mod' : 0.01})])
                }),
     ('flanger', {'cmd' : 'f',
                   'args' : OrderedDict([('delay', {'min' : 0, 'max' : 15, 'mod' : 0.001}),
                                        ('rate', {'min' : 10, 'max' : 100, 'mod' : 0.01}),
                                        ('amp', {'min' : 65, 'max' : 75, 'mod' : 0.01})])
                 }),
     ('vibrato', {'cmd' : 'v',
                  'args' : OrderedDict([('depth', {'min' : 0, 'max' : 3, 'mod' : 0.001}),
                                         ('mod', {'min' : 10, 'max' : 500, 'mod' : 0.01})])
                 }),
     ('bitcrusher', {'cmd' : 'f',
                  'args' : OrderedDict([('bits', {'min' : 1, 'max' : 16, 'mod' : 1}),
                                         ('freq', {'min' : 2048, 'max' : 11025, 'mod' : 1})])
                     }),
     ('wahwah', {'cmd' : 'w',
                  'args' : OrderedDict([('damp', {'min' : 1, 'max' : 10, 'mod' : 0.01}),
                                         ('minf', {'min' : 400, 'max' : 1000, 'mod' : 1}),
                                         ('maxf', {'min' : 2500, 'max' : 3500, 'mod' : 1}),
                                         ('freq', {'min' : 1000, 'max' : 3000, 'mod' : 1})])
                 })
     ])

if __name__ == '__main__':
    for effect in Effects.keys():
        cantIter = 100
        #cantIter = 1
        for inputFile in ['guitar.wav', 'gibson.wav', 'beirut.wav', 'DiMarzio.wav']:
            args = []
            for arg in Effects[effect]['args'].keys():
                min = Effects[effect]['args'][arg]['min']
                max = Effects[effect]['args'][arg]['max']
                mod = Effects[effect]['args'][arg]['mod']
                args.append(str(random.randint(min, max)*mod))

            for lang in ['c','asm']:
                filename = "-".join([inputFile[:-4], effect, lang, str(cantIter)+"iter", "-".join(args)])+".wav"
                if lang=='c':
                    cmd = effect[0].lower()
                else:
                    cmd = effect[0].upper()
                cmdToExecute = ["/home/petemir/Projects/orga2dantooine/pedalerIA64/src/main", '/home/petemir/Projects/orga2dantooine/pedalerIA64/src/inputExamples/'+inputFile, '/home/petemir/Projects/orga2dantooine/pedalerIA64/src/outputExamples/'+filename,
                str(cantIter), "-"+cmd," ".join(args)]
                cmdToExecuteWithValgrind = ["valgrind", "--tool=callgrind", "--callgrind-out-file=/home/petemir/Projects/orga2dantooine/pedalerIA64/src/callgrind/"+filename+".out", "--dump-instr=yes", "--collect-jumps=yes", "/home/petemir/Projects/orga2dantooine/pedalerIA64/src/main", '/home/petemir/Projects/orga2dantooine/pedalerIA64/src/inputExamples/'+inputFile, '/home/petemir/Projects/orga2dantooine/pedalerIA64/src/outputExamples/'+filename,
                str(cantIter), "-"+cmd," ".join(args)]

                #print(" ".join(cmdToExecuteWithValgrind))
                print(" ".join(cmdToExecute))
