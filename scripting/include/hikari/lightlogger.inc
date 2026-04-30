// Hikari's Light Logger v1.1.0
enum {
  LOGLVL_DEBUG = 0,
  LOGLVL_INFO = 1,
  LOGLVL_WARN = 2,
  LOGLVL_ERROR = 3
};

void LightLogger(int logLevel, const char[] logData, any...) {
  char buffer[512]; VFormat(buffer, sizeof(buffer), logData, 3);
  switch (logLevel) {
    case LOGLVL_DEBUG: { LogMessage("[DEBUG]: %s", buffer); }
    case LOGLVL_INFO: { LogMessage("[INFO]: %s", buffer); }
    case LOGLVL_WARN: { LogMessage("[WARN]: %s", buffer); }
    case LOGLVL_ERROR: { LogMessage("[ERROR]: %s", buffer); }
  }
}