#include "CameraTrackingOpenCV.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <vector>

#if defined(VAVATAR_ENABLE_OPENCV)
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/face.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/videoio.hpp>
#endif

namespace {
// Clamp calibration values to the ranges supported by the tracker.
float clampCalibrationValue(float value, float minValue, float maxValue)
{
    return std::clamp(value, minValue, maxValue);
}

// Smooth numeric face values while keeping boolean detection states immediate.
FaceTrackingState blendState(const FaceTrackingState& previous, const FaceTrackingState& next, float smoothing)
{
    if (!next.foundPerson || !previous.foundPerson) return next;

    const float alpha = std::clamp(smoothing, 0.0f, 1.0f);
    auto blend = [alpha](float before, float after) { return before + (after - before) * alpha; };
    FaceTrackingState result = next;
    result.headYaw = blend(previous.headYaw, next.headYaw);
    result.headPitch = blend(previous.headPitch, next.headPitch);
    result.headRoll = blend(previous.headRoll, next.headRoll);
    result.mouthOpen = blend(previous.mouthOpen, next.mouthOpen);
    result.leftEyeOpen = blend(previous.leftEyeOpen, next.leftEyeOpen);
    result.rightEyeOpen = blend(previous.rightEyeOpen, next.rightEyeOpen);
    result.smile = blend(previous.smile, next.smile);
    result.mouthForm = blend(previous.mouthForm, next.mouthForm);
    result.eyeGazeX = blend(previous.eyeGazeX, next.eyeGazeX);
    result.eyeGazeY = blend(previous.eyeGazeY, next.eyeGazeY);
    result.browLeft = blend(previous.browLeft, next.browLeft);
    result.browRight = blend(previous.browRight, next.browRight);
    result.shoulderYaw = blend(previous.shoulderYaw, next.shoulderYaw);
    result.shoulderPitch = blend(previous.shoulderPitch, next.shoulderPitch);
    if (previous.handLandmarkCount == next.handLandmarkCount) {
        for (int i = 0; i < next.handLandmarkCount; ++i) {
            result.handLandmarks[static_cast<size_t>(i)].x = blend(previous.handLandmarks[static_cast<size_t>(i)].x, next.handLandmarks[static_cast<size_t>(i)].x);
            result.handLandmarks[static_cast<size_t>(i)].y = blend(previous.handLandmarks[static_cast<size_t>(i)].y, next.handLandmarks[static_cast<size_t>(i)].y);
            result.handLandmarks[static_cast<size_t>(i)].z = blend(previous.handLandmarks[static_cast<size_t>(i)].z, next.handLandmarks[static_cast<size_t>(i)].z);
        }
    }
    return result;
}

#if defined(VAVATAR_ENABLE_OPENCV)
// Measure the distance between two 2D landmark points.
double distance2D(const cv::Point2f& a, const cv::Point2f& b)
{
    const double dx = a.x - b.x;
    const double dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

// Estimate eye openness from six eye landmarks.
double eyeAspectRatio(const std::vector<cv::Point2f>& eye)
{
    const double a = distance2D(eye[1], eye[5]);
    const double b = distance2D(eye[2], eye[4]);
    const double c = distance2D(eye[0], eye[3]);
    return c <= 0.0 ? 0.0 : (a + b) / (2.0 * c);
}

// Estimate mouth openness from inner-mouth landmarks.
double mouthAspectRatio(const std::vector<cv::Point2f>& mouth)
{
    const double a = distance2D(mouth[2], mouth[6]);
    const double b = distance2D(mouth[3], mouth[5]);
    const double c = distance2D(mouth[0], mouth[4]);
    return c <= 0.0 ? 0.0 : (a + b) / (2.0 * c);
}

// Convert EAR to a stable continuous 0..1 eye-open signal.
float continuousEyeOpen(double ear, float blinkThreshold)
{
    const float openReference = std::max(blinkThreshold + 0.04f, 0.32f);
    return std::clamp(
        (static_cast<float>(ear) - blinkThreshold) /
            std::max(0.01f, openReference - blinkThreshold),
        0.0f, 1.0f);
}

// Estimate iris direction from the centroid of dark pixels inside an eye polygon.
cv::Point2f estimateEyeGaze(
    const cv::Mat& gray,
    const std::vector<cv::Point2f>& eye)
{
    cv::Rect bounds = cv::boundingRect(eye);
    bounds &= cv::Rect(0, 0, gray.cols, gray.rows);
    if (bounds.width < 3 || bounds.height < 2) return {};

    cv::Mat eyeImage = gray(bounds);
    cv::Mat dark;
    cv::threshold(eyeImage, dark, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
    std::vector<cv::Point> polygon;
    polygon.reserve(eye.size());
    for (const cv::Point2f& point : eye) {
        polygon.emplace_back(
            static_cast<int>(point.x) - bounds.x,
            static_cast<int>(point.y) - bounds.y);
    }
    cv::Mat mask = cv::Mat::zeros(bounds.size(), CV_8U);
    cv::fillConvexPoly(mask, polygon, cv::Scalar(255));
    cv::bitwise_and(dark, mask, dark);
    const cv::Moments moments = cv::moments(dark, true);
    if (moments.m00 <= 0.0) return {};

    const float x = static_cast<float>(moments.m10 / moments.m00);
    const float y = static_cast<float>(moments.m01 / moments.m00);
    return cv::Point2f(
        std::clamp((x / std::max(1.0f, static_cast<float>(bounds.width)) - 0.5f) * 2.0f, -1.0f, 1.0f),
        std::clamp((y / std::max(1.0f, static_cast<float>(bounds.height)) - 0.5f) * 2.0f, -1.0f, 1.0f));
}

float eyebrowRaise(
    const std::vector<cv::Point2f>& landmarks,
    int browBegin,
    int browEnd,
    int eyeBegin,
    int eyeEnd,
    double faceScale)
{
    float browY = 0.0f;
    float eyeY = 0.0f;
    for (int i = browBegin; i <= browEnd; ++i) browY += landmarks[static_cast<size_t>(i)].y;
    for (int i = eyeBegin; i <= eyeEnd; ++i) eyeY += landmarks[static_cast<size_t>(i)].y;
    browY /= static_cast<float>(browEnd - browBegin + 1);
    eyeY /= static_cast<float>(eyeEnd - eyeBegin + 1);
    const float normalized = static_cast<float>((eyeY - browY) / std::max(1.0, faceScale));
    return std::clamp((normalized - 0.10f) / 0.08f, -1.0f, 1.0f);
}

// Produce low-confidence hand contour landmarks when no dedicated hand model is
// configured. These preserve two-hand motion and fingertip-like hull samples.
void estimateHandContours(
    const cv::Mat& frame,
    const cv::Rect& face,
    FaceTrackingState& state)
{
    cv::Mat ycrcb;
    cv::cvtColor(frame, ycrcb, cv::COLOR_BGR2YCrCb);
    cv::Mat skin;
    cv::inRange(ycrcb, cv::Scalar(0, 133, 77), cv::Scalar(255, 173, 127), skin);
    cv::morphologyEx(skin, skin, cv::MORPH_OPEN, cv::Mat(), cv::Point(-1, -1), 1);
    cv::morphologyEx(skin, skin, cv::MORPH_CLOSE, cv::Mat(), cv::Point(-1, -1), 2);

    cv::Rect excluded = face;
    excluded.x = std::max(0, excluded.x - excluded.width / 3);
    excluded.y = std::max(0, excluded.y - excluded.height / 4);
    excluded.width = std::min(frame.cols - excluded.x, excluded.width + excluded.width * 2 / 3);
    excluded.height = std::min(frame.rows - excluded.y, excluded.height + excluded.height / 2);
    skin(excluded).setTo(0);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(skin, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::sort(contours.begin(), contours.end(), [](const auto& a, const auto& b) {
        return cv::contourArea(a) > cv::contourArea(b);
    });

    const double minimumArea = static_cast<double>(frame.cols * frame.rows) * 0.004;
    int handCount = 0;
    int outputIndex = 0;
    for (const auto& contour : contours) {
        if (handCount >= 2 || cv::contourArea(contour) < minimumArea) break;
        std::vector<cv::Point> hull;
        cv::convexHull(contour, hull);
        if (hull.size() < 5) continue;

        const cv::Moments moments = cv::moments(contour);
        const cv::Point2f center(
            moments.m00 > 0.0 ? static_cast<float>(moments.m10 / moments.m00) : static_cast<float>(hull[0].x),
            moments.m00 > 0.0 ? static_cast<float>(moments.m01 / moments.m00) : static_cast<float>(hull[0].y));
        state.handLandmarks[static_cast<size_t>(outputIndex++)] = {
            center.x / frame.cols, center.y / frame.rows, 0.0f, 0.35f
        };
        for (int landmark = 1; landmark < 21; ++landmark) {
            const size_t hullIndex =
                static_cast<size_t>(landmark - 1) * hull.size() / 20;
            const cv::Point& point = hull[std::min(hullIndex, hull.size() - 1)];
            state.handLandmarks[static_cast<size_t>(outputIndex++)] = {
                static_cast<float>(point.x) / frame.cols,
                static_cast<float>(point.y) / frame.rows,
                -distance2D(cv::Point2f(static_cast<float>(point.x), static_cast<float>(point.y)), center) / std::max(frame.cols, frame.rows),
                0.25f
            };
        }
        ++handCount;
    }
    state.detectedHands = handCount;
    state.handLandmarkCount = outputIndex;
}
// Solve a simple 3D head pose from standard 68-point face landmarks.
bool estimateHeadPose(const std::vector<cv::Point2f>& landmarks, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs, cv::Vec3d& rvec, cv::Vec3d& tvec)
{
    if (landmarks.size() < 68) {
        return false;
    }

    std::vector<cv::Point3d> modelPoints = {
        {0.0, 0.0, 0.0},
        {0.0, -330.0, -65.0},
        {-225.0, 170.0, -135.0},
        {225.0, 170.0, -135.0},
        {-150.0, -150.0, -125.0},
        {150.0, -150.0, -125.0}
    };

    std::vector<cv::Point2d> imagePoints = {
        landmarks[30],
        landmarks[8],
        landmarks[36],
        landmarks[45],
        landmarks[48],
        landmarks[54]
    };

    return cv::solvePnP(modelPoints, imagePoints, cameraMatrix, distCoeffs, rvec, tvec, false, cv::SOLVEPNP_ITERATIVE);
}

// Convert OpenCV rotation vectors into pitch/yaw/roll degrees.
cv::Vec3d rotationVectorToEuler(const cv::Vec3d& rvec)
{
    cv::Mat rotation;
    cv::Rodrigues(rvec, rotation);

    const double sy = std::sqrt(
        rotation.at<double>(0, 0) * rotation.at<double>(0, 0) +
        rotation.at<double>(1, 0) * rotation.at<double>(1, 0));

    const bool singular = sy < 1e-6;
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    if (!singular) {
        x = std::atan2(rotation.at<double>(2, 1), rotation.at<double>(2, 2));
        y = std::atan2(-rotation.at<double>(2, 0), sy);
        z = std::atan2(rotation.at<double>(1, 0), rotation.at<double>(0, 0));
    } else {
        x = std::atan2(-rotation.at<double>(1, 2), rotation.at<double>(1, 1));
        y = std::atan2(-rotation.at<double>(2, 0), sy);
    }

    constexpr double radiansToDegrees = 180.0 / 3.14159265358979323846;
    return cv::Vec3d(x * radiansToDegrees, y * radiansToDegrees, z * radiansToDegrees);
}
#endif
}

CameraTrackingOpenCV::~CameraTrackingOpenCV()
{
    stop();
}

bool CameraTrackingOpenCV::start(int cameraIndex, const std::string& faceCascadePath, const std::string& lbfModelPath)
{
    if (m_running.load() || m_starting.load()) {
        return true;
    }

#if !defined(VAVATAR_ENABLE_OPENCV)
    setStatus("OpenCV disabled at build time");
    (void)cameraIndex;
    (void)faceCascadePath;
    (void)lbfModelPath;
    return false;
#else
    if (m_thread.joinable()) {
        m_thread.join();
    }

    m_stopRequested.store(false);
    m_starting.store(true);
    m_thread = std::thread(&CameraTrackingOpenCV::captureLoop, this, cameraIndex, faceCascadePath, lbfModelPath);
    return true;
#endif
}

void CameraTrackingOpenCV::stop()
{
    m_stopRequested.store(true);

    if (m_thread.joinable()) {
        m_thread.join();
    }

    m_running.store(false);
    m_starting.store(false);
}

CameraTrackingCalibration CameraTrackingOpenCV::getCalibration() const
{
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_calibration;
}

void CameraTrackingOpenCV::setCalibration(const CameraTrackingCalibration& calibration)
{
    CameraTrackingCalibration next = calibration;
    next.headYawScale = clampCalibrationValue(next.headYawScale, 0.1f, 3.0f);
    next.headPitchScale = clampCalibrationValue(next.headPitchScale, 0.1f, 3.0f);
    next.headRollScale = clampCalibrationValue(next.headRollScale, 0.1f, 3.0f);
    next.blinkThreshold = clampCalibrationValue(next.blinkThreshold, 0.05f, 0.45f);
    next.mouthClosed = clampCalibrationValue(next.mouthClosed, 0.0f, 0.6f);
    next.mouthOpenRange = clampCalibrationValue(next.mouthOpenRange, 0.05f, 1.2f);
    next.smileThreshold = clampCalibrationValue(next.smileThreshold, 1.0f, 3.5f);
    next.smoothing = clampCalibrationValue(next.smoothing, 0.02f, 1.0f);

    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_calibration = next;
}

std::vector<CameraDeviceInfo> CameraTrackingOpenCV::listAvailableDevices(int maxDevices)
{
    std::vector<CameraDeviceInfo> devices;
    maxDevices = std::clamp(maxDevices, 1, 16);

#if defined(VAVATAR_ENABLE_OPENCV)
    for (int index = 0; index < maxDevices; ++index) {
        cv::VideoCapture capture(index);
        if (capture.isOpened()) {
            char name[64] = {};
            std::snprintf(name, sizeof(name), "Camera %d", index);
            devices.push_back({ index, name });
            capture.release();
        }
    }
#else
    (void)maxDevices;
#endif

    if (devices.empty()) {
        devices.push_back({ 0, "Camera 0" });
    }

    return devices;
}

bool CameraTrackingOpenCV::isAvailable() const
{
#if defined(VAVATAR_ENABLE_OPENCV)
    return true;
#else
    return false;
#endif
}

std::string CameraTrackingOpenCV::getStatus() const
{
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_status;
}

FaceTrackingState CameraTrackingOpenCV::getState() const
{
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_state;
}

void CameraTrackingOpenCV::setState(const FaceTrackingState& state)
{
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_state = state;
}

void CameraTrackingOpenCV::setStatus(const std::string& status)
{
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_status = status;
}

void CameraTrackingOpenCV::captureLoop(int cameraIndex, std::string faceCascadePath, std::string lbfModelPath)
{
#if !defined(VAVATAR_ENABLE_OPENCV)
    (void)cameraIndex;
    (void)faceCascadePath;
    (void)lbfModelPath;
#else
    cv::CascadeClassifier faceCascade;
    if (!faceCascade.load(faceCascadePath)) {
        setStatus("Could not load face cascade");
        m_starting.store(false);
        return;
    }

    cv::Ptr<cv::face::Facemark> facemark = cv::face::FacemarkLBF::create();
    try {
        facemark->loadModel(lbfModelPath);
    } catch (const cv::Exception&) {
        setStatus("Could not load LBF landmark model");
        m_starting.store(false);
        return;
    }

    cv::VideoCapture capture(cameraIndex);
    if (!capture.isOpened()) {
        setStatus("Could not open camera");
        m_starting.store(false);
        return;
    }

    const double frameWidth = std::max(1.0, capture.get(cv::CAP_PROP_FRAME_WIDTH));
    const double frameHeight = std::max(1.0, capture.get(cv::CAP_PROP_FRAME_HEIGHT));
    const cv::Point2d center(frameWidth * 0.5, frameHeight * 0.5);
    const cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << frameWidth, 0, center.x, 0, frameWidth, center.y, 0, 0, 1);
    const cv::Mat distCoeffs = cv::Mat::zeros(4, 1, CV_64F);

    m_running.store(true);
    m_starting.store(false);
    setStatus("OpenCV camera tracking running");

    int emptyFrameCount = 0;
    while (!m_stopRequested.load()) {
        cv::Mat frame;
        capture >> frame;
        if (frame.empty()) {
            setState(FaceTrackingState{});
            ++emptyFrameCount;
            if (emptyFrameCount > 60) {
                setStatus("Camera disconnected or no frames received");
                break;
            }
            continue;
        }
        emptyFrameCount = 0;

        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        std::vector<cv::Rect> faces;
        faceCascade.detectMultiScale(gray, faces, 1.1, 3, 0, cv::Size(100, 100));

        FaceTrackingState nextState;
        if (!faces.empty()) {
            const auto largest = std::max_element(faces.begin(), faces.end(), [](const cv::Rect& a, const cv::Rect& b) { return a.area() < b.area(); });
            const cv::Rect trackedFace = *largest;
            faces.assign(1, trackedFace);
            std::vector<std::vector<cv::Point2f>> landmarks;
            if (facemark->fit(frame, faces, landmarks) && !landmarks.empty() && landmarks[0].size() >= 68) {
                const std::vector<cv::Point2f>& lm = landmarks[0];
                nextState.foundPerson = true;
                const CameraTrackingCalibration calibration = getCalibration();

                cv::Vec3d rvec;
                cv::Vec3d tvec;
                if (estimateHeadPose(lm, cameraMatrix, distCoeffs, rvec, tvec)) {
                    const cv::Vec3d euler = rotationVectorToEuler(rvec);
                    nextState.headPitch = (static_cast<float>(euler[0]) - calibration.headPitchOffset) * calibration.headPitchScale;
                    nextState.headYaw = (static_cast<float>(euler[1]) - calibration.headYawOffset) * calibration.headYawScale;
                    nextState.headRoll = (static_cast<float>(euler[2]) - calibration.headRollOffset) * calibration.headRollScale;
                }

                const std::vector<cv::Point2f> leftEye(lm.begin() + 36, lm.begin() + 42);
                const std::vector<cv::Point2f> rightEye(lm.begin() + 42, lm.begin() + 48);
                const double leftEar = eyeAspectRatio(leftEye);
                const double rightEar = eyeAspectRatio(rightEye);
                nextState.leftEyeOpen = continuousEyeOpen(leftEar, calibration.blinkThreshold);
                nextState.rightEyeOpen = continuousEyeOpen(rightEar, calibration.blinkThreshold);
                nextState.leftBlink = nextState.leftEyeOpen <= 0.05f;
                nextState.rightBlink = nextState.rightEyeOpen <= 0.05f;
                const cv::Point2f leftGaze = estimateEyeGaze(gray, leftEye);
                const cv::Point2f rightGaze = estimateEyeGaze(gray, rightEye);
                nextState.eyeGazeX = std::clamp((leftGaze.x + rightGaze.x) * 0.5f, -1.0f, 1.0f);
                nextState.eyeGazeY = std::clamp((leftGaze.y + rightGaze.y) * 0.5f, -1.0f, 1.0f);
                const double faceScale = std::max(distance2D(lm[0], lm[16]), 1.0);
                nextState.browLeft = eyebrowRaise(lm, 17, 21, 36, 41, faceScale);
                nextState.browRight = eyebrowRaise(lm, 22, 26, 42, 47, faceScale);

                const std::vector<cv::Point2f> mouth(lm.begin() + 60, lm.begin() + 68);
                const double mar = mouthAspectRatio(mouth);
                nextState.mouthOpen = std::clamp(static_cast<float>((mar - calibration.mouthClosed) / calibration.mouthOpenRange), 0.0f, 1.0f);
                const float smileRatio = static_cast<float>(
                    distance2D(lm[48], lm[54]) /
                    std::max(distance2D(lm[51], lm[57]), 1.0));
                nextState.smile = std::clamp(
                    (smileRatio - calibration.smileThreshold * 0.65f) /
                        std::max(0.01f, calibration.smileThreshold * 0.35f),
                    0.0f, 1.0f);
                nextState.smiling = nextState.smile >= 0.55f;
                const float mouthWidth = static_cast<float>(distance2D(lm[48], lm[54]) / faceScale);
                nextState.mouthForm = std::clamp(
                    (mouthWidth - 0.34f) * 7.0f - nextState.mouthOpen * 0.25f,
                    -1.0f, 1.0f);
                nextState.shoulderYaw = std::clamp(nextState.headYaw * 0.35f, -30.0f, 30.0f);
                nextState.shoulderPitch = std::clamp(nextState.headPitch * 0.25f, -30.0f, 30.0f);
                estimateHandContours(frame, trackedFace, nextState);
            }
        }

        const CameraTrackingCalibration calibration = getCalibration();
        setState(blendState(getState(), nextState, calibration.smoothing));
    }

    capture.release();
    if (!m_stopRequested.load() && getStatus() != "Camera disconnected or no frames received") {
        setStatus("OpenCV camera tracking stopped");
    } else if (m_stopRequested.load()) {
        setStatus("OpenCV camera tracking stopped");
    }
    m_running.store(false);
    m_starting.store(false);
#endif
}
