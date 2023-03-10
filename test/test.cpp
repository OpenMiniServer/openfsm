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
