#!/usr/bin/python


import sys
import os
import commands
from commands import getstatusoutput
import datetime


# ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


def prepareJob (jobID, outputFolder):
    filename = 'job_' + jobID
    workingFolder = '/afs/cern.ch/user/g/govoni/scratch0/fibresCalo/FibresCalo'
    f = open (filename, 'w')
    commandOutput = getstatusoutput ('/afs/cern.ch/project/eos/installation/cms/bin/eos.select mkdir /eos/cms/store/user/govoni/upgrade/' + outputFolder)
    f.write ('cd ' + workingFolder + '\n')
    f.write ('source setup_lxplus.sh\n')
    f.write ('cd -\n')
    f.write ('cp ' + workingFolder + '/config.cfg ./\n')
    f.write ('cp ' + workingFolder + '/gps.mac ./\n')
    f.write ('/afs/cern.ch/user/g/govoni/geant4_workdir/bin/Linux-g++/FibresCalo config.cfg ' + filename + '\n')
    f.write (workingFolder + '/plotResolution ' + filename + '.root\n')
    f.write ('cmsStage ' + filename + '.root /store/user/govoni/upgrade/' + outputFolder + '\n')
    f.write ('cmsStage out_' + filename + '.root /store/user/govoni/upgrade/' + outputFolder + '\n')
    f.close ()
    getstatusoutput ('chmod 755 job_' + jobID)
    getstatusoutput ('bsub -q 8nh job_' + jobID)


# ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


if __name__ == '__main__':

    if len (sys.argv) < 3 : 
        print 'usage :', sys.argv[0], 'jobsNumber baseFolderName','\n'
        exit (1)
    jobsNum = int (sys.argv[1])
    outputFolder = str (sys.argv[2])
        
    print 'submitting', jobsNum, 'jobs'
        
    for i in range (0, jobsNum):
        prepareJob (str (i), outputFolder)    
