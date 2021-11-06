#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <iomanip>

#include "queue.h"

using namespace single_channel_queue_simulation;


int Customer::customer_id_ = 0;

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


void EventModel::SetCumProb() {
  float cum;

  for(int i = 0; i < n_options_; i++) {
    cum = 0;
    for(int j = 0; j <= i; j++) {
      cum += probs_[j];
    }
    cum_prob_[i] = cum;
  }
}

void EventModel::SetCumSum() {
  for(int i = 0; i < n_options_; i++) {
    cum_sum_[i] = std::pow(10, n_decimal_) * cum_prob_[i];
  }
}

EventModel::EventModel(int n_decimal, std::vector<int> options, std::vector<float> probs) {
  options_ = options;
  probs_ = probs;
  n_decimal_ = n_decimal;
  n_options_ = options_.size();
  cum_prob_.resize(n_options_);
  cum_sum_.resize(n_options_);
  SetCumProb();
  SetCumSum();
}

int EventModel::GetEvent() {
  int r = std::rand();
  int range = std::pow(10, n_decimal_);
  r = r % range;

  if(r == 0) return options_.back();

  for(int i = 0; i < n_options_; i++) {
    if(r <= cum_sum_[i]) return options_[i];
  }

  throw "GET EVENT FAILED";
}

Simulator::Simulator(int n_customer, EventModel& arrival_model, EventModel& service_model)
  : arrival_model_(arrival_model), service_model_(service_model) {
  n_customer_ = n_customer;
  arrival_model_ = arrival_model;
  service_model_ = service_model;
  total_it_ = total_its_ = total_st_ = total_tcss_ = total_wtq_ = 0;
}

Customer::Customer(int service_time) {
  customer_id_++;
  inter_arrival_time_ = arrival_time_ = time_service_begins_ =
    waiting_time_in_queue_ = idle_time_of_server_ = 0;
  service_time_ = time_service_ends_ = time_customer_spends_in_system_ = service_time;
}

Customer::Customer(int arrival_interval, int service_time, Customer prev_customer) {
  customer_id_++;

  inter_arrival_time_ = arrival_interval;
  arrival_time_ = inter_arrival_time_ + prev_customer.arrival_time_;
  service_time_ = service_time;
  time_service_begins_ = std::max(arrival_time_, prev_customer.time_service_ends_);
  waiting_time_in_queue_ = (arrival_time_ >= prev_customer.time_service_ends_)
    ? 0
    : prev_customer.time_service_ends_ - arrival_time_;
  time_service_ends_ = time_service_begins_ + service_time_;
  time_customer_spends_in_system_ = service_time_ + waiting_time_in_queue_;
  idle_time_of_server_ = (arrival_time_ >= prev_customer.time_service_ends_)
    ? arrival_time_ - prev_customer.time_service_ends_
    : 0;
}

void Simulator::LogCustomer(Customer& c) {
  std::stringstream details;
  details << c.customer_id_
          << std::setw(6) << c.inter_arrival_time_
          << std::setw(6) << c.arrival_time_
          << std::setw(6) << c.service_time_
          << std::setw(6) << c.time_service_begins_
          << std::setw(6) << c.waiting_time_in_queue_
          << std::setw(6) << c.time_service_ends_
          << std::setw(6) << c.time_customer_spends_in_system_
          << std::setw(6) << c.idle_time_of_server_ << std::endl
    ;

  std::string details_string = details.str();
  logger_.Log(details_string);
}

void Simulator::InitializeLogTable() {
  std::stringstream heads;
  heads << "C"
        << std::setw(6) << "IT"
        << std::setw(6) << "AT"
        << std::setw(6) << "ST"
        << std::setw(6) << "TSB"
        << std::setw(6) << "WTQ"
        << std::setw(6) << "TSE"
        << std::setw(6) << "TCSS"
        << std::setw(6) << "ITS" << std::endl
    ;

  std::string heads_string = heads.str();
  logger_.Log(heads_string);
}


void Simulator::UpdateHistory(Customer& c) {
  total_it_ += c.inter_arrival_time_;
  total_its_ += c.idle_time_of_server_;
  total_st_ += c.service_time_;
  total_tcss_ += c.time_customer_spends_in_system_;
  total_wtq_ += c.waiting_time_in_queue_;
}

void Simulator::LogMetrics() {
  std::stringstream metrics;
  metrics << ""
          << std::setw(10) << total_it_
          << std::setw(6) << ""
          << std::setw(6) << total_st_
          << std::setw(6) << ""
          << std::setw(6) << total_wtq_
          << std::setw(6) << ""
          << std::setw(6) << total_tcss_
          << std::setw(6) << total_its_ << std::endl
    ;

  std::string metrics_string = metrics.str();
  logger_.Log(metrics_string);
}

void Simulator::RunSimulation() {
  if(n_customer_ == 0) return;

  InitializeLogTable();

  Customer customer(service_model_.GetEvent());
  LogCustomer(customer);
  UpdateHistory(customer);

  for(int i = 0; i < n_customer_ - 1; i++) {
    int service_time, arrival_interval;
    service_time = service_model_.GetEvent();
    arrival_interval = arrival_model_.GetEvent();
    customer = Customer(arrival_interval, service_time, customer);
    LogCustomer(customer);
    UpdateHistory(customer);
  }

  LogMetrics();
}

int main() {
  std::vector<int> arrival_intervals {1, 2, 3, 4, 5, 6, 7, 8};
  std::vector<float> arrival_probs (8, 0.125);
  std::vector<int> service_times {1, 2, 3, 4, 5, 6};
  std::vector<float> service_probs {0.1, 0.2, 0.3, 0.25, 0.1, 0.05};
  int n_customer = 100;

  EventModel arrival_model(3, arrival_intervals, arrival_probs);
  EventModel service_model(2, service_times, service_probs);
  Simulator simulator(n_customer, arrival_model, service_model);

  simulator.RunSimulation();

  return 0;
}
