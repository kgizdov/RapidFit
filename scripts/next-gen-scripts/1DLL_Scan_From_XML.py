#	For any constants and such, will more than likely be used as we're doing complex stuff
from math import *
#	Interfacing with the command line arguments is required
import sys, os
#       Very useful for string manipulations
import string
#	Used for tagging the date/time into the job desc
import datetime
now = datetime.datetime.now()

#	This script is intended to be run as 'ganga script.py', it will submit itself and the relative configuration as a pyROOT job
is_ganga = "Ganga.Core" in sys.modules

#	USAGE:
#
#	ganga script_name.py xml_for_job.xml file1.root file2.root	to run on most backends
#
#	ganga script_name.py xml_for_job.xml LFN1  LFN2 		to run on the GRID
#

#	Configurables

job_name = "TOY-1D"

#	All the possible output files right now
#
#			THIS HAS TO BE CHANGED BASED ON THE OUTPUT FROM YOUR SCAN

output_file_list = [ 'LLScanData_newformat.root' ]

param_name="deltaGamma"
X_min=-2.
X_max=0.
STEPS=10
STEPS_PER_CORE=2

ROOT_VERSION='5.26.00b'

LFN_LIST=[]
FILE_LIST=[]

xml = str()

#	written up here for clarity, process all possible LFNs and PFNs that have to be processed
if is_ganga:
	for arg in sys.argv:
		if string.find( arg, "LFN:" ) != -1 :
			LFN_LIST.append( str( arg ) )
		elif string.find( arg, ".xml" ) != -1 :
			xml = str( arg )
		else:
			FILE_LIST.append( str( argv ) )
	print "LFNs:"
	print LFN_LIST
	print "FILEs:"
	print FILE_LIST

#       Splitter for 1DLL studies
def _1DLL_Splitter( XML='XML.xml', STEPS=10, STEPS_PER_CORE=2 ):
	args = []
	if int(STEPS) < 2:
		STEPS = 2
	step_size = (X_max - X_min)/(STEPS-1)

	param_min=0.
	param_val=0.
	i = 0
	for i in range( 0, int(STEPS/STEPS_PER_CORE) , 1 ):
		temp = []
		temp.append( str( XML ) )
		param_min = X_min + step_size * i * STEPS_PER_CORE
		param_val = X_min + step_size * ((i+1) * STEPS_PER_CORE-1)
		param_str = str(param_name) + "," + str( param_min ) + "," + str( param_val ) + "," + str( STEPS_PER_CORE )
		temp.append( param_str )
		args.append( temp )
	if int(STEPS/STEPS_PER_CORE)*STEPS_PER_CORE != STEPS:
		param_val = X_min + step_size * ((i+1) * STEPS_PER_CORE)
		param_str = str(param_name) + "," + str( param_val ) +","+ str( X_max ) + "," + str( STEPS - int(STEPS/STEPS_PER_CORE)*STEPS_PER_CORE )
		temp = []
		temp.append( str( XML ) )
		temp.append( param_str )
		args.append( temp )
	print args
	return args

#	GANGA JOB

#	This is the section of code which will be executed within ganga
if is_ganga:

	RapidFit_Path = os.environ.get("RAPIDFITROOT")
	if not RapidFit_Path:
		print ""
		print "\t\tCouldn't find $RAPIDFITROOT, Please check this!!"
		print ""
		sys.exit(-3)

	RapidFit_Library=RapidFit_Path+"/lib/libRapidRun.so"

	if not os.path.isfile( RapidFit_Library ):
		print "Please (re) compile RapidFit for submitting to a batch system"
		print "              run:    'make lib'           not just:   'make'"
		print ""
		print "This could also mean you haven't defined '$RAPIDFITROOT, please check!!"
		print ""
		sys.exit(-42)

	#	By definition of how this should be run!
	script_name = str( sys.argv[0] )

	#	i.e.	> ganga script.py some.xml

        #       Input Parameters
        script_onlyname = script_name
        script_list = string.split( script_name, "/" )
        if len( script_list ) == 1:
                script_onlyname = string.split( script_name, "\\" )
        script_onlyname = script_list[ int(len(script_list)-1) ]


	#	Create the job
	j = Job( application = Root( version = ROOT_VERSION ) )

	datetimeinfo = str( "_" + str( now.strftime("%Y-%m-%d_%H.%M") ) )
	#       Change the name of your job for records
	j.name = str(job_name + "_" + str(script_onlyname) + datetimeinfo)

	#
	j.application.script = File( name=script_name )
	#	Tell the script where the RapidFit library is
	j.inputsandbox = [ script_name, xml, RapidFit_Library ]


	#	Backend to submit jobs to
	#	This changes based on the system your on

	host_name = os.popen('hostname').readline()

	if ( string.find( host_name, "frontend" ) != -1 ):
		print "Running on ECDF, submitting to SGE"
		j.backend = SGE()
		j.outputsandbox = output_file_list
		sandbox_data = [ script_name, xml, RapidFit_Library ]
		for k in FILE_LIST:
			sandbox_data.append( k )
		j.inputsandbox = sandbox_data

	elif ( string.find( host_name, "lxplus" ) != -1 ):
		choice = int( raw_input("Running on LXPLUS, submit to 1) GRID 2) lxplus Batch or 3) Interactive?\t") )
		while ( choice != 1 ) and ( choice != 2 ):
			choice = int( raw_input( "try again...  " ) )
		if choice == 1:
			j.backend = Dirac()
			j.inputdata = LFN_LIST                  #       Point the job to the data
			j.backend.inputSandboxLFNs = LFN_LIST   #       Tell Dirac we need a local copy in order to process it
			sandbox_data = [ script_name, xml, RapidFit_Library ]
			#print sandbox_data
			for k in FILE_LIST:
				sandbox_data.append( k )
			#print sandbox_data
			j.inputsandbox = sandbox_data
			j.outputdata = output_file_list
		if choice == 2:
			j.backend = LSF()
			j.backend.queue = '8nh'         #       1nh, 8nh, 1nd, 2nd, 1nw, 2nw
			j.inputdata = FILE_LIST
			j.outputsandbox = j.outputdata.files
			j.outputdata=[]
		if choice == 3:
			j.backend = Interactive()
			j.inputdata = FILE_LIST

	elif ( string.find( host_name, "ppe" ) != -1 ):
		choice = int( raw_input("Running on PPE, submit to 1) CONDOR or 2) Interactive? ") )
		while ( choice != 1 ) and ( choice != 2 ):
			choice = int( raw_input( "try again... " ) )
		if choice == 1 :
			j.backend = Condor()
			j.inputdata = FILE_LIST
		if choice == 2 :
			j.backend = Interactive()
			j.inputdata = FILE_LIST

	else:
		print "Unknown system, just running the job, check this is what you want"
		j.inputdata = FILE_LIST

        #       Input Parameters
        FIT_XML = xml
        FIT_LIST = string.split( FIT_XML, "/" )
        if len( FIT_LIST ) == 1:
                FIT_LIST = string.split( FIT_XML, "\\" )

        #       just need the absolute name of the XML in order to run on the backend
        FIT_XML = FIT_LIST[ int(len(FIT_LIST)-1) ]

	#	Splitter to use for job
	j.splitter=ArgSplitter( args = _1DLL_Splitter( FIT_XML, STEPS, STEPS_PER_CORE ) )
	#	submit the job
	j.submit()



#	Actual pyROOT code which will be executed
if ( __name__ == '__main__' ) and ( not is_ganga ) :

	#	Just to record the input for any debugging
	for i in sys.argv:
		print i

	#	We want ROOT
	import ROOT

	#	Input Parameters
	FIT_XML = sys.argv[1]

	SCAN_RANGE = sys.argv[2]

	#	Load the RapidFit binary library
	ROOT.gSystem.Load("libRapidRun")


	#	RapidFit arguments
	args = ROOT.TList()
	#	Construct the RapidFit Arguments as you would when running the fitter binary
	args.Add( ROOT.TObjString( "RapidFit"     ) )
	args.Add( ROOT.TObjString( "-f"           ) )
	args.Add( ROOT.TObjString( str( FIT_XML ) ) )
	args.Add( ROOT.TObjString( "--doLLscan"    ) )
	args.Add( ROOT.TObjString( "--defineScan" ) )
	args.Add( ROOT.TObjString( str( SCAN_RANGE )) )

	#	Print the command that is being run for reference
	#print args

	#	Construct an instance of the Fitting algorithm
	fitter = ROOT.RapidRun( args )
	#	Run the Fit
	result = fitter.run()

	#	Exit
	ROOT.gApplication.Terminate()

