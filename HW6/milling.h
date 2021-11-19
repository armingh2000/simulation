#ifndef MILLING_H_
#define MILLING_H_

#include <vector>
#include <sstream>
#include <fstream>

namespace milling {

  template <class T>
  class EventModel {
  public:
    EventModel();
    EventModel(int, std::vector<T>, std::vector<float>);

    T GetEvent();

  private:
    int n_decimal_, n_options_;
    std::vector<int> cum_sum_;
    std::vector<T> options_;
    std::vector<float> probs_, cum_prob_;

    void SetCumProb();
    void SetCumSum();
  }; // class EventModel

  class Logger {
  public:
    Logger();
    ~Logger();

    void Log(std::string);

  private:
    std::ofstream log_file_;
  }; // class Logger

  class Simulator {
  public:
    Simulator(EventModel<int>&, EventModel<int>&);

    virtual void StepSimulate(std::vector<int>&) = 0;
    virtual void UpdateTotals(std::vector<int>&) = 0;

    void ResetTotals();
    void LogMetrics();
    void RunSimulation(int);
    void UpdateTotals(int, int);
    int GetLife(), GetDelay();
    void Log(std::string);
    void SetCosts(int);

  private:
    Logger logger_;
    EventModel<int> life_model_, delay_model_;
    int total_delay_, total_life_, cost_bearings_, cost_delay_, cost_downtime_,
      cost_repair_, total_cost_, total_cost_per_10k_hour;
  }; // class Simulator

  class OnDemandSimulator : public Simulator {
  public:
    using Simulator::Simulator;

    virtual void StepSimulate(std::vector<int>&);
    virtual void UpdateTotals(std::vector<int>&);
  private:
  }; // class OnDemandSimulator

  class BroadcastSimulator : public Simulator{
  public:
    using Simulator::Simulator;

    virtual void StepSimulate(std::vector<int>&);
    virtual void UpdateTotals(std::vector<int>&);
  private:
  }; // class Broadcastsimulator

} // namespace milling

#endif // MILLING_H_
