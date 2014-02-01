#!/usr/bin/python


import sys
import os
import commands
from commands import getstatusoutput
import datetime
import argparse



def replaceParameterInFile (inputFile, outputFile, replacementTag, replacementValue): 
    f = open (inputFile)
    s = f.read ()
    f.close ()
    s = s.replace (replacementTag, replacementValue)
    f = open (outputFile, 'w')
    f.write (s)
    f.close ()


# ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


def prepareConfigs (energy, outputFolder, bfield, events) :
    GPSfileName = outputFolder + '.mac'
    replaceParameterInFile ('template.mac', GPSfileName, 'ENERGY', energy)
    replaceParameterInFile (GPSfileName, GPSfileName, 'EVENTS', events)
    configFileName = outputFolder + '.cfg'
    replaceParameterInFile ('template.cfg', configFileName, 'GPSFILE', GPSfileName)
    replaceParameterInFile (configFileName, configFileName, 'BFIELD', bfield)
    return configFileName


# ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


def submitJob (jobID, outputFolder, configFileName, queue):
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
    getstatusoutput ('bsub -q ' + queue + ' + filename')


# ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


if __name__ == '__main__':

    parser = argparse.ArgumentParser (description = 'boh')
    parser.add_argument('-q', '--queue'  , default= '1nd',  help='batch queue (1nd)')
    parser.add_argument('-j', '--jobsNum', default= 1,      type=int, help='number of jobs (1)')
    parser.add_argument('-e', '--energy' , default= '50',   help='beam energy (50)')
    parser.add_argument('-o', '--outTag' , default= 'fix_', help='outfolder tag base name (fix_)')
    parser.add_argument('-b', '--bfield' , default= '0',    help='magnetic filed value (0)')
    parser.add_argument('-n', '--events' , default= '10000',help='number of generated events (10000)')
    
    args = parser.parse_args ()

    outputFolder = args.outTag + args.energy 
    configFileName = prepareConfigs (args.energy, outputFolder, args.bfield, args.events)

    print 'submitting', args.jobsNum, 'jobs', to queue args.queue
    for i in range (0,  args.jobsNum):
        submitJob (str (i), outputFolder, configFileName, args.queue)    
