#include "traffic-light.h"

using mono::display::Color;
using mono::geo::Circle;
using mono::geo::Point;
using mono::geo::Rect;
using mono::IApplicationContext;
using mono::ui::ResponderView;
using mono::ui::SceneController;

Lights::Lights ()
:
  SceneController(Rect(0, 0, 176, 220))
{
  addView(threeLights);
}

void Lights::setLight (Light light)
{
  threeLights.light = light;
  threeLights.scheduleRepaint();
  printf("%d\r\n", light);
}

ThreeLights::ThreeLights ()
:
  View(Rect(0, 0, 176, 220)),
  light(static_cast<Light>(0))
{
}

#define RADIUS 35

void ThreeLights::repaint ()
{
  painter.useAntialiasedDrawing(true);
  if ((light & RED) == RED)
  {
    painter.setForegroundColor(RedColor);
    painter.drawFillCircle(Circle(Point(176/2, RADIUS), RADIUS));
  }
  else
  {
    painter.setForegroundColor(BlackColor);
    painter.drawFillCircle(Circle(Point(176/2, RADIUS), RADIUS));
    painter.setForegroundColor(WhiteColor);
    painter.drawCircle(Circle(Point(176/2, RADIUS), RADIUS));
  }
  if ((light & YELLOW) == YELLOW)
  {
    painter.setForegroundColor(Color(0xFF, 0xFF, 0x00));
    painter.drawFillCircle(Circle(Point(176/2, 2*RADIUS+4+RADIUS), RADIUS));
  }
  else
  {
    painter.setForegroundColor(BlackColor);
    painter.drawFillCircle(Circle(Point(176/2, 2*RADIUS+4+RADIUS), RADIUS));
    painter.setForegroundColor(WhiteColor);
    painter.drawCircle(Circle(Point(176/2, 2*RADIUS+4+RADIUS), RADIUS));
  }
  if ((light & GREEN) == GREEN)
  {
    painter.setForegroundColor(GreenColor);
    painter.drawFillCircle(Circle(Point(176/2, 2*RADIUS+4+2*RADIUS+4+RADIUS), RADIUS));
  }
  else
  {
    painter.setForegroundColor(BlackColor);
    painter.drawFillCircle(Circle(Point(176/2, 2*RADIUS+4+2*RADIUS+4+RADIUS), RADIUS));
    painter.setForegroundColor(WhiteColor);
    painter.drawCircle(Circle(Point(176/2, 2*RADIUS+4+2*RADIUS+4+RADIUS), RADIUS));
  }
  painter.useAntialiasedDrawing(false);
}

#define SIZE 55

CornerSelector::CornerSelector (TrafficLight * app)
:
  SceneController(Rect(0, 0, 176, 220)),
  application(app),
  northButton(Rect(0, 0, 176, SIZE), "North"),
  eastButton(Rect(176-SIZE, SIZE, SIZE, 220-2*SIZE), "East"),
  southButton(Rect(0, 220-SIZE, 176, SIZE), "South"),
  westButton(Rect(0, SIZE, SIZE, 220-2*SIZE), "West")
{
  northButton.setClickCallback(this, &CornerSelector::selectNorth);
  northButton.setHighlight(GreenColor);
  addView(northButton);
  eastButton.setClickCallback(this, &CornerSelector::selectEast);
  eastButton.setHighlight(GreenColor);
  addView(eastButton);
  southButton.setClickCallback(this, &CornerSelector::selectSouth);
  southButton.setHighlight(GreenColor);
  addView(southButton);
  westButton.setClickCallback(this, &CornerSelector::selectWest);
  westButton.setHighlight(GreenColor);
  addView(westButton);
  setDismissCallback(static_cast<SceneController*>(this), &SceneController::hide);
}

void CornerSelector::selectNorth ()
{
  application->selectCorner(North);
}

void CornerSelector::selectEast ()
{
  application->selectCorner(East);
}

void CornerSelector::selectSouth ()
{
  application->selectCorner(South);
}

void CornerSelector::selectWest ()
{
  application->selectCorner(West);
}

void TrafficLight::newState (Light n, Light e, Light s, Light w, uint32_t ms)
{
  states.push_back(State(n, e, s, w, ms));
}

void TrafficLight::effectuateState ()
{
  State const & state = states[stateIndex];
  printf("north = %d, east = %d, south = %d, west = %d\r\n", state.north, state.east, state.south, state.west);
  switch (myCorner)
  {
    case None:
      printf("app error: no corner selected\r\n");
      break;
    case North:
      lights.setLight(state.north);
      master_timer.setInterval(state.msPeriod);
      master_timer.start();
      break;
    case East:
      lights.setLight(state.east);
      break;
    case South:
      lights.setLight(state.south);
      break;
    case West:
      lights.setLight(state.west);
      break;
  }
}

void TrafficLight::changeState ()
{
  ++stateIndex;
  if (stateIndex >= states.size())
    stateIndex = 0;
  effectuateState();
}

void TrafficLight::master_startClock ()
{
  master_clock = 0;
  master_pulse.start();
  changeState();
}

void TrafficLight::master_finishClock ()
{
  master_clock = 1;
  master_reset = 1;
}

void TrafficLight::slave_receiveClock ()
{
  if (slave_reset == 0)
    stateIndex = 0;
  changeState();
}

void TrafficLight::setAutoSleep ()
{
  sleeper.setInterval(5 * 60 * 1000);
  sleeper.setCallback(IApplicationContext::EnterSleepMode);
  sleeper.start();
}

// Called by CornerSelector.
void TrafficLight::selectCorner (Corner corner)
{
  myCorner = corner;
  printf("corner = %d\r\n", myCorner);
  if (myCorner == None)
  {
    cornerSelector.show();
    lights.requestDismiss();
    printf("ERROR: no corner selected\r\n");
    return;
  }
  setAutoSleep();
  cornerSelector.requestDismiss();
  lights.show();
  if (states.size() == 0)
    setupDefaultStates();
  if (myCorner == North)
    master_setup();
  else
    slave_setup();
}

void TrafficLight::setupDefaultStates ()
{
  newState( RED,        RED,        RED,        RED,        1000 );
  newState( RED+YELLOW, RED+YELLOW, RED+YELLOW, RED+YELLOW, 1000 );
  newState( GREEN,      GREEN,      GREEN,      GREEN,      1000 );
  newState( YELLOW,     YELLOW,     YELLOW,     YELLOW,     1000 );
}

void TrafficLight::master_setup ()
{
  printf("Master\r\n");
  master_clock.setMode(PullUp);
  master_reset.setMode(PullUp);
  master_timer.setCallback(this, &TrafficLight::master_startClock);
  master_pulse.setCallback(this, &TrafficLight::master_finishClock);
  master_begin();
}

void TrafficLight::master_begin ()
{
  master_clock = 1;
  master_reset = 0;
  stateIndex = 0;
  effectuateState();
}

void TrafficLight::slave_setup ()
{
  printf("Slave\r\n");
  slave_clock.setDebounceTimeout(999);
  slave_clock.setDebouncing(true);
  slave_clock.fall(this,&TrafficLight::slave_receiveClock);
  slave_clock.setInterruptsSleep(false);
  slave_reset.setDebounceTimeout(999);
  slave_reset.setDebouncing(true);
  slave_reset.setInterruptsSleep(false);
  slave_begin();
}

void TrafficLight::slave_begin ()
{
  stateIndex = 0;
  effectuateState();
}

void TrafficLight::start ()
{
  cornerSelector.show();
}

TrafficLight::TrafficLight ()
:
  stateIndex(0),
  myCorner(None),
  cornerSelector(this),
  master_timer(1000, true),
  master_pulse(2, true),
  master_clock(J_RING1, PullNone),
  master_reset(J_RING2, PullNone),
  slave_clock(J_RING1, PullUp),
  slave_reset(J_RING2, PullUp)
{
  CyPins_SlowSlew(J_RING1);
  CyPins_SlowSlew(J_RING2);
  setAutoSleep();
  lights.requestDismiss();
  cornerSelector.show();
}
