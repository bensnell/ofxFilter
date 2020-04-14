#include "ofMain.h"
#include "ofxRemoteUIServer.h"
#include "ofxFilter.h"

class ofxFilterGroup {
public:
    
    ofxFilterGroup();
    ~ofxFilterGroup();

	// Setup the group with a name
	// All filters within this group will use the same number, type and
	// order of operators. The opList is a string of comma-delimited
	// op names. For example, "kalman,continuity" describes a two layer
	// filter with a kalman, then a continuity.
	void setup(string name, string opList);

	bool filterExists(string key);
	ofxFilter* getFilter(string key, bool bCreateIfNone = true);
	map<string, ofxFilter*> getFilters() { return filters; }

	// Process all filters which have not been requested since the last time
	// this frunction was called.
	void processRemaining();
	
	


private:

	string name = "";
	string ruiGroupName = ""; // cannot contain ':' --> use '-' instead
	string opList = "";

	// These are the settings for all filters in this group
	vector<ofxFilterOpSettings*> opSettings;

	// These are all of the filters in a dict with key value of their name
	map<string, ofxFilter*> filters;

	// To receive updates
	void paramsUpdated(RemoteUIServerCallBackArg& arg);

};
