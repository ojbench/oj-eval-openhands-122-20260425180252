#include <cmath>

#include "memo.hpp"

// 测试样例的个性化信息生成函数
std::string CustomGen(int n) {
  double score = std::pow(0.8, static_cast<double>(n));
  return "\033[35mYour Bookstore project has only " + std::to_string(score) + " points left.\033[0m";
}

// 与题干样例完全一致的测试
void SampleTest() {
  Event *event1 = new NormalEvent{"Pick Rubbish", 3};
  Event *event2 = new CustomNotifyLateEvent{"Complete project Bookstore", 1, 2, CustomGen};
  Event *event3 = new NotifyBeforeEvent("Participate in the final exam", 3, 2);
  Memo memo_demo(3);

  memo_demo.AddEvent(event1);
  memo_demo.AddEvent(event2);
  // Your memo should notify about your project
  memo_demo.Tick();

  memo_demo.AddEvent(event3);
  // Your memo should notify about your final exam
  memo_demo.Tick();

  event1->SetComplete();
  event3->SetComplete();
  // Your memo should notify about your project
  memo_demo.Tick();

  delete event1;
  delete event2;
  delete event3;
}

// 如果调试需要， 你可以自由地在这里添加其他测试点

int main()
{
  SampleTest();
  return 0;
}
