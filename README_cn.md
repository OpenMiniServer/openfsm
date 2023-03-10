# OpenFSM
一个简单易用的C++有限状态机。

**OpenLinyou项目设计跨平台服务器框架，在VS或者XCode上写代码，无需任何改动就可以编译运行在Linux上，甚至是安卓和iOS.**
OpenLinyou：https://github.com/openlinyou
https://gitee.com/linyouhappy

## 编译和执行
请安装cmake工具，用cmake构建工程，可以在vs或者xcode上编译运行。
源代码：https://github.com/openlinyou/openfsm
https://gitee.com/linyouhappy/openfsm
```
#克隆项目
git clone https://github.com/openlinyou/openfsm
cd ./openfsm
#创建build工程目录
mkdir build
cd build
cmake ..
#如果是win32，在该目录出现openfsm.sln，点击它就可以启动vs写代码调试
make
./test
```

## 全部源文件
+ src/openfsm.h
+ src/openfsm.cpp


## 有限状态机设计星舰

星舰有四种状态：地面压力测试（StateTest）,点火发射升空（StateLaunch），回收返航（ActionReturn）和发射失败（StateFailure）。

每一个状态由几个行为组成。这样，多个行为组成一个状态，多个状态组成一个状态机。
1. 地面压力测试（StateTest）：引擎测试（ActionTestEngine）和燃料罐测试（ActionTestTank）；
2. 点火发射升空（StateLaunch）：星舰点火（ActionFireUp）和发射升空（ActionLaunch）；
3. 回收返航（ActionReturn）：返回地面回收（ActionReturn）；
4. 发射失败（StateFailure）：发射失败（ActionFailure）；

状态切换。有两种状态切换。
1. 地面压力测试（StateTest）如果成功，就切换到点火发射升空（StateLaunch），否则，切换到发射失败（StateFailure）
2. 点火发射升空（StateLaunch）如果成功，就切换到回收返航（ActionReturn），否则，切换到发射失败（StateFailure）

在创建状态机之前，先进行动作注册。
```C++
OpenFSM::RegisterAction<ActionTestEngine>("ActionTestEngine");
OpenFSM::RegisterAction<ActionTestTank>("ActionTestTank");
OpenFSM::RegisterAction<ActionFireUp>("ActionFireUp");
OpenFSM::RegisterAction<ActionLaunch>("ActionLaunch");
OpenFSM::RegisterAction<ActionReturn>("ActionReturn");
OpenFSM::RegisterAction<ActionFailure>("ActionFailure");
```

选择一组动作，组成一个状态。共四个状态。
```C++
    OpenFSM::RegisterState("StateTest", { "ActionTestEngine", "ActionTestTank" }, EStateTest);
    OpenFSM::RegisterState("StateLaunch", { "ActionFireUp", "ActionLaunch" }, EStateLaunch);
    OpenFSM::RegisterState("StateRecycle", { "ActionReturn" }, EStateRecycle);
    OpenFSM::RegisterState("StateFailure", { "ActionFailure"}, EStateFailure);
```

指定状态可以切换到下一个状态。例如，星舰处理发射失败爆炸状态，那就无法切换到其他状态。
```C++
OpenFSM::RegisterRelation("StateTest", { "StateLaunch", "StateFailure" });
OpenFSM::RegisterRelation("StateLaunch", { "StateRecycle", "StateFailure" });
```

完整设计代码
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
