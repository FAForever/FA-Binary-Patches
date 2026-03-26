// Address: 0x765F1C (entry at 0x765ED0)
// Function: Moho::PathQueue::Work - PathQueue work loop, processes path requests with budget
// Source: c:\work\rts\main\code\src\sim\PathQueue.cpp

void __usercall Moho::PathQueue::Work(Moho::PathQueue *this@<ebx>, int *budget@<esi>)
{
  Moho::PathQueue::Impl *mQueue; // eax
  int mPtr; // ecx
  Moho::CAiPathFinder *v4; // edi
  Moho::PathQueue::ImplBase *p_mBase; // edi
  int v6; // eax
  int cpuBudget; // ecx
  Moho::PathQueue::ImplBase *v8; // [esp-4h] [ebp-8h]

  while ( *budget > 0 )
  {
    mQueue = this->mQueue;
    if ( this->mQueue->mBase.mTraveler.mNext == &this->mQueue->mBase.mTraveler )
    {
      if ( (int *)mQueue->mPtr == &mQueue->mHeight )
        return;
      mPtr = mQueue->mPtr;
      if ( mPtr )
        v4 = (Moho::CAiPathFinder *)(mPtr - 4);
      else
        v4 = 0;
      sub_765FE0(&mQueue->mBase, v4, mQueue->mSize);
    }
    p_mBase = &this->mQueue->mBase;
    if ( this->mQueue->mBase.mTraveler.mNext == &this->mQueue->mBase.mTraveler )
      gpg::HandleAssertFailure("!mTraveler.empty()", 169, "c:\\work\\rts\\main\\code\\src\\sim\\PathQueue.cpp");
    v8 = &this->mQueue->mBase;
    this->mQueue->mBase.mBudget = *budget;
    v6 = Moho::PathQueue::WorkOnce(p_mBase, v8);
    cpuBudget = p_mBase->mBudget;
    *budget = cpuBudget;
    if ( v6 == 2 )
    {
      if ( cpuBudget > 0 )
        gpg::HandleAssertFailure("cpuBudget <= 0", 179, "c:\\work\\rts\\main\\code\\src\\sim\\PathQueue.cpp");
    }
    else
    {
      sub_766140(p_mBase, v6 == 1);
    }
  }
}
