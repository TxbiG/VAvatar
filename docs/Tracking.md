# Tracking

VAvatar tracks voice and camera data, then maps both into one neutral tracking state.

## Voice Tracking

Voice tracking is handled by `AudioTracking`.

It currently uses Windows WASAPI and exposes:

- raw microphone level
- smoothed microphone level
- speaking/quiet state
- threshold
- normalized mouth-open value
- microphone status text

The mouth-open value is blended with camera mouth-open so the avatar can still talk when face landmarks are weak.

## Camera Tracking

Camera tracking can run in two modes.

Preview mode generates simulated face/head/mouth data for UI and mapping development.

OpenCV mode uses:

- Haar cascade face detection
- LBF facial landmarks
- head yaw, pitch, and roll
- blink detection
- mouth-open detection
- smile detection
- basic camera disconnect detection

Expected default files:

```text
res/data/haarcascade_frontalface_default.xml
res/data/lbfmodel.yaml
```

The Editor lets the user browse for these files and saves them into the project `Tracking/` folder.

## Calibration

Camera calibration includes:

- head yaw/pitch/roll offsets
- blink threshold
- mouth closed baseline
- mouth range
- smoothing

Voice calibration includes:

- speaking threshold
- mouth response

Calibration presets are saved as `.vcal` files, normally inside:

```text
ProjectFolder/Calibration/
```

## Moss Microphone Direction

MossFramework includes microphone APIs, but the current framework checkout still has public API mismatches around `Moss_Microphone`. VAvatar keeps voice tracking behind `AudioTracking` so a Moss microphone backend can be added later without changing the app-level tracking pipeline.
