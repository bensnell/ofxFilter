# ofxFilter

## Introduction

This addon is a realtime filter capable of smoothing and filtering streams of data in realtime. 

The base form of the data operated on is a transformation matrix (`glm::mat4`). The matrix describes all properties of a reference frame, including position, orientation and scale. However, not all of these attributes need be used. A filter can process a scalar, 2D point, 3D point, etc. using the appropriate `process()` function (see more below).

This filter is also capable of handling invalid input (when inputs are missing or obscured). See below for a use case.

## Requirements

- Openframeworks version 0.11.0+
- OF Addons, at the branches/commits linked below:
  - [ofxCv](https://github.com/local-projects/ofxCv/tree/project/lp.rtls-server)
  - [ofxRemoteUI](https://github.com/local-projects/ofxRemoteUI)

## Structure

An ofxFilter consists of a number of layered operators. Each operator acts on the data output by the previous operator and in so doing, information is transformed as it passes through a stack of layers. 

A layer is called an `ofxFilterOp`. Available layers include (where the accompanied string is the layer key): 
- None (`none`): This layer does not transform the data.
- Easing (`easing`): This layer applies an ease to the data.
- Kalman (`kalman`): This layer applies a kalman filter to the data. 
- Add Rate (`add-rate`): This layer adds differentiable rates (of order `n`) to the `mat4`, in the form of a `mat4rate` object. Rates describe the change of a value from one frame to another. Rates are not calculated by default, since not all ops require them. When an op requires rates, place this layer before it.
- Continuity (`continuity`): This layer is capable of making predictions to "fill in" missing data. Is is also capable of "merging" predicted data with known data, hence the name "continuity." It is designed to intelligently fill in gaps when new information is unavailable. It must follow the `add-rate` operator with `order=3`.

A group of filters that share the same settings (types, locations and quantity of ops) can be contained with an `ofxFilterGroup` dictionary, where each filter is accessible via a key string.

## Usage

ofApp.h

```c++
ofxFilterGroup group;
```

ofApp.cpp

```c++
void setup() {
  
  // Setup the Remote UI
	RUI_SET_CONFIGS_DIR("configs");
	RUI_SETUP();
	RUI_LOAD_FROM_XML();
  
	// Setup the Filter Group with a name and operator types. 
  // All filters in this group will process data using a kalman op, then an add-rate op,
  // then a continuity op, using the same settings.
	group.setup("myGroupName", "kalman,add-rate,continuity");
  
	// Filters are created as soon as they are queried. No need to create ahead of time.
}

void update() {
  
  // Update the first filter with new data, for example, mouse coordinates.
  if (mousePressed) {
		// If valid data is available, the process with it.
		glm::vec2 position = {mouseX, mouseY};
    filters.getFilter("mouseFilter")->process(position);
  } else {
	  // If new data is not available, we can process the filter without, and yield a 
	  // prediction as output.
    filters.getFilter("mouseFilter")->process();
  }
  
  // Update the second filter with new data, for example, rotation data.
  filters.getFilter("rotationFilter")->process(glm::quat({pitch, yaw, roll}));
}

void draw() {
  
  // Get the mouse prediction and draw it. For data streams with invalid data, we can
  // check the validity of predictions before drawing them.
  if (filters.getFilter("mouseFilter")->isDataValid()) {
    glm::vec2 position = filters.getFilter("mouseFilter")->getPosition2D();
    ofDrawCircle(position, 10);
  }
  
  // Get the other prediction data
  filters.getFilter("rotationFilter")->getOrientation();
}
```

For a complete demo, see the example provided in the folder `example`.



## Parameters

Following are the parameter groups and their `variables`, with names corresponding to those in the *ofxRemoteUISettings.rui* file. 

All parameters that change data incrementally from frame to frame, like easing params and exponents (powers) are normalized to 60 fps. This means that a change in FPS will not change the behavior of data with the same parameters.

### ofxFilterUnits

This is a singleton that contains global variables and conversations relevant across all filters.

`FPS` : frames per second

### ofxFilterOp

No parameters

### ofxFilterOpEasing

`Easing Param` : Fraction of information retained in an ease / lerp / mix. For example, `data = glm::mix(newData, lastData, param)`. Range: [0, 1).

`Frames To Reset` : After how many invalid frames will the filter be reset? Range: [0, +inf)

### ofxFilterOpKalman

`Smoothness` : How smooth should the data be? Range: (-inf, +inf)

`Rapidness` : How quick should the filter be to respond? Range: (-inf, +inf)

`Use Accel` : Should the filter use acceleration in its calculations?

`Use Jerk` : Should the filter use jerk and acceleration in its calculations?

`Predict Empty` : Should this filter make predictions when there is no data? This filter is capable of making predictions, but is not reliable in the long term because of divergent high-order rates. 

`Max Empty Pred` : The maximum number of consecutive predictions this filter will make on invalid (empty) input data. After this number of frames, the filter will stop making predictions (and export invalid data).

`Reset After Empty` : Should the filter reset after invalid data? Almost always, the answer is yes.

### ofxFilterOpAddRate

`Order` : What order of rates are to be calculated? Range: [1, +inf). 1 = no rates; 2 = velocity; 3 = velocity and acceleration; 4 = velocity, acceleration and jerk; etc.

`Frames To Reset` : After how many invalid frames will the filter be reset? Range: [0, +inf)

#### RateForwardParams

These parameters describe how data is used to update rates.

`Fast Ease` : Easing parameter describing how fast rates change when speeds are fast. Range: [0, 1).

`Slow Ease` : Easing parameter describing how fast rates change when speeds are slow. Range: [0, 1). 

`Default Ease` : Easing parameter describing how fast rates change by default. Range: [0, 1). The eases should have this relationship: `Fast Ease` < `Default Ease` < `Slow Ease`.

`Ease Power` : How much more easing is applied to rates, the higher their order is? Range: (0, +inf), where 1 = there is no difference across different orders.

`Max Trans Speed` : Maximum translational speed (in meters); used as a reference point to calculate an appropriate ease.

`Max Rot Speed` : Maximum rotational speed (in degrees); used as a reference point to calculate an appropriate ease.

`Max Scale Speed` : Maximum scale speed (unitless); used as a reference point to calculate an appropriate ease.

### ofxFilterOpContinuity

This operator has a few states, described by the booleans `bExporting`, `bLinked` and `data.bValid`. If exporting data, then `bExporting` is true. If `bLinked` is true, then the output is closely linked with the input and there is a minimal prediction. In this state, most of the input data is observed (known) (`data.bValid` = true). If `bLinked` is false, then the output is mostly predicted, attempting to "relink" with valid observations when they are seen again. In this state, most of the input data is invalid (absent) (`data.bValid` = false).

Relevant parameters include:

`Max Pred Frames` : For how many consecutive frames without valid data will this continue predicting? (This is the maximum span gap).

`Rate Order for Export` : At what rate order will this being exporting?

`Linked Recon Mode` : How is newly observed data reconciled while linked?

`Frames To Unlink` : 

`LookaheadFrames` : 

`New Link Recon Mode` : 

#### SimilarityParams

`Trans Thresh` : 

`Trans Mix` : 

`Rot Thresh` : 

`Rot Mix` : 

`Scale Thresh` : 

`Scale Mix` : 

`Num Rates` : 

`Rate Thresh Mult` : 

`Rate Weight` : 

#### LinkedFrictionParams

`Friction` : 

`Friction Rate Power`:

#### UnlinkedFrictionParams

`Friction` : 

`Friction Rate Power`:

#### ConvergenceParams

`Epsilon Power` : 

`Max Trans Speed` : 

`Max Rot Speed` : 

`Max Scale Speed` : 

`Approach Time` : 

`Approach Buf` : 

`Acc Step Power` : 

`Target Speed Ease` : 

`Acc Mag Ease` : 

#### RateReduceParams

`Opp Dir Mult` : 

`Aln Dir Mult` : 

`Power` : 















## Troubleshooting

### Orientations produce strange arifacts

This addon is untested on orientation/rotation data. Contributions and bug-fixes are welcome.

### Continuity doesn't work as expected

TODO







