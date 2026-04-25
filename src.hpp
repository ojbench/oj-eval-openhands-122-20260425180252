

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

struct EventInfo {
  const Event* event;
  int type; // 0: NormalEvent, 1: NotifyBeforeEvent, 2: NotifyLateEvent
  int extra_info; // For NotifyBeforeEvent: notify_time, For NotifyLateEvent: frequency
};

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
    EventInfo info;
    info.event = event;
    
    // Determine event type and store extra info
    const NormalEvent* normal_event = dynamic_cast<const NormalEvent*>(event);
    if (normal_event) {
      info.type = 0;
      info.extra_info = 0;
    } else {
      const NotifyBeforeEvent* before_event = dynamic_cast<const NotifyBeforeEvent*>(event);
      if (before_event) {
        info.type = 1;
        info.extra_info = before_event->GetNotifyTime();
        
        // Check if this event should have already triggered a notification
        if (current_hour_ > 0 && !event->IsComplete()) {
          int deadline = event->GetDeadline();
          if (current_hour_ >= deadline - info.extra_info && current_hour_ < deadline) {
            std::cout << before_event->GetNotification(0) << std::endl;
          }
        }
      } else {
        const NotifyLateEvent* late_event = dynamic_cast<const NotifyLateEvent*>(event);
        if (late_event) {
          info.type = 2;
          info.extra_info = late_event->GetFrequency();
        } else {
          info.type = -1;
          info.extra_info = 0;
        }
      }
    }
    
    events_.push_back(info);
  }

  // 模拟时间流逝，进行下一个小时的事件提醒
  void Tick() {
    current_hour_++;
    if (current_hour_ > max_duration_) {
      return;
    }

    // Process each event without any dynamic_cast
    const size_t event_count = events_.size();
    for (size_t i = 0; i < event_count; ++i) {
      const EventInfo& info = events_[i];
      const Event* event = info.event;
      
      if (event->IsComplete() || info.type == -1) {
        continue;
      }

      const int deadline = event->GetDeadline();
      
      if (info.type == 0) { // NormalEvent
        if (current_hour_ > deadline) {
          std::cout << event->GetNotification(0) << std::endl;
          const_cast<Event*>(event)->SetComplete();
        }
      }
      else if (info.type == 1) { // NotifyBeforeEvent
        const int notify_time = info.extra_info;
        if (current_hour_ == deadline - notify_time) {
          std::cout << event->GetNotification(0) << std::endl;
        }
        else if (current_hour_ == deadline) {
          std::cout << event->GetNotification(1) << std::endl;
          const_cast<Event*>(event)->SetComplete();
        }
      }
      else if (info.type == 2) { // NotifyLateEvent
        const int frequency = info.extra_info;
        if (current_hour_ == deadline) {
          std::cout << event->GetNotification(0) << std::endl;
        }
        else if (current_hour_ > deadline) {
          const int hours_late = current_hour_ - deadline;
          if (hours_late % frequency == 0) {
            const int notification_count = hours_late / frequency;
            std::cout << event->GetNotification(notification_count) << std::endl;
          }
        }
      }
    }
  }

 private:
  int current_hour_;
  int max_duration_;
  std::vector<EventInfo> events_;
};
#endif

