/***************************************************************************
 * Copyright (C) 2023-, openlinyou, <linyouhappy@outlook.com>
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 ***************************************************************************/

#ifndef HEADER_OPEN_FSM_H
#define HEADER_OPEN_FSM_H

#include <string>
#include <assert.h>
#include <vector>
#include <map>

namespace open
{

class OpenFSM;
class OpenFSMState;

class OpenFSMAction
{
public:
    OpenFSMAction();
    virtual ~OpenFSMAction();
    virtual void enter(OpenFSM& fsm) const;
    virtual void update(OpenFSM& fsm) const;
    virtual void exit(OpenFSM& fsm) const;
protected:
    std::string actionName_;
    friend class OpenFSM;
};

class OpenFSMState
{
public:
    OpenFSMState();
    OpenFSMState(int eState, const std::string& stateName);
    virtual ~OpenFSMState();

    virtual void enter(OpenFSM& fsm) const;
    virtual void update(OpenFSM& fsm) const;
    virtual void exit(OpenFSM& fsm) const;
protected:
    void nextAction(OpenFSM& fsm) const;
    int eState_;
    std::string stateName_;
    std::vector<const OpenFSMAction*> vectAction_;
    std::vector<OpenFSMState*> vectRelationState_;
    friend class OpenFSM;
};

class OpenFSM
{
public:
    OpenFSM();
    virtual ~OpenFSM();
    virtual void update();
    bool enterState(int eState);
    bool enterState(const std::string& stateName);

    bool nextState(int eState);
    bool nextState(const std::string& stateName);
    void nextAction();

    bool setStates(std::vector<std::string>& vectStateName);
    bool setStates(const std::initializer_list<std::string>& list);
    bool setStates(std::vector<int>& vectStateId);
    bool setStates(const std::initializer_list<int>& list);

    inline int focusEState() { return fsmState_ ? fsmState_->eState_ : 0; }
    inline const std::string& focusStateName() 
    { 
        static std::string empty;
        return fsmState_ ? fsmState_->stateName_ : empty;
    }

    inline void operator=(const std::initializer_list<std::string>& list) { setStates(list); }
    inline void operator=(const std::initializer_list<int>& list) { setStates(list); }

    inline void setCustom(void* custom) { custom_ = custom; }
    template <class T>
    inline T* custom() { return dynamic_cast<T*>((T*)custom_); }

private:
    OpenFSMState* getState(int eState);
    OpenFSMState* getState(const std::string& stateName);

    int eNextState_;
    int eLastState_;
    unsigned int actionIdx_;
    OpenFSMState* fsmState_;
    void* custom_;
    std::vector<OpenFSMState*> vectState_;
    friend class OpenFSMState;

    class OpenFSMPool
    {
    public:
        OpenFSMPool();
        ~OpenFSMPool();
        bool registerAction(OpenFSMAction* action);
        template <class T>
        bool registerAction(const std::string& actionName)
        {
            std::map<std::string, OpenFSMAction*>::iterator iter = mapNameAction_.find(actionName);
            if (iter != mapNameAction_.end())
            {
                printf("OpenFSMPool::registerAction [%s] is exist!\n", actionName.c_str());
                assert(false);
                return false;
            }
            T* t = new T;
            t->actionName_ = actionName;
            mapNameAction_[actionName] = t;
            return true;
        }
        bool registerState(OpenFSMState* state);
        bool registerState(const std::string& stateName, std::vector<std::string>& vectActionName, int eState = -1);
        bool registerRelation(const std::string& stateName, std::vector<std::string>& vectStateName);
        OpenFSMAction* getAction(const std::string& actionName);
        OpenFSMState* getState(int eState);
        OpenFSMState* getState(const std::string& stateName);
    private:
        int uid_;
        std::map<std::string, OpenFSMAction*> mapNameAction_;
        std::map<std::string, OpenFSMState*> mapNameState_;
    };
    static OpenFSMPool OpenFSMPool_;

public:
    template <class T>
    static void RegisterAction(const std::string& actionName)
    {
        OpenFSMPool_.registerAction<T>(actionName);
    }
    static bool RegisterState(const std::string& stateName, std::vector<std::string>& vectActionName, int eState = -1)
    {
        return OpenFSMPool_.registerState(stateName, vectActionName, eState);
    }
    static bool RegisterState(const std::string& stateName, const std::initializer_list<std::string>& list, int eState = -1)
    {
        std::vector<std::string> vectActionName = list;
        return OpenFSMPool_.registerState(stateName, vectActionName, eState);
    }
    static bool RegisterRelation(const std::string& stateName, std::vector<std::string>& vectStateName)
    {
        return OpenFSMPool_.registerRelation(stateName, vectStateName);
    }
    static bool RegisterRelation(const std::string& stateName, const std::initializer_list<std::string>& list)
    {
        std::vector<std::string> vectStateName = list;
        return OpenFSMPool_.registerRelation(stateName, vectStateName);
    }
};

};

#endif /* HEADER_OPEN_FSM_H */
