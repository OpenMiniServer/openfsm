/***************************************************************************
 * Copyright (C) 2023-, openlinyou, <linyouhappy@outlook.com>
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 ***************************************************************************/
#include "openfsm.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <string>

namespace open
{

//OpenFSMAction
OpenFSMAction::OpenFSMAction()
{

}
OpenFSMAction::~OpenFSMAction()
{
}

void OpenFSMAction::enter(OpenFSM& fsm) const
{
}

void OpenFSMAction::update(OpenFSM& fsm) const
{
}

void OpenFSMAction::exit(OpenFSM& fsm) const
{
}

//OpenFSMState
OpenFSMState::OpenFSMState()
    :eState_(-1)
    ,stateName_("UnkownState")
{
    assert(false);
}

OpenFSMState::OpenFSMState(int eState, const std::string& stateName)
    :eState_(eState)
    ,stateName_(stateName)
{
}

OpenFSMState::~OpenFSMState()
{
}

void OpenFSMState::enter(OpenFSM& fsm) const
{
    assert(eState_ != 0);
    fsm.actionIdx_ = 0;
    if (fsm.actionIdx_ >= vectAction_.size())
        return;

    vectAction_[fsm.actionIdx_]->enter(fsm);
}

void OpenFSMState::update(OpenFSM& fsm) const
{
    if (fsm.actionIdx_ >= vectAction_.size())
    {
        if (!vectRelationState_.empty())
        {
            fsm.nextState(vectRelationState_[0]->eState_);
            return;
        }
        assert(false);
        return;
    }
    vectAction_[fsm.actionIdx_]->update(fsm);
}

void OpenFSMState::exit(OpenFSM& fsm) const
{
    if (fsm.actionIdx_ < vectAction_.size())
    {
        vectAction_[fsm.actionIdx_]->exit(fsm);
    }
}

void OpenFSMState::nextAction(OpenFSM& fsm) const
{
    if (fsm.actionIdx_ < vectAction_.size())
    {
        vectAction_[fsm.actionIdx_]->exit(fsm);
    }
    fsm.actionIdx_++;
    if (fsm.actionIdx_ < vectAction_.size())
    {
        vectAction_[fsm.actionIdx_]->enter(fsm);
    }
    else
    {
        if (!vectRelationState_.empty())
        {
            fsm.nextState(vectRelationState_[0]->eState_);
            return;
        }
        assert(false);
        return;
    }
}


//OpenFSM
OpenFSM::OpenFSMPool OpenFSM::OpenFSMPool_;
OpenFSM::OpenFSM()
 :eNextState_(0)
,eLastState_(0)
,fsmState_(0)
{
}

OpenFSM::~OpenFSM()
{
}

bool OpenFSM::enterState(int eState)
{
    if (fsmState_)
    {
        fsmState_->exit(*this);
        eLastState_ = fsmState_->eState_;
    }
    fsmState_ = getState(eState);
    if (!fsmState_)
    {
        return false;
    }
    fsmState_->enter(*this);
    return true;
}

bool OpenFSM::enterState(const std::string& stateName)
{
    if (fsmState_)
    {
        fsmState_->exit(*this);
        eLastState_ = fsmState_->eState_;
    }
    fsmState_ = getState(stateName);
    if (!fsmState_)
    {
        return false;
    }
    fsmState_->enter(*this);
    return true;
}

bool OpenFSM::nextState(int eState)
{
    OpenFSMState* state = getState(eState);
    if (!state)
    {
        return false;
    }
    assert(state->eState_ == eState);
    eNextState_ = eState;
    return true;
}

bool OpenFSM::nextState(const std::string& stateName)
{
    OpenFSMState* state = getState(stateName);
    if (!state)
    {
        return false;
    }
    assert(state->stateName_ == stateName);
    eNextState_ = state->eState_;
    return true;
}

void OpenFSM::nextAction()
{
    if (!fsmState_)
    {
        if (vectState_.empty())
        {
            return;
        }
        nextState(vectState_[0]->eState_);
        return;
    }
    fsmState_->nextAction(*this);
}

void OpenFSM::update()
{
    eNextState_ = 0;
    if (fsmState_)
    {
        fsmState_->update(*this);
    }
    while (eNextState_ != 0)
    {
        int state = eNextState_;
        eNextState_ = 0;
        enterState(state);
        if (fsmState_)
        {
            fsmState_->update(*this);
        }
    }
}

bool OpenFSM::setStates(std::vector<std::string>& vectStateName)
{
    vectState_.clear();
    OpenFSMState* state = 0;
    for (size_t i = 0; i < vectStateName.size(); ++i)
    {
        state = OpenFSMPool_.getState(vectStateName[i]);
        if (!state)
        {
            vectState_.clear();
            assert(false);
            return false;
        }
        assert(state->eState_ != 0);
        vectState_.push_back(state);
    }
    return true;
}

bool OpenFSM::setStates(const std::initializer_list<std::string>& list)
{
    vectState_.clear();
    OpenFSMState* state = 0;
    std::initializer_list<std::string>::const_iterator iter = list.begin();
    for (; iter != list.end(); iter++)
    {
        state = OpenFSMPool_.getState(*iter);
        if (!state)
        {
            vectState_.clear();
            assert(false);
            return false;
        }
        assert(state->eState_ != 0);
        vectState_.push_back(state);
    }
    return true;
}

bool OpenFSM::setStates(std::vector<int>& vectStateId)
{
    vectState_.clear();
    OpenFSMState* state = 0;
    for (size_t i = 0; i < vectStateId.size(); ++i)
    {
        state = OpenFSMPool_.getState(vectStateId[i]);
        if (!state)
        {
            vectState_.clear();
            assert(false);
            return false;
        }
        assert(state->eState_ != 0);
        vectState_.push_back(state);
    }
    return true;
}

bool OpenFSM::setStates(const std::initializer_list<int>& list)
{
    vectState_.clear();
    OpenFSMState* state = 0;
    std::initializer_list<int>::const_iterator iter = list.begin();
    for (; iter != list.end(); iter++)
    {
        state = OpenFSMPool_.getState(*iter);
        if (!state)
        {
            vectState_.clear();
            assert(false);
            return false;
        }
        assert(state->eState_ != 0);
        vectState_.push_back(state);
    }
    return true;
}

OpenFSMState* OpenFSM::getState(int eState)
{
    for (size_t i = 0; i < vectState_.size(); ++i)
    {
        if (vectState_[i]->eState_ == eState)
            return vectState_[i];
    }
    return 0;
}

OpenFSMState* OpenFSM::getState(const std::string& stateName)
{
    for (size_t i = 0; i < vectState_.size(); ++i)
    {
        if (vectState_[i]->stateName_ == stateName)
            return vectState_[i];
    }
    return 0;
}


//OpenFSMPool
OpenFSM::OpenFSMPool::OpenFSMPool()
    :uid_(1)
{
}

OpenFSM::OpenFSMPool::~OpenFSMPool()
{
    std::map<std::string, OpenFSMState*>::iterator iter1 = mapNameState_.begin();
    for (; iter1 != mapNameState_.end(); iter1++)
    {
        delete iter1->second;
    }
    mapNameState_.clear();
    std::map<std::string, OpenFSMAction*>::iterator iter2 = mapNameAction_.begin();
    for (; iter2 != mapNameAction_.end(); iter2++)
    {
        delete iter2->second;
    }
    mapNameAction_.clear();
}

bool OpenFSM::OpenFSMPool::registerAction(OpenFSMAction* action)
{
    if (!action)
    {
        assert(false);
        return false;
    }
    const std::string& actionName = action->actionName_;
    std::map<std::string, OpenFSMAction*>::iterator iter = mapNameAction_.find(actionName);
    if (iter != mapNameAction_.end())
    {
        printf("OpenFSMPool::registerAction actionName [%s] is exist!\n", actionName.c_str());
        assert(false);
        return false;
    }
    mapNameAction_[actionName] = action;
    return true;
}

bool OpenFSM::OpenFSMPool::registerState(OpenFSMState* state)
{
    if (!state)
    {
        assert(false);
        return false;
    }
    const std::string& stateName = state->stateName_;
    std::map<std::string, OpenFSMState*>::iterator iter = mapNameState_.find(stateName);
    if (iter != mapNameState_.end())
    {
        printf("OpenFSMPool::registerState stateName [%s] is exist!\n", stateName.c_str());
        assert(false);
        return false;
    }
    if (state->eState_ <= 0)
    {
        state->eState_ = -uid_++;
    }
    assert(state->eState_ != 0);
    mapNameState_[stateName] = state;
    return true;
}

bool OpenFSM::OpenFSMPool::registerState(const std::string& stateName, std::vector<std::string>& vectActionName, int eState)
{
    std::map<std::string, OpenFSMState*>::iterator iter = mapNameState_.find(stateName);
    if (iter != mapNameState_.end())
    {
        printf("OpenFSMPool::registerState stateName [%s] is exist!\n", stateName.c_str());
        assert(false);
        return false;
    }
    if (eState <= 0)
    {
        eState = -uid_++;
    }
    OpenFSMState* state = new OpenFSMState(eState, stateName);
    OpenFSMAction* action = 0;
    for (size_t i = 0; i < vectActionName.size(); ++i)
    {
        action = OpenFSMPool_.getAction(vectActionName[i]);
        if (!action)
        {
            delete state;
            printf("OpenFSMPool::registerState actionName [%s] is not exist!\n", vectActionName[i].c_str());
            assert(false);
            return false;
        }
        state->vectAction_.push_back(action);
    }
    assert(state->eState_ != 0);
    mapNameState_[stateName] = state;
    return true;
}

bool OpenFSM::OpenFSMPool::registerRelation(const std::string& stateName, std::vector<std::string>& vectStateName)
{
    if (vectStateName.empty())
    {
        printf("OpenFSMPool::registerRelation[%s] vectStateName is empty!\n", stateName.c_str());
        assert(false);
        return false;
    }
    OpenFSMState* focusState = getState(stateName);
    if (!focusState)
    {
        printf("OpenFSMPool::registerRelation[%s] state is not exist!\n", stateName.c_str());
        assert(false);
        return false;
    }
    if (!focusState->vectRelationState_.empty())
    {
        printf("OpenFSMPool::registerRelation[%s] state had relation!\n", stateName.c_str());
        assert(false);
        return false;
    }
    std::vector<OpenFSMState*> vectState;
    OpenFSMState* state = 0;
    for (size_t i = 0; i < vectStateName.size(); ++i)
    {
        state = getState(vectStateName[i]);
        if (!state)
        {
            printf("OpenFSMPool::registerRelation stateName [%s] is not exist!\n", vectStateName[i].c_str());
            assert(false);
            return false;
        }
        vectState.push_back(state);
    }
    focusState->vectRelationState_ = vectState;
    return true;
}

OpenFSMAction* OpenFSM::OpenFSMPool::getAction(const std::string& actionName)
{
    std::map<std::string, OpenFSMAction*>::iterator iter = mapNameAction_.find(actionName);
    if (iter != mapNameAction_.end())
    {
        return iter->second;
    }
    return 0;
}

OpenFSMState* OpenFSM::OpenFSMPool::getState(const std::string& stateName)
{
    std::map<std::string, OpenFSMState*>::iterator iter = mapNameState_.find(stateName);
    if (iter != mapNameState_.end())
    {
        return iter->second;
    }
    return 0;
}

OpenFSMState* OpenFSM::OpenFSMPool::getState(int eState)
{
    OpenFSMState* state = 0;
    std::map<std::string, OpenFSMState*>::iterator iter = mapNameState_.begin();
    for (; iter != mapNameState_.end(); iter++)
    {
        state = iter->second;
        if (state->eState_ >= 0 && state->eState_ == eState)
        {
            return state;
        }
    }
    return 0;
}

};