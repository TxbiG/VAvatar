#include "TrackingCamera.h"


TrackingCamera::TrackingCamera()
{
	if (Tracking::TrackingCamera::m_foundPerson)
	{
		if (m_faceTrack) {}

		if (m_upperbodyTrack) {}

		if (m_handTrack) {}
	}
}