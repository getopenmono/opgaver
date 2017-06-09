#include <mono.h>
#include <app_controller.h>
#include "traffic-light.h"

void setup() {
  static TrafficLight trafficlight;

  trafficlight.start();
}

void loop() {
}
