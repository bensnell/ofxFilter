#include "ofMain.h"
#include "ofxRemoteUIServer.h"
#include "ofxFilter.h"

class ofxFilterGroup {
public:
    
    ofxFilterGroup();
    ~ofxFilterGroup();

	bool filterExists(string key);
	ofxFilter* getFilter(string key, bool bCreateIfNone = true);

	void setupParams(string name);
	void setup();


private:

	string groupName = "";
	string ruiGroupName = ""; // cannot contain ':' --> use '-' instead

	map<string, ofxFilter*> filters;

	ofxFilterSettings settings;

	// To receive updates
	void paramsUpdated(RemoteUIServerCallBackArg& arg);
	void updateAllParams();
};
