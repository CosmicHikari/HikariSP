char TR_VER[7] = "1.1.0";

// Used to set up zones and allow them to tick
enum struct Zone {
  bool onlyPlayers;
  bool shouldTick;
  float start[3];
  float end[3];
  float dmg;
  int dmgType;
  int ticks;
  int tickrate;
  bool canTick() { return this.shouldTick && (++this.ticks % this.tickrate == 0); }
  bool isTriggeredByEntityAt(const float pos[3]) { return (pos[0] >= this.start[0] && pos[0] <= this.end[0] && pos[1] >= this.start[1] && pos[1] <= this.end[1] && pos[2] >= this.start[2] && pos[2] <= this.end[2]); }
  bool isInvalid() { return (this.start[0] == 0.0 && this.end[0] == 0.0 || this.start[1] == 0.0 && this.end[1] == 0.0 || this.start[2] == 0.0 && this.end[2] == 0.0); }
}
Zone g_IndoorZones[4];
Zone g_WeatherZone;
Zone g_FrontRottenburg;

public void InitAllZones() {
  LightLogger(LOGLVL_INFO, "[Triggers @ v%s] Starting up triggers!", TR_VER);
  g_IndoorZones[0].start = {-4440.0, -200.0, -850.0};
  g_IndoorZones[0].end = {-2400.0, 2400.0, 500.0};
  g_IndoorZones[1].start = {-882.0, 1400.0, -630.0};
  g_IndoorZones[1].end = {-320.0, 3350.0, -285.0};
  g_IndoorZones[2].start = {400.0, 1190.0, -425.0};
  g_IndoorZones[2].end = {790.0, 1410.0, -230.0};
  g_WeatherZone.start = {-3200.0, -4600.0, 1200.0}
  g_WeatherZone.end = {4000.0, 6000.0, 4000.0}
  g_FrontRottenburg.start = {-260.0, -574.0, 1024.0}
  g_FrontRottenburg.end = {3600.0, -4500.0, -256.0}
  g_FrontRottenburg.tickrate = 77;
}

// Use to check if client index is indoors
public bool g_IsPlayerIndoors[MAXPLAYERS+1] = { true }

// Check if client is indoors
public bool IsClientIndoors(int client) {
  bool inside = false; float dest[3]; GetClientAbsOrigin(client, dest); // set up vars and client origin
  for (int i = 0; i < sizeof(g_IndoorZones); i++) { if (g_IndoorZones[i].isInvalid()) continue; if (g_IndoorZones[i].isTriggeredByEntityAt(dest)) inside = true; } // Check if client is in any of the zones
  return inside; // return the result
}

// Tick every trigger
public void TickAllTriggers() {
  if (g_IndoorZones[0].canTick()) { for (int i = 1; i <= MaxClients; i++) { if (!IsClientConnected(i) || !IsPlayerAlive(i) || !IsClientInGame(i)) continue; g_IsPlayerIndoors[i] = IsClientIndoors(i); }} // Check if clients are indoors
  if (g_FrontRottenburg.canTick()) { AttackInZone(g_FrontRottenburg.dmg, g_FrontRottenburg.dmgType, g_FrontRottenburg, g_FrontRottenburg.onlyPlayers); } // Attack in front rottenburg area if requested
}

// Damages <ent> for <damage> with <type>
void AttackEntity(int ent, float damage, int type) { SDKHooks_TakeDamage(ent, 0, 0, damage, type); }

// Damages all entities for <damage> with <type> if they are in <zone>, optionally only affect players.
void AttackInZone(float damage, int type, const Zone z, bool onlyPlayers = false) {
  for (int i = 0; i <= (onlyPlayers ? MaxClients : GetMaxEntities()); i++) if (IsValidEntity(i)) if (HasEntProp(i, Prop_Send, "m_vecOrigin")) {
    float pos[3]; GetEntPropVector(i, Prop_Send, "m_vecOrigin", pos);
    if (z.isTriggeredByEntityAt(pos)) AttackEntity(i, damage, type);
  }
}

// Damages all entities for <damage> with <type> if they are in <zone>, optionally only players, at a tick rate of <tickrate>, optionally start and end time
void AttackInZoneDuration(float damage, int type, const Zone z, bool onlyPlayers = false, int tickrate, float startTime = 0.0, float stopTime = 1.0) {
  bool delayed = startTime > 0.0;
  z.dmg = damage; z.dmgType = type; z.onlyPlayers = onlyPlayers; z.tickrate = tickrate; z.shouldTick = !delayed;
  if (delayed) { CreateTimer(startTime, START_ATTACKING, z); }
  CreateTimer(stopTime, STOP_ATTACKING, z);
}
public Action START_ATTACKING(Handle timer, const Zone z) { z.shouldTick = true; return Plugin_Stop; }
public Action STOP_ATTACKING(Handle timer, const Zone z) { z.shouldTick = false; return Plugin_Stop; }