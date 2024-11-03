#include <cmath>
#include <memory>

#include "rclcpp/rclcpp.hpp"

class LidarConfig
{
public:
    const char *PARAM_MIN_RANGE = "min_range";
    const char *PARAM_MAX_RANGE = "max_range";
    const char *PARAM_MIN_ANGLE = "min_angle";
    const char *PARAM_MAX_ANGLE = "max_angle";
    const char *PARAM_SAMPLE_COUNT = "sample_count";
    const char *PARAM_SAMPLING_FREQUENCY = "sampling_frequency";

    const double DEFAULT_MIN_RANGE = 0.2;
    const double DEFAULT_MAX_RANGE = 2.0;
    const double DEFAULT_MIN_ANGLE = -M_PI;
    const double DEFAULT_MAX_ANGLE = M_PI;
    const int DEFAULT_SAMPLE_COUNT = 360;
    const double DEFAULT_SAMPLING_FREQUENCY = 10.0;

    const char *DESCRIPTION_MIN_RANGE =
        "minimum range value [m]";
    const char *DESCRIPTION_MAX_RANGE =
        "maximum range value [m]";
    const char *DESCRIPTION_MIN_ANGLE =
        "start angle of the scan [rad]";
    const char *DESCRIPTION_MAX_ANGLE =
        "end angle of the scan [rad]";
    const char *DESCRIPTION_SAMPLE_COUNT =
        "Number of samples per full laser scan";
    const char *DESCRIPTION_SAMPLING_FREQUENCY =
        "Number of full Scans per second.";

public:
    std::pair<double, double> range;
    std::pair<double, double> angle;
    int sample_count;
    double sampling_frequency;

public:
    void declare_parameters(rclcpp::Node *node);
    void update_parameters(rclcpp::Node *node);
    void print_config(rclcpp::Node *node);

    double get_scan_step() const;
    int get_scan_period_ms() const;
    double get_scaled_sample(double scale) const;
};