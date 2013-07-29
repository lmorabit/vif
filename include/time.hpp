#ifndef TIME_HPP
#define TIME_HPP

#include <chrono>
#include "string.hpp"

// Return the current time [seconds]
double now() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count()*1e-6;
}

// Converts an ammount of time [seconds] to a formatted string hh:mm:ss
template<typename T>
std::string date_str(T t) {
    std::string date;
    
    if (t < 1.0) {
        int mag = floor(log10(t));
        if (mag >= -3) {
            return strn(round(t*1e3))+"ms";
        } else if (mag >= -6) {
            return strn(round(t*1e6))+"us";
        } else {
            return strn(round(t*1e9))+"ns";
        }
    } else {
        std::size_t day  = floor(t/(24*60*60));
        std::size_t hour = floor(t/(60*60)) - day*24;
        std::size_t min  = floor(t/60) - day*24*60 - hour*60;
        std::size_t sec  = floor(t) - day*24*60*60 - hour*60*60 - min*60;
        
        if (day  != 0) date += strn(day)+'d';
        if (hour != 0) date += strn(hour,2)+'h';
        if (min  != 0) date += strn(min,2)+'m';
        date += strn(sec,2)+'s';
        
        if (date[0] == '0' && date.size() != 2) {
            date.erase(0,1);
        }
    }
    
    return date;
}

// Execute the provided code and return the time it took to execute [seconds]
template<typename F>
double profile(F&& func) {
    auto start = now();
    func();
    return now() - start;
}

struct progress_t {
    double start;
    std::size_t i = 0;
    std::size_t n;
    std::size_t max_length = 0;
};

// Begin timing an iterative process of 'n' iterations
progress_t progress_start(std::size_t n) {
    progress_t r;
    r.start = now();
    r.n = n;
    return r;
}

void progress_(progress_t& p) {
    double total = now() - p.start;
    double remaining = total*double(p.n)/(p.i+1) - total;

    const std::size_t ndash = 50;
    std::string msg;
    // Progress bar
    msg += "["+std::string(floor(ndash*(p.i+1)/double(p.n)),'-')
        + std::string(ndash - floor(ndash*(p.i+1)/double(p.n)),' ')+"] ";
    // Iteration count
    msg += strn((p.i+1), floor(log10(double(p.n))) + 1, ' ')+" ";
    // Percentage
    msg += strn(std::size_t(floor(100.0*(p.i+1)/double(p.n))), 3, ' ')+"%, ";
    // Timings
    msg += date_str(total)+" elapsed, "+date_str(remaining)+" left, "
        + date_str(total+remaining)+" total";
    // Fill with spaces
    p.max_length = std::max(p.max_length, msg.size());
    msg += std::string(p.max_length - msg.size(), ' ');

    std::cout << "\r" << msg << std::flush;
}

// Updates a progress bar for an iterative process ('p' is created from 'progress_start')
void progress(progress_t& p) {
    progress_(p);

    ++p.i;

    if (p.i >= p.n) {
        std::cout << std::endl;
    }
}

// Updates a progress bar for an iterative process ('p' is created from 'progress_start')
// 'i' is the current iteration number.
template<typename I>
void print_progress(progress_t& p, const I& ti) {
    p.i = ti;
    if (p.i >= p.n) p.i = p.n-1;

    progress_(p);

    if (p.i == p.n-1) {
        std::cout << std::endl;
    }
}

#endif
