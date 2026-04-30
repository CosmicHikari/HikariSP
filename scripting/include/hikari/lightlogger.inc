// Hikari's Light Logger v1.2.0
enum {
  LOGLVL_DEBUG = 0,
  LOGLVL_INFO = 1,
  LOGLVL_WARN = 2,
  LOGLVL_ERROR = 3
};

int LL_Threshold = LOGLVL_INFO;
void LightLogger(int logLevel, const char[] logData, any...) {
  if (logLevel < LL_Threshold) return;
  char buffer[512]; VFormat(buffer, sizeof(buffer), logData, 3);
  switch (logLevel) {
    case LOGLVL_DEBUG: { LogMessage("[DEBUG]: %s", buffer); }
    case LOGLVL_INFO: { LogMessage("[INFO]: %s", buffer); }
    case LOGLVL_WARN: { LogMessage("[WARN]: %s", buffer); }
    case LOGLVL_ERROR: { LogMessage("[ERROR]: %s", buffer); }
  }
}