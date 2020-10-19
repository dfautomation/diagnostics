#include <diagnostic_updater/update_functions.h>

using namespace diagnostic_updater;

void TimeStampStatus::run(DiagnosticStatusWrapper& stat) {
    boost::mutex::scoped_lock lock(lock_);

    using diagnostic_msgs::DiagnosticStatus;

    stat.summary(DiagnosticStatus::OK, "Timestamps are reasonable.");
    if (!deltas_valid_)
    {
      const auto no_data_is_problem = dynamic_cast<SlowTimeStampStatus*>(this) == nullptr;
      const auto status = no_data_is_problem ? DiagnosticStatus::WARN : DiagnosticStatus::OK;
      stat.summary(status, "No data since last update.");
    }
    else
    {
      if (min_delta_ < params_.min_acceptable_)
      {
	stat.summary(DiagnosticStatus::ERROR, "Timestamps too far in future seen.");
	early_count_++;
      }
    
      if (max_delta_ > params_.max_acceptable_)
      {
	stat.summary(DiagnosticStatus::ERROR, "Timestamps too far in past seen.");
	late_count_++;
      }

      if (zero_seen_)
      {
	stat.summary(DiagnosticStatus::ERROR, "Zero timestamp seen.");
	zero_count_++;
      }
    }

    stat.addf("Timestamp delay - Earliest | Latest | Acceptable", "%g | %g | %g-%g", min_delta_, max_delta_, params_.min_acceptable_, params_.max_acceptable_);
    stat.addf("Late | Early | Zero Seen count", "%i | %i | %i", late_count_, early_count_, zero_count_);

    deltas_valid_ = false;
    min_delta_ = 0;
    max_delta_ = 0;
    zero_seen_ = false;
}
