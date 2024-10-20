#ifndef TRACKING_CAMERA_H
#define TRACKING_CAMERA_H

// Not sure to use OpenCV or Tensorflow
// Later on add expressions
class TrackingCamera
{
public:
	TrackingCamera();

	bool set_detect_person(bool b) { return m_foundPerson = b; } // Returns true or false if face is detected in cam

	// Used for Tracking settings
	bool set_face_tracking(bool b) { return m_faceTrack = b; }
	bool set_head_tracking(bool b) { return m_headTrack = b; }
	bool set_blink_tracking(bool b) { return m_blinkTrack = b; }
	bool set_hand_tracking(bool b) { return m_handTrack = b; }
	bool set_upperbody_tracking(bool b) { return m_upperbodyTrack = b; }

	bool get_detect_person() const { return m_foundPerson; }
	bool get_hand_tracking() const { return m_headTrack; }
	bool get_face_tracking() const { return m_faceTrack; }
	bool get_upperbody_tracking() const { return m_upperbodyTrack; }

	// Display
	//void display_tracking_vision() const;
	// Display computer vision (Used as a debugger)
	bool set_vision_mark(bool b) { return m_camVision = b; } // Only shows tracking no face
	bool set_vision_normal(bool b) { return m_camNormal = b; } // Shows tracking and face
	bool get_vision_mark() const { return m_camVision; }
	bool get_vision_normal() const { return m_camNormal; }

private:
	bool m_foundPerson = false;
	bool m_handTrack = false;
	bool m_faceTrack = false;
	bool m_blinkTrack = false;
	bool m_headTrack = false;
	bool m_upperbodyTrack = false;
	bool m_camNormal = false;
	bool m_camVision = false;
};
#endif // TRACKING_CAMERA_H