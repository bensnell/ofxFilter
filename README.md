# ofxFilter

## Introduction

This addon is a realtime filter capable of smoothing and filtering streams of data in realtime. 

The base form of the data operated on is a transformation matrix (`glm::mat4`). The matrix describes all properties of a reference frame, including position, orientation and scale. However, not all of these attributes need be used. A filter can process a scalar, 2D point, 3D point, etc. using the appropriate `process()` function (see more below).

This filter is also capable of handling invalid input (when inputs are missing or obscured). In the example below, ofxFilter is used to "fill in" gaps of missing data. The "filling in" is in green; blue represents known data or data produced while observations are available.

![example-demo.gif](example-demo.gif)

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
- Axes (`axes`): This layer can adjust the coordinate system axes by changing the handedness (right or left) of a coordinate system and the up vector (X, Y, or Z) of that system.
- Age (`age`): This layer only allows data of a certain age to pass through. It is a "wrappable" layer, which means it wraps the entire stack in an operator. Only one of these operators can be used per filter.
- Persist (`persist`): This layer forces invalid data to be valid for a number of frames. It can be useful to force tracking across multiple consecutive frames when other operators would otherwise prevent this.
- Transform (`transform`): This layer will apply a transformation to the data. Available transformations include scale, rotate, and translation (applied in this order). Rotation is supplied via euler angles applied in Yaw-Pitch-Roll (YXZ) order.

A group of filters that share the same settings (types, locations and quantity of ops) can be contained with an `ofxFilterGroup` dictionary, where each filter is accessible via a key string.

A filter's operators are defined by a string containing each op key in order, separated by a comma. For example, `"kalman,add-rate,continuity"` describes the filter containing a Kalman operator, followed by an Add Rate operator, followed by a Continuity operator.

## Usage

Make sure to include the following Preprocessor Definitions (macros) in your project:

- `GLM_EXT_INCLUDED`
- `GLM_ENABLE_EXPERIMENTAL`

Then, usage follows this form:

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

`Frames To Unlink` : After how many empty frames will predictions unlink from observations?

`LookaheadFrames` : In an unlinked state, how many frames do we look ahead to reconcile our current heading with?

`Linked Recon Mode` : How is newly observed data reconciled while linked? Modes include: COPY_ALL, COPY_FRAME, and COPY_FRAME_AND_UPDATE_RATE.

`New Link Recon Mode` : How is newly linked data reconciled with observational data? (same options as above).

#### SimilarityParams

These parameters describe how a similarity decision is made. Given two pieces of data, the calculation that determines whether they are similar is governed by these parameters.

`Trans Thresh` : What is the threshold (in meters) by which two pieces of data can be consdiered similar, from a translational perspective?

`Trans Mix` : How much of translational similarity is used to judge overall similarity? This value is relative to the other mix parameters.

`Rot Thresh` : What is the threshold (in degrees) by which two pieces of data can be consdiered similar, from a rotational perspective?

`Rot Mix` : How much of rotational similarity is used to judge overall similarity? This value is relative to the other mix parameters.

`Scale Thresh` : What is the threshold (unitless) by which two pieces of data can be consdiered similar, from a scalar perspective?

`Scale Mix` : How much of scalar similarity is used to judge overall similarity? This value is relative to the other mix parameters.

`Num Rates` : How many rates do we compare for similarity? Range: [1, nRates]. 1 = position only; 2 = position and velocity, etc.

`Rate Thresh Mult` : How much are the higher order rate thresholds multiples of the original thresholds? Range: [1, +inf)

`Rate Weight` : How much are higher order rates weighted differently? Range: [0, +inf)

#### FrictionParams

These parameters describe how friction is applied. There are two sets of friction parameters: one for the linked state and another for the unlinked state.

`Friction` : The fraction of energy retained each step (frame).

`Friction Rate Power` : Friction gets progressively higher for higher order rates by a power function. This parameter describes how friction changes with rate order. Range: [-1, +inf). -1 = no change; 0 = constant value; 1 = linearly increasing with order; 2 = exponentially increasing by a power of 2 with order.

#### ConvergenceParams

These parameters describe how unlinked data attempts to converge with observed data (when observed data is available). Once converged, the predicted data relinks with observations.

`Epsilon Power` : What exponent (negative power of 10) describes a practical zero?

`Max Trans Speed` : Maximum reasonable translational speed (meters/sec) one would want data to move during convergence.

`Max Rot Speed` : Maximum reasonable rotational speed (degrees/sec) one would want data to move during convergence.

`Max Scale Speed` : Maximum reasonable scalar speed (/sec) one would want data to move during convergence.

`Approach Time` : How many seconds before convergence should data begin to slow down to the observed data's speed?

`Approach Buf` : Fraction of the approach time until the data's speed is the observed target's speed. Range: [0, 1).

`Acc Step Power` : How do speed thresholds differ for higher order rates? If this is 1, then higher order rates have the same maximum magnitude as those provided (`max __ speed`). If > 1, then higher order rates have a  smaller maximum magnitude than those provided. Range: [1, +inf).

`Target Speed Ease` : How much should the target speed be eased? Easing will prevent jumps in acceleration. Range: [0, 1). 0 = no easing; 0.5 = half ease. Note: this is one of the only easing parameters that is not normalized to framerate. Normalizing this results in poor quality performance.

`Acc Mag Ease` : How fast does the magnitude of the acceleration ease? Range: [0, 1). 0 = no easing; 0.995 = rounded corners. 

#### RateReduceParams

When data transitions from being observed to being predicted, high order rates (acceleration+) must be reduced to prevent undesireable motion artifacts. These parameters describe how much it is reduced and when.

`Opp Dir Mult` : By what fraction should a rate be reduced when its direction opposes the next lowest rate's direction? Range: [0, 1].

`Aln Dir Mult` : By what fraction should a rate be reduced when its direction is aligned with the next lowest rate's direction? Range: [0, 1].

`Power` : This exponent sensitizes the range between opposing and aligned rates. A value < 1 makes orthogonal rates closer to `Aln Dir Mult`, while a value > 1 makes orthogonal rates closer to `Opp Dir Mult`. Range: (0, +inf).

### ofxFilterOpAxes

In order to adjust a coordinate system's axes, the reference (source) handedness and up vector must be know. Supply those values in the first two variables:

`Src Handedness`: Source handedness of the coordinate system (for reference).

`Src Up Vector`: Source up vector of the coordinate system (for reference).

`Convert Handedness`: Boolean indicating whether handedness should be converted to the destination handedness.

`Dst Handedness`: Desired handedness of the system.

`Convert Up Vector`: Boolean indicating whether up vector should be converted to the destination up vector.

`Dst Up Vector`: Desired up vector of the system.

`Num Up Vec Rotations`: Number of 90º clockwise rotations around the up vector.

### ofxFilterOpAge

This operator wraps the entire operator stack. Only one can be used per filter. 

`Min Age`: Data must be at least this age (in frames) in order to be exported.

### ofxFilterOpPersist

This operator forces validity of invalid data for a number of frames.

`Num Frames`: Number of frames to persist validity.

### ofxFilterOpTransform

This operator applies a linear transformation matrix (from components scale, rotate and translate) to the data.

`Translate X`: X translation.

`Translate Y`: Y translation.

`Translate Z`: Z translation.

`Rotate X Pitch`: Euler angle (degrees) of pitch, or rotation around X.

`Rotate Y Yaw`: Euler angle (degrees) of yaw, or rotation around Y.

`Rotate Z Roll`: Euler angle (degrees) of roll, or rotation around Z.

`Scale X`: X scale.

`Scale Y`: Y scale.

`Scale Z`: Z scale.

## Troubleshooting

### Orientations produce strange artifacts

This addon is untested on orientation/rotation data. Contributions and bug-fixes are welcome.

### Continuity doesn't work as expected

TODO

### It doesn't run as fast as expected

The `kalman` operator is the most computationally intensive. On a computer with the following specs, the app was limited to processing 150 simultaneous 3D points at 240 fps:

- Processor	Intel(R) Core(TM) i7-9700K CPU @ 3.60GHz, 3600 Mhz, 8 Core(s), 8 Logical Processor(s)

### No member named `glm::eulerAngleYXZ`

Make sure to include the appropriate preprocessor macros [above](#usage).