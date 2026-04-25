

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
    event_types_.clear();
  }

  // 析构函数，需保证没有内存泄漏
  ~Memo() {
    // Note: Events are managed by the caller, so we don't delete them here
    events_.clear();
    event_types_.clear();
  }

  // 向备忘录中加入一项事件。传入一个Event指针，需根据实际派生类类型进行具体操作
  void AddEvent(const Event *event) {
    events_.push_back(event);
    
    // Store event type to avoid repeated dynamic_cast
    if (dynamic_cast<const NormalEvent*>(event)) {
      event_types_.push_back(0); // NormalEvent
    } else if (dynamic_cast<const NotifyBeforeEvent*>(event)) {
      event_types_.push_back(1); // NotifyBeforeEvent
    } else if (dynamic_cast<const NotifyLateEvent*>(event)) {
      event_types_.push_back(2); // NotifyLateEvent
    } else {
      event_types_.push_back(-1); // Unknown
    }
    
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

    for (size_t i = 0; i < events_.size(); ++i) {
      const Event* event = events_[i];
      if (event->IsComplete()) {
        continue;
      }

      int deadline = event->GetDeadline();
      int event_type = event_types_[i];
      
      // Handle different event types using stored type
      if (event_type == 0) { // NormalEvent
        if (current_hour_ > deadline) {
          std::cout << event->GetNotification(0) << std::endl;
          const_cast<Event*>(event)->SetComplete();
        }
      }
      else if (event_type == 1) { // NotifyBeforeEvent
        const NotifyBeforeEvent* before_event = static_cast<const NotifyBeforeEvent*>(event);
        int notify_time = before_event->GetNotifyTime();
        if (current_hour_ == deadline - notify_time) {
          std::cout << before_event->GetNotification(0) << std::endl;
        }
        else if (current_hour_ == deadline) {
          std::cout << before_event->GetNotification(1) << std::endl;
          const_cast<NotifyBeforeEvent*>(before_event)->SetComplete();
        }
      }
      else if (event_type == 2) { // NotifyLateEvent
        const NotifyLateEvent* late_event = static_cast<const NotifyLateEvent*>(event);
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
  std::vector<int> event_types_; // 0: NormalEvent, 1: NotifyBeforeEvent, 2: NotifyLateEvent, -1: Unknown
};
#endif

