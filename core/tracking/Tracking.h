


#ifndef TRACKING_H
#define TRACKING_H

// Not sure to use OpenCV or Tensorflow

class Tracking
{
	// Display computer vision (Used as a debugger)
	bool show_vision_mark(bool b) { return camVision = b; } // Only shows tracking no face
	bool show_vision_normal(bool b) { return camNormal = b; } // Shows tracking and face


	bool set_detect_person(bool b) { return foundPerson = b; } // Returns true or false if face is detected in cam

	// Used for Tracking settings
	bool set_face_tracking(bool b) { return faceTrack = b; }
	bool set_head_tracking(bool b) { return headTrack = b; }
	bool set_blink_tracking(bool b) { return blinkTrack = b; }
	bool set_hand_tracking(bool b) { return handTrack = b; }
	bool set_upperbody_tracking(bool b) { return upperbodyTrack = b; }

	void detect_person() const;
	void get_hand_tracking() const;
	void get_face_tracking() const;
	void get_upperbody_tracking() const;

	// Emotions

	// Display
	void display_tracking_vision() const;

private:
	bool foundPerson = { false };

	bool handTrack, faceTrack, blinkTrack, headTrack, upperbodyTrack = { false };
	bool camNormal, camVision = { false };
};

#endif // TRACKING_H