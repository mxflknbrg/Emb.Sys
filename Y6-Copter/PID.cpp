#include "PID.hpp"

namespace embSys {
    PID::PID() :
        last_error_(0.f),
        last_output_(0.f),
        integral_(0.f),
        error_threshold_(0.f)
        {}

	void PID::setKp(const float &Kp) {
		this->Kp_ = Kp;
	}

	void PID::setKi(const float &Ki) {
		this->Ki_ = Ki*interval_;
	}

	void PID::setKd(const float &Kd) {
		this->Kd_ = Kd/interval_;
	}

	void PID::setWeights(const float &Kp, const float &Ki, const float &Kd) {
		setKp(Kp);
		setKi(Ki);
		setKd(Kd);
    }

	void PID::setRefreshInterval(const float &refresh_interval) {
		interval_ = refresh_interval;
	}

	void PID::setRefreshRate(const float &refresh_rate) {
		interval_ = 1.f/refresh_rate;
	}

	void PID::setErrorThreshold(const float &error_threshold) {
		this->error_threshold_ = error_threshold;
	}

	void PID::setOutputLowerLimit(const float &output_lower_limit) {
		this->output_lower_limit_ = output_lower_limit;
	}

	void PID::setOutputUpperLimit(const float &output_upper_limit) {
		this->output_upper_limit_ = output_upper_limit;
	}

	void PID::setDesiredPoint(const float &desired_point) {
		set_point_ = desired_point;
	}

	float PID::refresh(const float &feedback_input) {
		error_ = set_point_-feedback_input;
		if (error_ >= error_threshold_ or error_ <= -error_threshold_) {
			last_output_ = Kp_*error_ + Ki_*integral_ + Kd_*(error_-last_error_);
			if (last_output_ > output_upper_limit_) {
				last_output_ = output_upper_limit_;
				if (integral_/error_ < 0.f) {
					integral_ += (error_+last_error_)/2.f;
					last_error_ = error_;
				}
				return output_upper_limit_;
			}
			if (last_output_ < output_lower_limit_) {
				if (integral_/error_ < 0.f) {
					integral_ += (error_+last_error_)/2.f;
					last_error_ = error_;
				}
				last_output_ = output_lower_limit_;
				return output_lower_limit_;
			}
			integral_ += (error_+last_error_)/2.f;
			last_error_ = error_;
		}
		return last_output_;
	}
};
