#ifndef HW1_BASE_QUEUE_H_
#define HW1_BASE_QUEUE_H_

#include <vector>
#include <fstream>
#include <sstream>

namespace queue_simulation {

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
        Simulator(std::vector<float>, std::vector<float>, const unsigned);

        void RunSimulation();
        int GetCurrentEventType(); // returns the earliest event
        void SetArrivalEvent();
        void SetDepartureEvent();
        void UpdateBTArea();
        void UpdateQTArea();
        void UpdateTotalDelay();
        void UpdateArrivalTimes();
        void LogMetrics();
        void SetMetrics();
        void PrintMetrics(std::string);
        std::string GetStringVector(std::vector<float>);
        void Log();

    private:
        Logger logger_;
        const unsigned kLimit;
        float clock_, last_event_time_, total_delay_, qt_area_, bt_area_;
        unsigned number_serviced_, number_in_queue_;
        bool server_status_;
        float wq_, lq_, p_, l_, e_s_, w_;

        std::vector<float> event_list_;
        std::vector<float> arrival_intervals_;
        std::vector<float> service_times_;
        std::vector<float> arrival_times_;
    };

}
#endif // HW1_BASE_QUEUE_H_
