# Combo

## InfraRed Control

This program uses these keys on the IR remote controller pad to change between operation modes:
```C
bool op_mode_change (IR_Code ircode) {
  const char * mode_text[] = {
    "STOP_MODE",
    "AUTO_MODE",
    "IR_MODE",
    "AVOIDANCE_MODE",
    "TRACKING_MODE",
    "TESTING_MODE",
    "DISTTEST_MODE"
  };
  int previous_mode = op_mode;
  switch (ircode) {
    case IR_0:    op_mode = STOP_MODE; stop_setup(); break;
    case IR_1:    op_mode = AUTO_MODE; auto_run_setup(); break;
    case IR_2:    op_mode = IR_MODE; ir_blink_setup(); break;
    case IR_3:    op_mode = avoidance_setup(); break;
    case IR_4:    op_mode = TRACKING_MODE; tracking_setup(); break;
    case IR_5:    op_mode = TESTING_MODE; break;
    case IR_6:    op_mode = DISTTEST_MODE; break;
  }
```
