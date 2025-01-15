#include <ArduinoSTL.h>

#define lint long long int
#define max_voltage 125
#define PI 3.14159265358979323846

size_t to_mili (size_t a) { return 100 * a; }
double to_voltage(double voltage) { return voltage * max_voltage + max_voltage; }

size_t pins[] = {3, 5, 6, 9, 10};

std::vector<std::vector<std::pair<size_t, size_t>>> pins_starts = {
  {{0, 50}, {2000, 30}, {5000, 40}},     // LED 1
  {{1000, 20}, {4000, 25}, {7000, 30}},  // LED 2
  {{1500, 35}, {4500, 25}},              // LED 3
  {{2500, 30}, {6000, 20}, {8500, 25}},  // LED 4
  {{500, 25}, {3000, 20}, {7500, 30}},   // LED 5
};

std::vector<std::vector<std::pair<size_t, size_t>>> pins_starts2 = {
  {{0, 50},{40, 50},{100, 50}},
  {{20, 50},{30, 50},{90, 50},{130, 50},{160, 50}},
  {{60, 50},{120, 50}},
  {{80, 50},{120, 50},{400, 50}},
  {{0, 50},{30, 50}, {160, 50}},
};


void setup() {
  for (auto pin : pins) {
    pinMode(pin, OUTPUT);
  }
}

lint process(lint curr_time, const std::vector<std::pair<size_t, size_t>> pin_starts, size_t pin) {

  size_t max_time = 0;

  for (const auto [start, duration] : pin_starts) {
    max_time = max_time < start + duration ? start + duration : max_time;
  }

  max_time = to_mili(max_time);
  
  bool reset_time = max_time < curr_time;
  curr_time = reset_time ? 0 : curr_time;

  double max_curr = -1;

  for (const auto [start, duration] : pin_starts) {
    if (to_mili(start) <= curr_time && to_mili(start + duration) >= curr_time) {
      double curr = -cos((1.0f * 2 * PI * (curr_time - to_mili(start))) / to_mili(duration));
      max_curr = std::max(curr, max_curr);
    }
  }
  analogWrite(pin, to_voltage(max_curr));

  return reset_time ? 0 : curr_time;
}

std::vector<lint> pin_times = {0, 0, 0, 0, 0};
lint last_time = 0;

void loop() {
  lint curr_time = millis();
  lint time_diff = curr_time - last_time;

  for (auto &time : pin_times) {
    time += time_diff;
  }

  for (int i = 0; i < 5; ++i) {
    pin_times[i] = process(pin_times[i], pins_starts[i], pins[i]);
  }


  if (last_time != curr_time) last_time = curr_time;
}
