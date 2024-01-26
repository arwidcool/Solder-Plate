#ifndef __enums_h__
#define __enums_h__

enum ButtonState
{
  IDLE,
  PRESSED,
  RELEASED
};

enum ButtonKind
{
  UP,
  DOWN,
  BACK,
  SELECT,
  NONE
};

#define KIND_STR(kind) (kind == ButtonKind::UP ? "UP" : kind == ButtonKind::DOWN ? "DOWN"   \
                                                    : kind == ButtonKind::BACK   ? "BACK"   \
                                                    : kind == ButtonKind::SELECT ? "SELECT" \
                                                                                 : "NONE")
#define STATE_STR(state) (state == ButtonState::IDLE ? "IDLE" : state == ButtonState::PRESSED ? "PRESSED" \
                                                                                              : "RELEASED")
#define STATECHANGE_STR(change) (String(STATE_STR(change.from)) + " -> " + STATE_STR(change.to))

#endif