// vtuber_face_tracker_opencv.cpp
// Pure OpenCV-based VTuber face and hand tracker (no dlib, no DNN)
// Tracks: head pose, facial landmarks, eye blink, mouth movement, expression (smile), and simple hand detection.
// Uses OpenCV's built-in Haar cascades and FacemarkLBF.
//
// Requirements:
//  - OpenCV 4.x built with opencv_contrib (for Facemark)
//  - haarcascade_frontalface_default.xml
//  - lbfmodel.yaml (OpenCV's pre-trained landmark model)
//
// Compile:
// g++ -std=c++17 vtuber_face_tracker_opencv.cpp -o vtuber_face_tracker `pkg-config --cflags --libs opencv4`
//
// Run:
// ./vtuber_face_tracker --face haarcascade_frontalface_default.xml --lbf lbfmodel.yaml

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;
using namespace cv;

// --- Utility functions ------------------------------------------------------

double euclidean(const Point2f &a, const Point2f &b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

double eye_aspect_ratio(const vector<Point2f>& eye) {
    double A = euclidean(eye[1], eye[5]);
    double B = euclidean(eye[2], eye[4]);
    double C = euclidean(eye[0], eye[3]);
    if (C == 0) return 0;
    return (A + B) / (2.0 * C);
}

double mouth_aspect_ratio(const vector<Point2f>& mouth) {
    double A = euclidean(mouth[2], mouth[10]);
    double B = euclidean(mouth[3], mouth[9]);
    double C = euclidean(mouth[0], mouth[6]);
    if (C == 0) return 0;
    return (A + B) / (2.0 * C);
}

Mat skin_mask(const Mat& frame) {
    Mat hsv, mask;
    cvtColor(frame, hsv, COLOR_BGR2HSV);
    inRange(hsv, Scalar(0, 30, 60), Scalar(20, 150, 255), mask);
    erode(mask, mask, Mat(), Point(-1,-1), 1);
    dilate(mask, mask, Mat(), Point(-1,-1), 2);
    GaussianBlur(mask, mask, Size(7,7), 0);
    return mask;
}

bool estimate_head_pose(const vector<Point2f>& lm, const Mat& camera_matrix, const Mat& dist_coeffs, Vec3d& rvec, Vec3d& tvec) {
    if (lm.size() < 68) return false;
    vector<Point3d> model_points = {
        {0.0, 0.0, 0.0}, {0.0, -330.0, -65.0}, {-225.0, 170.0, -135.0},
        {225.0, 170.0, -135.0}, {-150.0, -150.0, -125.0}, {150.0, -150.0, -125.0}
    };
    vector<Point2d> img_points = {
        lm[30], lm[8], lm[36], lm[45], lm[48], lm[54]
    };
    return solvePnP(model_points, img_points, camera_matrix, dist_coeffs, rvec, tvec, false, SOLVEPNP_ITERATIVE);
}

Vec3d rotationVectorToEuler(const Vec3d& rvec) {
    Mat R;
    Rodrigues(rvec, R);
    double sy = sqrt(R.at<double>(0,0)*R.at<double>(0,0) + R.at<double>(1,0)*R.at<double>(1,0));
    bool singular = sy < 1e-6;
    double x, y, z;
    if (!singular) {
        x = atan2(R.at<double>(2,1), R.at<double>(2,2));
        y = atan2(-R.at<double>(2,0), sy);
        z = atan2(R.at<double>(1,0), R.at<double>(0,0));
    } else {
        x = atan2(-R.at<double>(1,2), R.at<double>(1,1));
        y = atan2(-R.at<double>(2,0), sy);
        z = 0;
    }
    return Vec3d(x*180.0/M_PI, y*180.0/M_PI, z*180.0/M_PI);
}

// --- Main ------------------------------------------------------------------
int main(int argc, char** argv) {
    string faceCascadePath, lbfModelPath;
    for (int i=1; i<argc; i++) {
        string arg(argv[i]);
        if (arg == "--face" && i+1<argc) faceCascadePath = argv[++i];
        else if (arg == "--lbf" && i+1<argc) lbfModelPath = argv[++i];
    }

    if (faceCascadePath.empty() || lbfModelPath.empty()) {
        cerr << "Usage: ./vtuber_face_tracker --face haarcascade_frontalface_default.xml --lbf lbfmodel.yaml\n";
        return 1;
    }

    CascadeClassifier faceCascade;
    if (!faceCascade.load(faceCascadePath)) {
        cerr << "Could not load face cascade." << endl;
        return 1;
    }

    Ptr<face::Facemark> facemark = face::FacemarkLBF::create();
    facemark->loadModel(lbfModelPath);

    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "Cannot open camera." << endl;
        return 1;
    }

    double focal_length = cap.get(CAP_PROP_FRAME_WIDTH);
    Point2d center(cap.get(CAP_PROP_FRAME_WIDTH)/2, cap.get(CAP_PROP_FRAME_HEIGHT)/2);
    Mat camera_matrix = (Mat_<double>(3,3) << focal_length, 0, center.x, 0, focal_length, center.y, 0, 0, 1);
    Mat dist_coeffs = Mat::zeros(4,1,CV_64F);

    const double EAR_THRESHOLD = 0.2;
    const double MAR_THRESHOLD = 0.5;

    while (true) {
        Mat frame, gray;
        cap >> frame;
        if (frame.empty()) break;

        cvtColor(frame, gray, COLOR_BGR2GRAY);

        vector<Rect> faces;
        faceCascade.detectMultiScale(gray, faces, 1.1, 3, 0, Size(100, 100));

        vector<vector<Point2f>> landmarks;
        if (faces.size() > 0) {
            bool success = facemark->fit(frame, faces, landmarks);
            if (success) {
                for (size_t i=0; i<landmarks.size(); i++) {
                    auto& lm = landmarks[i];
                    for (auto &p : lm) circle(frame, p, 2, Scalar(0,255,0), -1);

                    Vec3d rvec, tvec;
                    if (estimate_head_pose(lm, camera_matrix, dist_coeffs, rvec, tvec)) {
                        Vec3d euler = rotationVectorToEuler(rvec);
                        putText(frame, format("Yaw:%.0f Pitch:%.0f Roll:%.0f", euler[1], euler[0], euler[2]), Point(10,30), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255,0,0), 2);
                    }

                    vector<Point2f> leftEye(lm.begin()+36, lm.begin()+42);
                    vector<Point2f> rightEye(lm.begin()+42, lm.begin()+48);
                    double ear = (eye_aspect_ratio(leftEye) + eye_aspect_ratio(rightEye)) / 2.0;
                    putText(frame, format("EAR:%.2f", ear), Point(10,60), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,255,255), 2);

                    vector<Point2f> mouth(lm.begin()+60, lm.begin()+68);
                    double mar = mouth_aspect_ratio(mouth);
                    putText(frame, format("MAR:%.2f", mar), Point(10,90), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255,0,255), 2);

                    bool smiling = (euclidean(lm[48], lm[54]) / euclidean(lm[51], lm[57])) > 1.8;
                    putText(frame, string("Smile:") + (smiling?"YES":"NO"), Point(10,120), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,200,0), 2);
                }
            }
        }

        Mat mask = skin_mask(frame);
        vector<vector<Point>> contours;
        findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        int hands = 0;
        for (auto &cnt : contours) {
            double area = contourArea(cnt);
            if (area < 1500) continue;
            Rect r = boundingRect(cnt);
            rectangle(frame, r, Scalar(0,128,255), 2);
            hands++;
        }
        putText(frame, format("Hands:%d", hands), Point(10,150), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,128,255), 2);

        imshow("VTuber Tracker (OpenCV Only)", frame);
        if (waitKey(1) == 27) break;
    }

    return 0;
}
