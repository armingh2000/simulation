#ifndef NEWS_PAPER_H_
#define NEWS_PAPER_H_

#include <vector>
#include <sstream>
#include <fstream>

namespace news_paper {
  enum DayType {
    kGood = 0,
    kFair,
    kPoor,
  };

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

  class Day {
  public:
    Day();
    Day(int, int, int, DayType);

    void SetFields();
    int GetID(), GetDemand(), GetNNP();
    DayType GetDayType();
    float GetRevenue(), GetLostProfit(), GetSalvage(), GetCost(), GetProfit();

  private:
    int id_, demand_, n_np_;
    DayType day_type_;
    float revenue_, lost_profit_, salvage_, cost_, profit_;
  }; // class Day

  class Logger {
  public:
    ~Logger();

    void SetLogFile(std::string);
    bool HasLogFile();
    void CloseLogFile();
    void Log(std::string);

  private:
    std::ofstream log_file_;
  };

  class Simulator {
  public:
    Simulator(EventModel<DayType>&, EventModel<int>&, EventModel<int>&, EventModel<int>&);

    void RunSimulation();
    void StepSimulate(int, Day&);
    void SetDemandModel(int, std::vector<int>, std::vector<float>);
    void SetNNewsPaper(int);
    void UpdateTotals(Day&);
    void ResetTotals();
    int GetDemand(DayType);
    void LogDay(Day&);
    void LogTotals();
    void InitializeLogTable();
    float GetTotalProfit();

  private:
    static const int kNDay;
    Logger logger_;
    int n_news_paper_;
    EventModel<DayType> day_model_;
    EventModel<int> good_model_, fair_model_, poor_model_;
    float total_revenue_, total_lost_profit_, total_salvage_, total_cost_, total_profit_;
  }; // class Simulator

} // namespace news_paper

#endif // NEWS_PAPER_H_
