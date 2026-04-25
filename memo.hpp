#ifndef MEMO_HPP
#define MEMO_HPP

#include <iostream>
#include <vector>
#include "event.h"

inline std::string CustomNotifyLateEvent::GetNotification(int n) const {
  // Call base class GetNotification first
  std::string base_notification = NotifyLateEvent::GetNotification(n);
  // Call the custom generator function
  std::string custom_message = generator_(n);
  // Combine both parts
  return base_notification + custom_message;
}

class Memo {
 public:
  // 显示删除默认构造函数
  Memo() = delete;

  // 构造函数，参数duration表示需要模拟第1~duration小时的备忘录
  Memo(int duration) : current_hour_(0), max_duration_(duration) {
    events_.clear();
  }

  // 析构函数，需保证没有内存泄漏
  ~Memo() {
    // Note: Events are managed by the caller, so we don't delete them here
    events_.clear();
  }

  // 向备忘录中加入一项事件。传入一个Event指针，需根据实际派生类类型进行具体操作
  void AddEvent(const Event *event) {
    events_.push_back(event);
    
    // Check if this event should have already triggered a notification
    int deadline = event->GetDeadline();
    
    const NotifyBeforeEvent* before_event = dynamic_cast<const NotifyBeforeEvent*>(event);
    if (before_event && !event->IsComplete()) {
      int notify_time = before_event->GetNotifyTime();
      if (current_hour_ >= deadline - notify_time && current_hour_ < deadline) {
        std::cout << before_event->GetNotification(0) << std::endl;
      }
    }
  }

  // 模拟时间流逝，进行下一个小时的事件提醒
  void Tick() {
    current_hour_++;
    if (current_hour_ > max_duration_) {
      return;
    }

    for (const Event* event : events_) {
      if (event->IsComplete()) {
        continue;
      }

      int deadline = event->GetDeadline();
      
      // Handle different event types
      const NormalEvent* normal_event = dynamic_cast<const NormalEvent*>(event);
      const NotifyBeforeEvent* before_event = dynamic_cast<const NotifyBeforeEvent*>(event);
      const NotifyLateEvent* late_event = dynamic_cast<const NotifyLateEvent*>(event);

      if (normal_event && current_hour_ > deadline) {
        // NormalEvent: notify once when deadline passes
        std::cout << normal_event->GetNotification(0) << std::endl;
        const_cast<NormalEvent*>(normal_event)->SetComplete();
      }
      else if (before_event) {
        // NotifyBeforeEvent: notify before deadline and at deadline
        int notify_time = before_event->GetNotifyTime();
        if (current_hour_ == deadline - notify_time) {
          std::cout << before_event->GetNotification(0) << std::endl;
        }
        else if (current_hour_ == deadline) {
          std::cout << before_event->GetNotification(1) << std::endl;
          const_cast<NotifyBeforeEvent*>(before_event)->SetComplete();
        }
      }
      else if (late_event) {
        // NotifyLateEvent: notify at deadline and periodically after
        if (current_hour_ == deadline) {
          std::cout << late_event->GetNotification(0) << std::endl;
        }
        else if (current_hour_ > deadline) {
          int hours_late = current_hour_ - deadline;
          int frequency = late_event->GetFrequency();
          if (hours_late % frequency == 0) {
            int notification_count = hours_late / frequency;
            std::cout << late_event->GetNotification(notification_count) << std::endl;
          }
        }
      }
    }
  }

 private:
  int current_hour_;
  int max_duration_;
  std::vector<const Event*> events_;
};
#endif