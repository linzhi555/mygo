#pragma once

#define INIT_TEST std::vector<void (*)()> _gloal_tests

// 定义函数的宏，自动注册函数指针
#define TEST(name)                                                     \
  void test_##name();                                                  \
  class test_##name##_registrar {                                      \
   public:                                                             \
    test_##name##_registrar() { _gloal_tests.push_back(test_##name); } \
  };                                                                   \
  test_##name##_registrar test_##name##_reg;                           \
  void test_##name()

#define RUN_TEST                  \
  do {                            \
    for (auto f : _gloal_tests) { \
      f();                        \
    }                             \
  } while (0)
