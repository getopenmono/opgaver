#include <mono.h>
#include <app_controller.h>
#include "traffic-light.h"

void setup() {
  static TrafficLight trafficlight;

  // **************************************************************************
  //                           EDIT HERE
  //
  //                  North       East        South       West        Time (ms)
  //
  trafficlight.newState( RED,        RED,        RED,        RED,        1000 );
  trafficlight.newState( RED+YELLOW, RED+YELLOW, RED+YELLOW, RED+YELLOW, 1000 );
  trafficlight.newState( GREEN,      GREEN,      GREEN,      GREEN,      1000 );
  trafficlight.newState( YELLOW,     YELLOW,     YELLOW,     YELLOW,     1000 );
  //
  // **************************************************************************

  trafficlight.start();
}

void loop() {
}
