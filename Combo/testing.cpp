#include "testing.h"

// Individual function testing
enum {
  TEST_LEFT, TEST_RIGHT
} test_mode = TEST_LEFT;

void testing_loop (IR_Code ircode) {
  switch(test_mode) {
    case TEST_LEFT:
      switch (ircode) {
        case IR_UP:     left_fore(); right_stop(); break;
        case IR_DOWN:   left_back(); right_stop(); break;
        case IR_RIGHT:  left_stop(); test_mode = TEST_RIGHT; return;
      }
      break;
    case TEST_RIGHT:
      switch (ircode) {
        case IR_UP:     left_stop(); right_fore(); break;
        case IR_DOWN:   left_stop(); right_back(); break;
        case IR_LEFT:   right_stop(); test_mode = TEST_LEFT; return;
      }
      break;
  }
  start_car();
}
