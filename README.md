# OpenFSM
A simple and easy-to-use C++ finite state machine.


## Compilation and execution
Please install the cmake tool. With cmake you can build a VS or XCode project and compile and run it on VS or XCode. 
Source code:https://github.com/openlinyou/openfsm
```
# Clone the project
git clone https://github.com/openlinyou/openfsm
cd ./openfsm
# Create a build project directory
mkdir build
cd build
cmake ..
# If it's win32, openfsm.sln will appear in this directory. Click it to start VS for coding and debugging.
make
./test
```

## All source files
+ src/openfsm.h
+ src/openfsm.cpp


## Finite State Machine Design for a Spaceship

The spaceship has four states: Ground Pressure Test (StateTest), Ignition Launch (StateLaunch), Recovery Return (ActionReturn), and Launch Failure (StateFailure).


Each state consists of several actions. In this way, multiple actions make up a state, and multiple states make up a state machine.
1. Ground Pressure Test (StateTest): Engine Test (ActionTestEngine) and Fuel Tank Test (ActionTestTank);
2. Ignition Launch (StateLaunch): Spaceship Ignition (ActionFireUp) and Launch Lift-off (ActionLaunch);
3. Recovery Return (ActionReturn): Return to Ground for Recovery (ActionReturn);
4. Launch Failure (StateFailure): Launch Failure(ActionFailure);


State transitions. There are two types of state transitions.
1. If the Ground Pressure Test(StateTest) is successful, it switches to Ignition Launch(StateLaunch), otherwise it switches to Launch Failure(StateFailure).
2. If the Ignition Launch(StateLaunch) is successful, it switches to Recovery Return(ActionReturn), otherwise it switches to Launch Failure(StateFailure).


Before creating the state machine, register the actions first.
```C++
OpenFSM::RegisterAction<ActionTestEngine>("ActionTestEngine");
OpenFSM::RegisterAction<ActionTestTank>("ActionTestTank");
OpenFSM::RegisterAction<ActionFireUp>("ActionFireUp");
OpenFSM::RegisterAction<ActionLaunch>("ActionLaunch");
OpenFSM::RegisterAction<ActionReturn>("ActionReturn");
OpenFSM::RegisterAction<ActionFailure>("ActionFailure");
```

Select a set of actions to form a state. There are four states in total.
```C++
    OpenFSM::RegisterState("StateTest", { "ActionTestEngine", "ActionTestTank" }, EStateTest);
    OpenFSM::RegisterState("StateLaunch", { "ActionFireUp", "ActionLaunch" }, EStateLaunch);
    OpenFSM::RegisterState("StateRecycle", { "ActionReturn" }, EStateRecycle);
    OpenFSM::RegisterState("StateFailure", { "ActionFailure"}, EStateFailure);
```

Specify that the state can switch to the next state. For example, if the spaceship is dealing with a launch failure explosion state, it cannot switch to other states.
```C++
OpenFSM::RegisterRelation("StateTest", { "StateLaunch", "StateFailure" });
OpenFSM::RegisterRelation("StateLaunch", { "StateRecycle", "StateFailure" });
```

Assemble the state machine, which can be assembled using state names or ids. 
If it is a traditional rocket, there are only three states: StateTest, StateLaunch, StateFailure. 
Starship has a recycling state, with four states: StateTest, StateLaunch, StateRecycle, StateFailure.
```C++
openFSM_.setStates({ "StateTest", "StateLaunch", "StateRecycle", "StateFailure" });
//or
openFSM_.setStates({ EStateTest, EStateLaunch, EStateRecycle, EStateFailure });
```

Complete design code
```C++
#include <assert.h>
#include <time.h>
#include <math.h>
#include "openfsm.h"

using namespace open;

enum EFMSState
{
    EStateTest = 100,
    EStateLaunch = 101,
    EStateRecycle = 102,
    EStateFailure = 103,
};
struct StarShip
{
    OpenFSM openFSM_;
    void start()
    {
        std::srand((int)time(NULL));
        openFSM_.setCustom(this);
        // or openFSM_.setStates({ "StateTest", "StateLaunch", "StateRecycle", "StateFailure" });
        openFSM_.setStates({ EStateTest, EStateLaunch, EStateRecycle, EStateFailure });
        openFSM_.enterState(EStateTest);
    }
    bool testEngine() {return std::rand() % 3 == 0;}
    bool testTank() {return std::rand() % 3 == 0;}
    bool fireUp() {return std::rand() % 3 == 0;}
    bool launch() {return std::rand() % 3 == 0;}
    bool update()
    {
        openFSM_.update();
        return openFSM_.focusEState() == EStateRecycle;
    }
};

//StateTest
class ActionTestEngine: public OpenFSMAction
{
    virtual void enter(OpenFSM& fsm) const
    {
        printf("[%s.%s] enter\n", fsm.focusStateName().c_str(), actionName_.c_str());
    }
    virtual void update(OpenFSM& fsm) const
    {
        printf("[%s.%s] update\n", fsm.focusStateName().c_str(), actionName_.c_str());
        if (fsm.custom<StarShip>()->testEngine())
        {
            printf("==>StarShip Engine Ok\n");
            fsm.nextAction();
        }
        else
        {
            printf("==>StarShip Engine Bad\n");
            fsm.nextState(EStateFailure);
        }
    }
    virtual void exit(OpenFSM& fsm) const
    {
        printf("[%s.%s] exit\n", fsm.focusStateName().c_str(), actionName_.c_str());
    }
};

class ActionTestTank: public OpenFSMAction
{
    virtual void enter(OpenFSM& fsm) const
    {
        printf("[%s.%s] enter\n", fsm.focusStateName().c_str(), actionName_.c_str());
    }
    virtual void update(OpenFSM& fsm) const
    {
        printf("[%s.%s] update\n", fsm.focusStateName().c_str(), actionName_.c_str());
        if (fsm.custom<StarShip>()->testTank())
        {
            printf("==>StarShip Tank Ok\n");
            fsm.nextAction();
        }
        else
        {
            printf("==>StarShip Tank Bad\n");
            fsm.nextState(EStateFailure);
        }
    }
    virtual void exit(OpenFSM& fsm) const
    {
        printf("[%s.%s] exit\n", fsm.focusStateName().c_str(), actionName_.c_str());
    }
};

//StateLaunch
class ActionFireUp : public OpenFSMAction
{
    virtual void enter(OpenFSM& fsm) const
    {
        printf("[%s.%s] enter\n", fsm.focusStateName().c_str(), actionName_.c_str());
    }
    virtual void update(OpenFSM& fsm) const
    {
        printf("[%s.%s] update\n", fsm.focusStateName().c_str(), actionName_.c_str());
        if (fsm.custom<StarShip>()->fireUp())
        {
            printf("==>StarShip FireUp Ok\n");
            fsm.nextAction();
        }
        else
        {
            printf("==>StarShip FireUp Failed\n");
            fsm.nextState(EStateFailure);
        }
    }
    virtual void exit(OpenFSM& fsm) const
    {
        printf("[%s.%s] exit\n", fsm.focusStateName().c_str(), actionName_.c_str());
    }
};
class ActionLaunch : public OpenFSMAction
{
    virtual void enter(OpenFSM& fsm) const
    {
        printf("[%s.%s] enter\n", fsm.focusStateName().c_str(), actionName_.c_str());
    }
    virtual void update(OpenFSM& fsm) const
    {
        printf("[%s.%s] update\n", fsm.focusStateName().c_str(), actionName_.c_str());
        if (fsm.custom<StarShip>()->launch())
        {
            printf("==>StarShip Launch Ok\n");
            fsm.nextAction();
        }
        else
        {
            printf("==>StarShip Launch Failed\n");
            fsm.nextState(EStateFailure);
        }
    }
    virtual void exit(OpenFSM& fsm) const
    {
        printf("[%s.%s] exit\n", fsm.focusStateName().c_str(), actionName_.c_str());
    }
};

//StateRecycle
class ActionReturn : public OpenFSMAction
{
    virtual void enter(OpenFSM& fsm) const
    {
        printf("[%s.%s] enter\n", fsm.focusStateName().c_str(), actionName_.c_str());
    }
    virtual void update(OpenFSM& fsm) const
    {
        printf("[%s.%s] update\n", fsm.focusStateName().c_str(), actionName_.c_str());
        printf("==>Congratulation! Complete mission!\n");
    }
    virtual void exit(OpenFSM& fsm) const
    {
        printf("[%s.%s] exit\n", fsm.focusStateName().c_str(), actionName_.c_str());
    }
};

//StateFailure
class ActionFailure : public OpenFSMAction
{
    virtual void enter(OpenFSM& fsm) const
    {
        printf("[%s.%s] enter\n", fsm.focusStateName().c_str(), actionName_.c_str());
    }
    virtual void update(OpenFSM& fsm) const
    {
        printf("[%s.%s] update\n", fsm.focusStateName().c_str(), actionName_.c_str());
        printf("==>StarShip Launch again\n");
        fsm.nextState(EStateTest);
    }
    virtual void exit(OpenFSM& fsm) const
    {
        printf("[%s.%s] exit\n", fsm.focusStateName().c_str(), actionName_.c_str());
    }
};

int main()
{
    OpenFSM::RegisterAction<ActionTestEngine>("ActionTestEngine");
    OpenFSM::RegisterAction<ActionTestTank>("ActionTestTank");
    OpenFSM::RegisterAction<ActionFireUp>("ActionFireUp");
    OpenFSM::RegisterAction<ActionLaunch>("ActionLaunch");
    OpenFSM::RegisterAction<ActionReturn>("ActionReturn");
    OpenFSM::RegisterAction<ActionFailure>("ActionFailure");

    OpenFSM::RegisterState("StateTest", { "ActionTestEngine", "ActionTestTank" }, EStateTest);
    OpenFSM::RegisterState("StateLaunch", { "ActionFireUp", "ActionLaunch" }, EStateLaunch);
    OpenFSM::RegisterState("StateRecycle", { "ActionReturn" }, EStateRecycle);
    OpenFSM::RegisterState("StateFailure", { "ActionFailure"}, EStateFailure);

    OpenFSM::RegisterRelation("StateTest", { "StateLaunch", "StateFailure" });
    OpenFSM::RegisterRelation("StateLaunch", { "StateRecycle", "StateFailure" });

    StarShip starShip;
    starShip.start();
    int count = 1000;
    while (count-- > 0)
    {
        if (starShip.update())
            break;
    }
    return getchar();
}
```
