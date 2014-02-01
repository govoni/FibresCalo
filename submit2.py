#!/usr/bin/python


import sys
import os
import commands
from commands import getstatusoutput
import datetime


def replaceParameterInFile (inputFile, outputFile, replacementTag, replacementValue): 
    f = open (inputFile)
    s = f.read ()
    f.close ()
    s = s.replace (replacementTag, replacementValue)
    f = open (outputFile, 'w')
    f.write (s)
    f.close ()


# ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


def prepareConfigs (energy, outputFolder) :
    GPSfileName = outputFolder + '.mac'
    replaceParameterInFile ('template.mac', GPSfileName, 'ENERGY', energy)
    configFileName = outputFolder + '.cfg'
    replaceParameterInFile ('template.cfg', configFileName, 'GPSFILE', GPSfileName)
    return configFileName


# ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


def submitJob (jobID, outputFolder, configFileName):
    filename = 'job_' + outputFolder + '_' + jobID
    workingFolder = '/afs/cern.ch/user/g/govoni/scratch0/fibresCalo/FibresCalo'
    f = open (filename, 'w')
    commandOutput = getstatusoutput ('/afs/cern.ch/project/eos/installation/cms/bin/eos.select mkdir /eos/cms/store/user/govoni/upgrade/' + outputFolder)
    f.write ('cd ' + workingFolder + '\n')
    f.write ('source setup_lxplus.sh\n')
    f.write ('cd -\n')
    f.write ('cp ' + workingFolder + '/' + configFileName + ' ./\n')
    f.write ('cp ' + workingFolder + '/' + outputFolder + '.mac' + ' ./\n')
    f.write ('/afs/cern.ch/user/g/govoni/geant4_workdir/bin/Linux-g++/FibresCalo ' + configFileName + ' ' + filename + '\n')
    f.write (workingFolder + '/plotResolution ' + filename + '.root\n')
    f.write ('cmsStage ' + filename + '.root /store/user/govoni/upgrade/' + outputFolder + '\n')
    f.write ('cmsStage out_' + filename + '.root /store/user/govoni/upgrade/' + outputFolder + '\n')
    f.close ()
    getstatusoutput ('chmod 755 ' + filename)
    getstatusoutput ('bsub -q 2nd ' + filename)


# ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


if __name__ == '__main__':

    if len (sys.argv) < 3 : 
        print 'usage :', sys.argv[0], 'jobsNumber baseFolderName','\n'
        exit (1)
    jobsNum = int (sys.argv[1])
    energy = str (sys.argv[2])
    outputFolder = 'fix_' + energy 
    configFileName = prepareConfigs (energy, outputFolder)

    print 'submitting', jobsNum, 'jobs'
    for i in range (0, jobsNum):
        submitJob (str (i), outputFolder, configFileName)    
