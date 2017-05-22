#ifndef mono_traffic_light_h
#define mono_traffic_light_h

#include <mono.h>
#include <vector>

enum Light
{
  RED = 1,
  YELLOW = 2,
  GREEN = 4
};

enum Corner
{
  None,
  North,
  East,
  South,
  West
};

class ThreeLights
:
  public mono::ui::View
{
  void repaint ();
public:
  Light light;
  ThreeLights ();
};

class Lights
:
  public mono::ui::SceneController
{
  ThreeLights threeLights;
public:
  Lights ();
  void setLight (Light light);
};

inline Light operator + (Light lhs, Light rhs)
{
  return static_cast<Light>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

class TrafficLight;

class CornerSelector
:
  public mono::ui::SceneController
{
  TrafficLight * application;
  mono::ui::ButtonView northButton;
  mono::ui::ButtonView eastButton;
  mono::ui::ButtonView southButton;
  mono::ui::ButtonView westButton;
  void selectNorth ();
  void selectEast ();
  void selectSouth ();
  void selectWest ();
public:
  CornerSelector (TrafficLight *);
};

struct State
{
  Light north;
  Light east;
  Light south;
  Light west;
  uint32_t msPeriod;

  State (Light n, Light e, Light s, Light w, uint32_t ms)
  :
    north(n),
    east(e),
    south(s),
    west(w),
    msPeriod(ms)
  {}
};

class TrafficLight
{
  void setUpStates ();
  void effectuateState ();
  void changeState ();
  size_t stateIndex;
  Corner myCorner;
  CornerSelector cornerSelector;
  Lights lights;
  std::vector<State> states;
  mono::Timer sleeper;
  void setAutoSleep ();
  void monoWakeFromReset ();
  void setupDefaultStates();
  // Only used by master.
  mono::Timer master_timer;
  mono::Timer master_pulse;
  mono::io::DigitalOut master_clock;
  mono::io::DigitalOut master_reset;
  void master_setup ();
  void master_begin ();
  void master_startClock ();
  void master_finishClock ();
  // Only used by slaves.
  mono::QueueInterrupt slave_clock;
  mono::QueueInterrupt slave_reset;
  void slave_setup ();
  void slave_begin ();
  void slave_receiveClock ();
public:
  TrafficLight ();
  void newState (Light n, Light e, Light s, Light w, uint32_t ms);
  void start();
  void selectCorner (Corner);
};

#endif /* mono_traffic_light_h */
