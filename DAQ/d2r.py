#!/usr/local/bin/python
from time import time, strptime, mktime, strftime
from struct import unpack
from array import array

import os, sys
import numpy as np
import ROOT
from ROOT import TFile, TTree

# from os import path, popen
# from numpy import uint16

# from numpy import array, uint16, nan, histogram2d, rot90, flipud, ma, nanargmin, sum, subtract, nanargmax, nanmin
# from numpy import multiply, interp, isnan, zeros, sqrt, add, divide, average
# import matplotlib.pylab as plt
# from matplotlib.colors import LogNorm

PROJECT_PATH = os.path.realpath(os.path.dirname(__file__))


class D2R:
    """
    Class to covert caen binary file format to ROOT tree
    """

    class Header:
        """
        Header Information
        """
        def __init__(self):
            maxCh = 8
            self.runNo = array("i", [0])        # runNo
            self.startTime = array("i", [0])    # file start unix time
            self.recordLen = array("i", [0])    # bytes of each trigger
            self.numCh     = array("i", [0])    # number of channels used
            self.numSample = array("i", [0])    # number of samples per waveform
            self.triggerTreshold = array("i", maxCh*[0])    # threshold of each channel
            self.dcOffset = array("i", maxCh*[0])           # DC offset of each channel


        def __str__(self):  
            return """ Header:
             startTime: %s
             recordLen: %s
                 numCh: %s
             numSample: %s
              dcOffset: %s
       triggerTreshold: %s
            """ % (self.startTime[0], self.recordLen[0], self.numCh[0],
                   self.numSample, self.dcOffset, self.triggerTreshold)

    class Trigger:
        """
        Raw Trigger (Event) Information
        """
        def __init__(self, numCh):
            self.eventNo = array("i", [0])
            self.triggerTime = array("d", [0])    # trigger time since file start

            maxSample = 500
            for i in range(numCh):
                setattr(self, "ch%s"%(i,), array("H", maxSample*[0]))    # channel waveform

            # self.pmtData = ROOT.vector('vector<unsigned short>')()
            # for i in range(numCh):  # each channel waveform is stored inside a vector
            #     wf = ROOT.vector('unsigned short')()
            #     self.pmtData.push_back(wf)

        def __str__(self):
            return """ Trigger %s:
        triggerTime: %s
            """ % (self.eventNo[0], self.triggerTime)

    def __init__(self, fileName):
        """
        Initializes the dataFile instance to include the fileName, access time,
        and the number of boards in the file. Also opens the file for reading.
        """
        self.fileName = fileName
        if self.fileName[-4:] != '.dat':
            raise NameError("Not a data file")
        self.file = open(self.fileName, 'r')

        self.numCh = 1  # number of channels
        self.numSample = 252  # number of samples per waveform
        # self.numSample = 500  # number of samples per waveform

        self.dateTime = 0
        self.oldTimeTag = 0.
        self.timeTagRollover = 0

        self.header = D2R.Header()
        self.event = D2R.Trigger(self.numCh)
        # for i in range(self.numCh):
        #     self.event.pmtData[i].reserve(self.numSample)

        self.initOutput()

    def run(self, nEvent=-1):
        self.readHeader()
        if not nEvent == -1:
            for i in range(nEvent):
                self.getNextTrigger()
                # print self.event
                self.reset()
        else:
            while self.getNextTrigger():
                if self.event.eventNo[0] % 10000 == 0:
                    print "processing event", self.event.eventNo[0]/1000, "K ..."
                self.reset()

        self.close()


    def initOutput(self):
        self.d2rFile = TFile("test.root", "recreate")

        self.headerTree = TTree("Header", "Header Tree")
        self.headerTree.Branch("runNo", self.header.runNo, "runNo/I")
        self.headerTree.Branch("startTime", self.header.startTime, "startTime/I")
        self.headerTree.Branch("recordLen", self.header.recordLen, "recordLen/I")
        self.headerTree.Branch("numCh", self.header.numCh, "numCh/I")
        self.headerTree.Branch("triggerTreshold", self.header.triggerTreshold, "triggerTreshold[numCh]/I")

        self.eventTree = TTree("Event", "Event Tree")
        self.eventTree.Branch("eventNo", self.event.eventNo, "eventNo/I")
        self.eventTree.Branch("triggerTime", self.event.triggerTime, "triggerTime/D")
        # self.eventTree.Branch("pmtData", self.event.pmtData)
        for i in range(self.numCh):
            chName = "ch%i" % (i,)
            self.eventTree.Branch(chName, getattr(self.event, chName), "%s[%i]/s" % (chName, self.numSample, ))


    def close(self):
        self.headerTree.Write()
        self.eventTree.Write("", ROOT.TObject.kOverwrite)

        # self.d2rFile.Write()
        self.d2rFile.Close()
        self.file.close()

    def reset(self):
        self.event.triggerTime[0] = 0
        # self.event.ch0 (we don't need to reset this as it's renewed every time)
        # for i in range(self.numCh):
        #     self.event.pmtData[i].clear()

    def readHeader(self):
        """
        The .dat file has a headerSize defined by a 4 byte control word at the beginning of the file.
        This control word lists the number of 32bit long words in the header, but python needs bytes, so we convert
        by multiplying by 4 and then subtract off the length of the control word itself.

        This function sets values for:
        1. The recordLen, which is the length of data (in samples) recorded for one trigger,
        2. A time tuple containing the full start time of the data set (self.dateTime)
        Also set is an array that contains the entire header.
        """
        h0 = self.file.read(4)
        (i0,) = unpack('I', h0)

        # In .dat file, size is listed in number of 32 bit long words, python needs bytes.
        # Also subtracting off the binary control word
        headerSize = (i0 - 0xb0000000) * 4 - 4

        header = self.file.read(headerSize)
        headerArr = header.split('\n')
        recordLen = 0
        startTime = ""
        startDate = ""

        enable_input = False
        numCh = 0
        for line in headerArr:
            tmp = line.split()
            # print tmp
            if len(tmp) == 0: continue
            if tmp[0] == "DATE(M/D/Y)":
                startDate = tmp[1]
            if tmp[0] == "TIME":
                startTime = tmp[1]
            if tmp[0] == "RECORD_LENGTH":
                recordLen = int(tmp[1])

            if tmp[0] == "ENABLE_INPUT":
                if tmp[1] == "YES":
                    enable_input = True
                    numCh += 1
                else:
                    enable_input = False
            if enable_input:
                if tmp[0] == "DC_OFFSET":
                    self.header.dcOffset[numCh-1] = int(tmp[1])
                if tmp[0] == "TRIGGER_THRESHOLD":
                    self.header.triggerTreshold[numCh-1] = int(tmp[1])

        self.dateTime = strptime(startDate + " " + startTime, "%m/%d/%Y %H:%M:%S")

        self.header.runNo[0] = 0
        self.header.startTime[0] = int(mktime(self.dateTime))
        self.header.recordLen[0] = recordLen
        assert numCh ==  self.numCh
        self.header.numCh[0] = self.numCh
        self.header.numSample[0] = self.numSample
        print self.header
        self.headerTree.Fill()

    def getNextTrigger(self):
        """
        This function returns  the next trigger from the dataFile. It reads the control words into h[0-3], unpacks them,
        and then reads the next event. It returns a RawTrigger object, which includes the fileName, location in the
        file, and a dictionary of the traces
        :raise:IOError if the header does not pass a sanity check: (sanity = 1 if (i0 & 0xa0000000 == 0xa0000000) else 0
        """

        h0 = self.file.read(4)
        h1 = self.file.read(4)
        h2 = self.file.read(4)
        h3 = self.file.read(4)

        if h3 == '':
            return None

        (i0,) = unpack('I', h0)
        (i1,) = unpack('I', h1)
        (i2,) = unpack('I', h2)
        (i3,) = unpack('I', h3)

        sanity = 1 if (i0 & 0xa0000000 == 0xa0000000) else 0
        if sanity == 0:
            raise IOError('Read did not pass sanity check')
        eventSize = i0 - 0xa0000000
        # print "eventSize:",eventSize
        boardId = (i1 & 0xf8000000) >> 27
        channelUse = i1 & 0x000000ff
        whichChan = [1 if (channelUse & 1 << k) else 0 for k in range(0, 8)]
        numChannels = sum(whichChan)
        assert numChannels == self.header.numCh[0]

        zLE = True if i1 & 0x01000000 != 0 else False

        eventCounterMask = 0x00ffffff
        eventCounter = i2 & eventCounterMask
        triggerTimeTag = i3

        self.deltaTS = (float(i3) - self.oldTimeTag) * 8.e-9
        if i3 < self.oldTimeTag:
            self.timeTagRollover += 1
            self.deltaTS += (2**31)*8.e-9
            self.oldTimeTag = float(i3)
        else:
            self.oldTimeTag = float(i3)
            # trigger.deltaTS = (float(i3) - self.oldTimeTag)/(2**31)*17

        self.event.triggerTime[0] = self.header.startTime[0]+float(self.timeTagRollover*(2**31)*8e-9)+float(i3)*8e-9

        size = int(4 * eventSize - 16L)
        chNo = 0
        for ind, k in enumerate(whichChan):
            #           print "Board = ", boardId
            #           print "ChanUse = ", channelUse
            #           print "Which Chan = ", whichChan
            if k == 1:  # channel is enabled
                traceName = "b" + str(boardId) + "tr" + str(ind)

                if not zLE:  # current setup
                    trace = unpack('H' * (size / (2 * numChannels)), self.file.read((size / numChannels)))
                else:
                    trace = np.zeros(self.header.recordLen[0])
                    trace[:] = np.nan
                    (trSize,) = unpack('I', self.file.read(4))
                    m = 1
                    trInd = 0
                    while m < trSize:
                        (controlWord,) = unpack('I', self.file.read(4))
                        length = (controlWord & 0x001FFFFF)
                        good = controlWord & 0x80000000
                        if good:
                            tmp = array(unpack('H' * (length * 2), self.file.read(length * 4)))
                            trace[trInd:trInd + length * 2] = tmp
                        trInd += length * 2
                        m += 1 + (length if good else 0)
                # print len(trace)
                ch = getattr(self.event, "ch%i"%(chNo,))
                for i, x in enumerate(trace):
                    ch[i] = x
                    # self.event.pmtData[chNo].push_back(x)
                chNo += 1

        self.event.eventNo[0] += 1
        # print self.event.ch0
        self.eventTree.Fill()
        return self.event


    def getTrigger(self, filePos):
        """
        Seeks to the file position of a given trigger, reads that trigger, and then returns it.
        :param filePos: The file position of the beginning of the desired event.
        :return: The trigger object read.
        """
        self.file.seek(filePos)
        trig = self.getNextTrigger()
        return trig


if __name__ == "__main__":
    x = D2R("/Users/chaozhang/Projects/PROSPECT/PROSPECT2/data/HFIR_LiLS11_1/Cal_2014-12-04-04-41-30.dat")
    # x = D2R("/Users/chaozhang/Projects/PROSPECT/PROSPECT2/data/HFIR_LiLS11_0/120314-LiLS11-LLNL5inPMT-1453V-Co60.dat")

    x.run(20000)
