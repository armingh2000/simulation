#ifndef HW2_SINGLE_CHANNEL_QUEUE_H_
#define HW2_SINGLE_CHANNEL_QUEUE_H_

#include <vector>
#include <sstream>
#include <fstream>

namespace single_channel_queue_simulation {

  class Simulator;

  class EventModel {
  public:
    EventModel(int, std::vector<int>, std::vector<float>);

    int GetEvent();

  private:
    int n_decimal_, n_options_;
    std::vector<int> options_, cum_sum_;
    std::vector<float> probs_, cum_prob_;

    void SetCumProb();
    void SetCumSum();
  };

  class Customer {
  public:
    Customer(int);
    Customer(int, int, Customer);

  private:
    static int customer_id_;
    int inter_arrival_time_, arrival_time_,
      service_time_, time_service_begins_, waiting_time_in_queue_,
      time_service_ends_, time_customer_spends_in_system_, idle_time_of_server_;

    friend Simulator;
  };

  class Logger {
  public:
    Logger();
    ~Logger();

    void Log(std::string);

  private:
    std::ofstream log_file_;
  };


  class Simulator {
  public:
    Simulator(int, EventModel&, EventModel&);

    void RunSimulation();
    void LogCustomer(Customer&);
    void InitializeLogTable();
    void UpdateHistory(Customer&);
    void LogTotals();
    void LogMetrics();

    static int n_wait_, clock_;

  private:
    int n_customer_;
    EventModel arrival_model_;
    EventModel service_model_;
    Logger logger_;
    int total_it_, total_st_, total_wtq_, total_tcss_, total_its_;
  };
}
#endif // HW2_CHANNEL_QUEUE_H_
