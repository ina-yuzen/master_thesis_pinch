#pragma once

#include <memory>
#include <vector>
#include <opencv2\opencv.hpp>

namespace mobamas {

struct Context;
struct DepthMap;
std::vector<cv::Point> PinchErosion(std::shared_ptr<Context> context, const DepthMap& data);
std::vector<cv::Point> PinchNailColor(std::shared_ptr<Context> context, const DepthMap& data);
cv::Vec3i DetectTouch(const DepthMap& data);
}