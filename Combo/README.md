# Combo

This program combines all interesting operation modes that the Elegoo Arduino Car can do.
In the mean time, it tries to introduce a non-preemptive way for multi-tasking using
Arduino.

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

  if (op_mode == previous_mode) { return false; }
  Serial.println(mode_text[op_mode]);
  stateChange();
  return true;
}
```
Mode change should happen when an IR remote key listed above in the code is pressed for now. Later this may change, but all modes
should exit and switch to ```STOP_MODE``` when the key ```0``` is pressed.
