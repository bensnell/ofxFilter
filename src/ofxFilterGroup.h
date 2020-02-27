#include "ofMain.h"
#include "ofxRemoteUIServer.h"
#include "ofxFilter.hpp"

class ofxFilterGroup {
public:
    
    ofxFilterGroup();
    ~ofxFilterGroup();

	glm::mat4x4 applyFilter(string _key, glm::mat4x4 _frame);
	float applyFilter(string _key, float _scalar);

	void setupParams(string name);
	void setup();


private:

	string groupName = "";
	string ruiGroupName = ""; // cannot contain ':' --> use '-' instead

	ofxFilter* getFilter(string _key);
	map<string, ofxFilter*> filters;

	ofxFilterMode mode = FRAME_FILTER_KALMAN;
	float smoothness = 0.001;
	float smoothnessExp = 3; // (0.1)^3
	float rapidness = 0.01;
	float rapidnessExp = 1; // (0.1)^1
	bool bUseAccel = false;
	float easingParam = 0.95;

	// To receive updates
	void paramsUpdated(RemoteUIServerCallBackArg& arg);
	void updateAllParams();
	void updateParams(ofxFilter* filter);
};
