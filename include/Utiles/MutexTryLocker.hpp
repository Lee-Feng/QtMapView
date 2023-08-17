#pragma once

#include <QMutex>
namespace utiles {
class MutexTryLocker {
  QMutex *m_;
  bool locked_;
public:
  MutexTryLocker(QMutex *m,int timeout = 0) : m_(m), locked_(m->tryLock(timeout)) {}
  ~MutexTryLocker() { unlock(); }
  bool isLocked() const { return locked_; }
  void unlock(){ if (locked_) {m_->unlock();locked_ = false;} }
};
}
