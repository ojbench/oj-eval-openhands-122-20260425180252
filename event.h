#ifndef EVENT_H
#define EVENT_H

#include <stdexcept>
#include <string>

// 抽象的事件类，作为各类备忘录事件的基类
class Event {
public:
  Event() = delete;
  // 构造函数，参数为事件的名字以及截止时间
  Event(const std::string &name, int deadline) : name_(name), deadline_(deadline), complete_(false) {}

  // 显示删除拷贝构造/移动构造/拷贝复制/移动赋值函数
  Event(const Event &other) = delete;
  Event(Event &&other) = delete;
  Event &operator=(const Event &other) = delete;
  Event &operator=(Event &&other) = delete;

  // 使用默认析构函数
  virtual ~Event() = default;

  // 获取事件名称
  const std::string &GetName() const { return name_; }

  // 设置事件是否已经完成。由测试程序调用，备忘录实现过程中不允许调用该函数。
  void SetComplete() { complete_ = true; }

  // 查询事件是否完成
  bool IsComplete() const { return complete_; }

  // 查询事件截止时间
  int GetDeadline() const { return deadline_; }

  // 纯虚函数，用于依据事件类型获取提示信息。参数n表示这是第几次提示用户。
  virtual std::string GetNotification(int n) const = 0;

private:
  std::string name_;
  int deadline_;
  bool complete_;
};

// 一般事件对应的类，继承自事件类
class NormalEvent final : public Event {
public:
  NormalEvent(const std::string &name, int deadline) : Event(name, deadline) {}

  // 普通事件的提醒消息重载。参数n必须为0（因为只会提醒1次）
  std::string GetNotification(int n) const override {
    if (n != 0) {
      throw std::runtime_error("Notification argument is invalid for Normal Events!");
    }
    return "Normal Event \"" + GetName() + "\" is over.";
  }
};

// 预先提醒事件对应的类，继承自事件类
class NotifyBeforeEvent final : public Event {
public:
  // 构造函数，除了事件类所需的基本参数外，额外拥有一个notify_time，表示提前的提醒时间
  NotifyBeforeEvent(const std::string &name, int deadline, int notify_time) :
    Event(name, deadline), notify_time_(notify_time) {}

  // 获取预先提醒
  int GetNotifyTime() const { return notify_time_; }

  // 预先提醒事件的提醒消息重载。参数n为0或1。若n=0，表示预先提醒消息；若n=1，表示截止提醒消息。
  std::string GetNotification(int n) const override {
    if (n == 0) {
      return "Notify Before Event \"" + GetName() + "\" is about to end. Please hurry!";
    }
    if (n == 1) {
      return "Notify Before Event \"" + GetName() +"\" is over.";
    }
    throw std::runtime_error("Notification argument is invalid for Notify Before Events!");
  }

private:
  int notify_time_;
};

// 延迟提醒事件对应的类，继承自事件类
class NotifyLateEvent : public Event {
public:
  // 构造函数，除了事件类所需的基本参数外，额外拥有一个frequency，表示延迟提醒频率
  NotifyLateEvent(const std::string &name, int deadline, int frequency) :
    Event(name, deadline), frequency_(frequency) {}

  // 获取延迟提醒频率
  int GetFrequency() const { return frequency_; }

  // 延迟提醒事件的消息重载。参数可以为任何n>=0的数，n=0表示截止提醒，n>0表示第n次延迟提醒。
  std::string GetNotification(int n) const override {
    if (n == 0) {
      return "Notify Late Event \"" + GetName() + "\" is over.";
    }
    if (n > 0) {
      return "Notify Late Event \"" + GetName() + "\" is late for " + std::to_string(frequency_ * n) + " hours. ";
    }
    throw std::runtime_error("Notification argument is invalid for Notify Late Events!");
  }

private:
  int frequency_;
};

// 自定义延迟提醒事件对应的类，继承自延迟提醒事件类
class CustomNotifyLateEvent final : public NotifyLateEvent {
public:
  // 构造函数，与延迟提醒事件参数一致，但额外拥有一个通配指针generator，表示自定义信息生成函数的指针
  CustomNotifyLateEvent(const std::string &name, int deadline, int frequency, std::string (*generator)(int)) :
    NotifyLateEvent(name, deadline, frequency), generator_(generator) {}

  /**
   * 自定义延迟事件的消息重载。参数意义与延迟提醒事件一致，不在此赘述。
   * 生成方式：
   *   1.首先调用NotifyLateEvent类的GetNotification(n)。
   *   2.调用函数指针指向的函数，获取生成的字符串。
   *   3.最终将两部分字符串加起来，得到返回的字符串。
   * TODO(student): 在memo.hpp中实现该函数。
   * @note 如果不按照上述方式生成消息，即便该函数返回值正确，也将扣除实现该函数的20%分数。
   * 调用基类的GetNotification不需要做任何类型转换，若在此部分使用类型转换将视为违规。
   * 若尝试在此部分粘贴延迟提醒事件的提示信息，会被评测机判为 Wrong Answer。
   */
  std::string GetNotification(int n) const override;

private:
  std::string (*generator_)(int);
};
#endif