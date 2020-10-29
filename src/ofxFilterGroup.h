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
	int size() { return filters.size(); }

	// Process all filters which have not been requested since the last time
	// this frunction was called.
	void processRemaining();
	
	// Filters that have not been processed recently should be deleted
	// (culled). Call this to delete filters that haven't exported
	// valid data recently.
	void removeUnused();

	// Reset all filters (resets internal states).
	void reset();


private:

	string name = "";
	string ruiGroupName = ""; // cannot contain ':' --> use '-' instead
	string opList = "";

	// These are the settings for all filters in this group
	// The size of this will not necessarily equal the number of ops, 
	// since some ops wrap around the op stack.
	vector<ofxFilterOpSettings*> opSettings;

	// These are all of the filters in a dict with key value of their name
	map<string, ofxFilter*> filters;

	// To receive updates
	void paramsUpdated(RemoteUIServerCallBackArg& arg);

	// What is the maximum lifespan of constituent filters?
	long maxLifespan();

};
