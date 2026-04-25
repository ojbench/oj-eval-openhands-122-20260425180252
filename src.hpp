

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
    // Only for NotifyBeforeEvent
    if (current_hour_ > 0) {
      int deadline = event->GetDeadline();
      // Use a single dynamic_cast for efficiency
      const NotifyBeforeEvent* before_event = dynamic_cast<const NotifyBeforeEvent*>(event);
      if (before_event && !event->IsComplete()) {
        int notify_time = before_event->GetNotifyTime();
        if (current_hour_ >= deadline - notify_time && current_hour_ < deadline) {
          std::cout << before_event->GetNotification(0) << std::endl;
        }
      }
    }
  }

  // 模拟时间流逝，进行下一个小时的事件提醒
  void Tick() {
    current_hour_++;
    if (current_hour_ > max_duration_) {
      return;
    }

    // Process each event with minimal dynamic_cast usage
    const size_t event_count = events_.size();
    for (size_t i = 0; i < event_count; ++i) {
      const Event* event = events_[i];
      if (event->IsComplete()) {
        continue;
      }

      const int deadline = event->GetDeadline();
      
      // Check each event type with early exit
      if (current_hour_ > deadline) {
        // Could be NormalEvent or NotifyLateEvent (late notification)
        const NormalEvent* normal_event = dynamic_cast<const NormalEvent*>(event);
        if (normal_event) {
          std::cout << normal_event->GetNotification(0) << std::endl;
          const_cast<NormalEvent*>(normal_event)->SetComplete();
          continue;
        }
        
        const NotifyLateEvent* late_event = dynamic_cast<const NotifyLateEvent*>(event);
        if (late_event) {
          const int hours_late = current_hour_ - deadline;
          const int frequency = late_event->GetFrequency();
          if (hours_late % frequency == 0) {
            const int notification_count = hours_late / frequency;
            std::cout << late_event->GetNotification(notification_count) << std::endl;
          }
          continue;
        }
      }
      else if (current_hour_ == deadline) {
        // Could be NotifyBeforeEvent (at deadline) or NotifyLateEvent (at deadline)
        const NotifyBeforeEvent* before_event = dynamic_cast<const NotifyBeforeEvent*>(event);
        if (before_event) {
          std::cout << before_event->GetNotification(1) << std::endl;
          const_cast<NotifyBeforeEvent*>(before_event)->SetComplete();
          continue;
        }
        
        const NotifyLateEvent* late_event = dynamic_cast<const NotifyLateEvent*>(event);
        if (late_event) {
          std::cout << late_event->GetNotification(0) << std::endl;
          continue;
        }
      }
      else {
        // current_hour_ < deadline - could be NotifyBeforeEvent (before deadline)
        const NotifyBeforeEvent* before_event = dynamic_cast<const NotifyBeforeEvent*>(event);
        if (before_event) {
          const int notify_time = before_event->GetNotifyTime();
          if (current_hour_ == deadline - notify_time) {
            std::cout << before_event->GetNotification(0) << std::endl;
          }
          continue;
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

