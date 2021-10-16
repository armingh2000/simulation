#include <iostream>
#include <ostream>
#include <vector>
#include <limits>
#include <sstream>

#include "queue.h"

using namespace queue_simulation;

const float kInf = std::numeric_limits<float>::max();

Logger::Logger() {
    log_file_ = std::ofstream("log.txt");
}

Logger::~Logger() {
    log_file_.close();
}

void Logger::Log(std::string log) {
    log_file_ << log;
    log_file_ << std::endl;
}

std::string Simulator::GetStringVector(std::vector<float> vec) {
    std::stringstream ss;
    for(size_t i = 0; i < vec.size(); ++i)
    {
        if(i != 0)
            ss << ",";
        ss << vec[i];
    }
    std::string s = ss.str();

    return s;
}

void Simulator::Log() {
    std::stringstream system_state;
    system_state << "NEW SYSTEM STATE" << std::endl
                 << "clock: " << clock_ << std::endl
                 << "event list: " << GetStringVector(event_list_) << std::endl
                 << "server status: " << server_status_ << std::endl
                 << "number in queue: " << number_in_queue_ << std::endl
                 << "times of arrival: " << GetStringVector(arrival_times_) << std::endl
                 << "time of last event: " << last_event_time_ << std::endl
                 << "number serviced: " << number_serviced_ << std::endl
                 << "total delay: " << total_delay_ << std::endl
                 << "area under q(t): " << qt_area_ << std::endl
                 << "area under b(t): " << bt_area_ << std::endl
        ;

    std::string system_state_string = system_state.str();
    logger_.Log(system_state_string);
}

Simulator::Simulator(std::vector<float> arrival_intervals, std::vector<float> service_times, const unsigned kNumberServiced)
    : kLimit(kNumberServiced) {
    arrival_intervals_ = arrival_intervals;
    service_times_ = service_times;
    event_list_.resize(2, kInf); // arrival = 0, departure = 1
    clock_ = last_event_time_ = bt_area_ = number_in_queue_ = qt_area_ = number_serviced_ = total_delay_ = 0;
    server_status_ = false;
    wq_ = lq_ = p_ = l_ = w_ = e_s_ = 0;
}

int Simulator::GetCurrentEventType() {
    float front = event_list_[0];
    float back = event_list_[1];

    if(front < back) return 0;

    // in case of equal timing return departure event
    return 1;
}

void Simulator::SetArrivalEvent() {
    float arrival_interval = arrival_intervals_.back();
    arrival_intervals_.pop_back();
    event_list_[0] = arrival_interval + clock_;
}

void Simulator::SetDepartureEvent() {
    float service_time = service_times_.back();
    service_times_.pop_back();
    event_list_[1] = service_time + clock_;

    e_s_ += service_time;
}

void Simulator::UpdateBTArea() {
    bt_area_ += clock_ - last_event_time_;
}

void Simulator::UpdateQTArea() {
    int number_in_queue = number_in_queue_;
    float interval = clock_ - last_event_time_;
    qt_area_ += number_in_queue * interval;
}

void Simulator::UpdateArrivalTimes() {
    float arrival_time = event_list_[0];
    event_list_[0] = kInf;
    arrival_times_.insert(arrival_times_.begin(), arrival_time);

    SetArrivalEvent();
}

void Simulator::UpdateTotalDelay() {
    float arrival_time = arrival_times_.back();
    arrival_times_.pop_back();
    float delay = clock_ - arrival_time;
    total_delay_ += delay;
}

void Simulator::RunSimulation() {
    if(kLimit == 0) return;

    // add first arrival
    float arrival_interval = arrival_intervals_.back();
    arrival_intervals_.pop_back();
    event_list_[0] = arrival_interval;
    Log();

    float event_time;
    int event_type;

    // simulation
    while(number_serviced_ < kLimit) {
        event_type = GetCurrentEventType();
        event_time = event_list_[event_type];
        last_event_time_ = clock_;
        clock_ = event_time;

        // arrival
        if(!event_type) {

            // server is idle
            if(!server_status_) {
                server_status_ = true;
                number_serviced_++;

                // update event list
                SetArrivalEvent();
                SetDepartureEvent();
            }

            // server is busy
            else {
                UpdateQTArea();
                UpdateBTArea();
                UpdateArrivalTimes();
            }
        }

        // departure
        else {
            UpdateBTArea();
            UpdateQTArea();
            if(number_in_queue_) {
                SetDepartureEvent();
                number_serviced_++;
                UpdateTotalDelay();
            }
            else {
                server_status_ = false;
                event_list_[1] = kInf;
            }
        }
        number_in_queue_ = arrival_times_.size();

        Log();
    }

    SetMetrics();
    LogMetrics();
}

void Simulator::SetMetrics() {
    wq_ = total_delay_ / kLimit;
    lq_ = qt_area_ / clock_;
    p_ = bt_area_ / clock_;
    l_ = lq_ + p_;
    e_s_ = e_s_ / kLimit;
    w_ = wq_ + e_s_;
}

void Simulator::PrintMetrics(std::string metrics) {
    std::cout << metrics;
}

void Simulator::LogMetrics() {
    std::stringstream metrics;
    metrics << "METRICS" << std::endl
            << "Wq: " << wq_ << std::endl
            << "Lq: " << lq_ << std::endl
            << "p: " << p_ << std::endl
            << "L: " << lq_ + p_ << std::endl
            << "E[s]: " << e_s_ << std::endl
            << "W: " << w_ << std::endl
        ;

    std::string metrics_string = metrics.str();
    logger_.Log(metrics_string);

    PrintMetrics(metrics_string);
}

int main() {
    const unsigned kNumberServiced = 6;
    std::vector<float> arrival_intervals{1.9, 1.4, 0.2, 1.6, 0.2, 1.7, 0.5, 1.2, 0.4};
    std::vector<float> service_times{0.6, 3.7, 1.1, 0.2, 0.7, 2};

    Simulator simulator(arrival_intervals, service_times, kNumberServiced);
    simulator.RunSimulation();

    return 0;
}
