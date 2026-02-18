/**
 * @file structs.h
 * @brief Common utility structures for timing and performance measurement.
 * 
 * This header provides lightweight structures for profiling and performance
 * monitoring during development and runtime. Timers are RAII-based for
 * automatic lifecycle management.
 * 
 * Architecture:
 * - Timer provides scoped timing with automatic reporting
 * - AverageTime implements exponential moving average for smoothing metrics
 * - Debug output conditionally compiled in _DEBUG builds
 * 
 * Design Principles:
 * - RAII for automatic timer start/stop
 * - Minimal overhead in release builds
 * - Flexible output control (print or silent)
 * 
 * @note Timer uses std::clock() which measures CPU time, not wall-clock time.
 * @note For GPU profiling, use OpenGL timer queries instead.
 */

#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <iostream>

/**
 * @brief RAII-based timer for profiling code sections.
 * 
 * Timer automatically starts on construction and reports elapsed time on
 * destruction. Provides intermediate Tick() for checkpoint measurements.
 * Output is conditionally compiled in DEBUG builds only.
 * 
 * Lifecycle:
 * 1. Construction: Capture start time, print start message
 * 2. Tick(): Print duration since last tick or start
 * 3. Destruction: Print total duration from start to end
 * 
 * Timing Precision:
 * - Uses std::clock() for CPU time measurement
 * - Resolution: CLOCKS_PER_SEC (typically microseconds)
 * - Measures CPU time, not wall-clock (excludes I/O wait, etc.)
 * 
 * @note Timer measures CPU time, which may differ from real time on multi-core.
 * @note For accurate GPU timing, use glQueryCounter with GL_TIMESTAMP.
 */
struct Timer
{
	time_t start = 0;        ///< Timer start timestamp (clock ticks)
	time_t end = 0;          ///< Timer end timestamp (clock ticks)
	time_t tick = 0;         ///< Current tick timestamp (clock ticks)
	time_t temp = 0;         ///< Previous tick timestamp for delta calculation
	time_t __duration = 0;   ///< Cached duration value
	int fact = 1;            ///< Multiplication factor for time conversion (1 = ms)
	std::string name;        ///< Timer name for identification in output
	bool is_print;           ///< Enable/disable automatic output

	/**
	 * @brief Construct timer and start measurement.
	 * 
	 * Captures start time and prints initialization message if is_print is true.
	 * Timer begins immediately upon construction (RAII pattern).
	 * 
	 * @param name Descriptive name for timer (appears in output)
	 * @param fact Time conversion factor (1 = milliseconds, 1000 = seconds)
	 * @param print Enable automatic output (DEBUG builds only)
	 * 
	 * @note Output only appears in DEBUG builds, regardless of is_print.
	 * 
	 * Example:
	 * @code
	 * {
	 *     Timer t("Mesh Loading");
	 *     LoadMeshFromFile(...);
	 * } // Timer destroyed here, prints total duration
	 * @endcode
	 */
	Timer(std::string name="Thread", int fact = 1, bool print = true) 
		:name(name), fact(fact), is_print(print)
	{
#ifdef _DEBUG
		if (is_print)
			std::cout << "[ " + name + " Start ]\n";
#endif // _DEBUG
		start = tick = temp = clock();
	}

	/**
	 * @brief Record intermediate checkpoint and print delta time.
	 * 
	 * Captures current time and prints duration since last Tick() or construction.
	 * Useful for profiling multiple stages within a single timer scope.
	 * 
	 * @return Duration since last tick in clock ticks (divide by CLOCKS_PER_SEC for seconds)
	 * 
	 * @note Resets internal delta reference - next Tick() measures from this point.
	 * @note Output only in DEBUG builds when is_print is true.
	 * 
	 * Example:
	 * @code
	 * Timer t("Renderer");
	 * DrawGeometry();
	 * t.Tick();  // Prints geometry pass duration
	 * ApplyPostProcessing();
	 * t.Tick();  // Prints post-processing duration
	 * @endcode
	 */
	time_t Tick() {
		/*std::cout << 000000 << "\n";*/
		tick = clock();
		__duration = tick - temp;
		temp = tick;
#ifdef _DEBUG
		if (is_print)
			std::cout /*<< "\r"*/ << "[ duration = " << __duration * fact << "ms ]\n";
#endif // _DEBUG
		return __duration;
	}

	/**
	 * @brief Get total elapsed time without stopping timer.
	 * 
	 * Calculates and prints duration from start to current time without
	 * destroying the timer. Useful for progress monitoring in long operations.
	 * 
	 * @return Total duration since construction in clock ticks
	 * 
	 * @note Does not stop timer - can be called multiple times.
	 * @note Output only in DEBUG builds when is_print is true.
	 * 
	 * Example:
	 * @code
	 * Timer t("Batch Processing");
	 * for (int i = 0; i < 1000; i++) {
	 *     ProcessItem(i);
	 *     if (i % 100 == 0) t.GetDuration(); // Print progress
	 * }
	 * @endcode
	 */
	time_t GetDuration() {
		end = clock();
#ifdef _DEBUG
		if (is_print)
			std::cout /*<< "\r"*/ << "[ " + name + " Whole Time = " << (end - start) * fact << "ms ]\n";
#endif // _DEBUG
		return (end - start);
	}

	/**
	 * @brief Destructor: stop timer and print total duration.
	 * 
	 * Automatically called when timer goes out of scope. Captures end time
	 * and prints total duration if is_print is enabled.
	 * 
	 * @note RAII guarantees timer stop even if exceptions thrown.
	 * @note Output only in DEBUG builds when is_print is true.
	 */
	~Timer() {
		end = clock();
		__duration = end - start;
#ifdef _DEBUG
		if (is_print)
			std::cout /*<< "\r"*/ << "[ "+name+" Whole Time = " << __duration * fact << "ms ]\n";
#endif // _DEBUG
	}
};

/**
 * @brief Exponential moving average for smoothing time-series data.
 * 
 * AverageTime computes exponentially-weighted moving average (EWMA) to smooth
 * noisy measurements like frame times or FPS. More recent values are weighted
 * higher than older values.
 * 
 * Algorithm:
 * - New average = Old average + (New sample - Old average) / Window size
 * - Equivalent to EWMA with alpha = 1 / _L
 * 
 * Mathematical Properties:
 * - Larger _L = slower response, smoother output
 * - Smaller _L = faster response, more noise
 * - Typical _L for FPS smoothing: 30-120 samples
 * 
 * @tparam _L Window size (number of samples for smoothing)
 * 
 * @note Does not store sample history - constant memory usage.
 * @note Not thread-safe - use per-thread instances if needed.
 */
template<int _L>
struct AverageTime
{
	float result = 0; ///< Current average value

	/**
	 * @brief Update average with new sample.
	 * 
	 * Incorporates new measurement into exponential moving average.
	 * Result converges to stable value after approximately _L samples.
	 * 
	 * @param _rate New sample value to incorporate
	 * 
	 * @note First _L samples have reduced accuracy (startup transient).
	 * @note Can be called with any numeric type (implicitly converts to float).
	 * 
	 * Example:
	 * @code
	 * AverageTime<60> fps_avg; // 60-sample window
	 * while (running) {
	 *     float frame_time = MeasureFrameTime();
	 *     fps_avg.Update(1.0f / frame_time);
	 *     DisplayFPS(fps_avg.result); // Smoothed FPS value
	 * }
	 * @endcode
	 */
	void Update(float _rate) {
		result += (_rate - result) / _L;
	}
};
