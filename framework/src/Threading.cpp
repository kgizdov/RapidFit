
//	ROOT Headers
#include "TSystem.h"
//	RapidFit Headers
#include "RapidRun.h"
#include "DataPoint.h"
#include "IDataSet.h"
#include "Threading.h"
#include "ClassLookUp.h"
//	System Headers
#ifdef _WIN32
	#include <windows.h>
#elif __APPLE__
	#include <sys/param.h>
	#include <sys/sysctl.h>
#else
	#include <unistd.h>
#endif
#include <vector>
#include <math.h>

using namespace::std;

//	This method returns the number of cores on the machine at run-time, _OR_ returns a compile time constant defined by
//	__NUM_RAPID_THREADS__ from the compiler option -D__NUM_RAPID_THREADS__=2
int Threading::numCores()
{
	int num_cores = 1;
	#ifndef __NUM_RAPID_THREADS__
		//	I would __LOVE__ to use ROOT's library check as a way of determining if we're in CINT
		//	OR even if __CINT__ has been defined....
		//
		//	However,	ROOT does things in a painful way when dealing with a global scope and so it's
		//			extremely difficult to determine if I was run as a library or a standalone exectuable
		//			If anyone knows of a variable defined _ONLY_ duing running _within_ CINT
		//				PLEASE LET ME KNOW	rcurrie@cern.ch
		string root_exe = "root.exe";
		string pathName = ClassLookUp::getSelfPath();
		if( pathName.find( root_exe ) == string::npos )		//	NOT running the root executable root.exe
		{
			#ifdef WIN32		//	Not tested
				SYSTEM_INFO sysinfo;
				GetSystemInfo(&sysinfo);
				num_cores = sysinfo.dwNumberOfProcessors;
			#elif __APPLE__		//	OS X (tested in 10.6)
				int nm[2];
				size_t len = 4;
				uint32_t count;

				nm[0] = CTL_HW; nm[1] = HW_AVAILCPU;
				sysctl(nm, 2, &count, &len, NULL, 0);

				if(count < 1) {
					nm[1] = HW_NCPU;
					sysctl(nm, 2, &count, &len, NULL, 0);
					if(count < 1) { count = 1; }
				}
				num_cores = count;
			#else			//	Linux
				num_cores = (int) sysconf(_SC_NPROCESSORS_ONLN);
			#endif
		} else {						//	running the ROOT executable root.exe
			num_cores = 1;
		}
	#else
		num_cores = (int)__NUM_RAPID_THREADS__;
	#endif

	return num_cores;
}

//	Method to return a vector of data subset(s)
vector<vector<DataPoint*> > Threading::divideData( IDataSet* input, int subsets )
{
	vector<vector<DataPoint*> > output_datasets;
	int subset_size = ( input->GetDataNumber() / subsets );

	for( int setnum = 0; setnum < subsets; ++setnum )
	{
		vector<DataPoint*> temp_dataset;
		for( unsigned int i=0; i< (unsigned) subset_size; ++i )
		{
			temp_dataset.push_back( input->GetDataPoint( setnum * subset_size + (int)i ) );
		}
		output_datasets.push_back( temp_dataset );
	}

	//	The theory goes that the subset_size >> subsets and hence lumping this all onto one subset is negligible in the effect on runtime and _MUCH_ easier to code
	if( (subsets+1)*subset_size != input->GetDataNumber() )
	{
		for( int i= subsets*subset_size; i< input->GetDataNumber() ; ++i )
		{
			output_datasets[0].push_back( input->GetDataPoint( i ) );
		}
	}

	return output_datasets;
}
