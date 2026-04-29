char TR_VER[7] = "1.2.1";

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
  // Damage players if this trigger does damage.
  void TickDamage() {
    if (this.dmg > 0 && this.canTick()) {
      for (int i = 0; i <= (this.onlyPlayers ? MaxClients : GetMaxEntities()); i++) if (!IsEntityInvalidForTrigger(i)) {
        float pos[3]; GetEntPropVector(i, Prop_Send, "m_vecOrigin", pos);
        if (this.isTriggeredByEntityAt(pos)) AttackEntity(i, this.dmg, this.dmgType);
      }
    }
  }
}
Zone g_IndoorZones[4];
Zone g_WeatherZone;
Zone g_FrontRottenburg;
Zone g_BombPickup;
Zone g_HatchExplode;
Zone g_HatchFire;
Zone g_Special[5];
Zone g_CustomZone;
Zone g_CustomZoneArray[7];

public void InitAllZones() {
  LightLogger(LOGLVL_INFO, "[Triggers @ v%s] Starting up triggers!", TR_VER);
  SetZoneExtents(g_IndoorZones[0], -4440.0, -200.0, -850.0, -2400.0, 2400.0, 500.0);
  SetZoneExtents(g_IndoorZones[1], -882.0, 1400.0, -630.0, -320.0, 3350.0, -285.0);
  SetZoneExtents(g_IndoorZones[2], 400.0, 1190.0, -425.0, 790.0, 1410.0, -230.0);
  SetZoneExtents(g_WeatherZone, -3200.0, -4600.0, 1200.0, 4000.0, 6000.0, 4000.0);
  SetZoneExtents(g_FrontRottenburg, -260.0, -574.0, 1024.0, 3600.0, -4500.0, -256.0);
  SetZoneExtents(g_Special[0], 2500.0, 1550.0, -340.0, 2564.0, 1500.0, -420.0);
  SetZoneExtents(g_Special[1], 700.0, 1940.0, -128.0, 764.0, 2000.0, -192.0);
  SetZoneExtents(g_Special[2], 420.0, 1332.0, -392.0, 470.0, 1290.0, -325.0)
  SetZoneExtents(g_Special[3], 400.0, -1145.0, -16.0, 450.0, -1092.0, -380.0);// Adds tf cond halloween giant for 60s
  SetZoneExtents(g_Special[4], 1950.0, 1272.0, -316.0, 1890.0, 1332.0, -390.0);
  SetZoneExtents(g_BombPickup, -3860.0, -2275.0, -4.0, -3540.0, -1970.0, -120.0);
  SetZoneExtents(g_HatchExplode, -1024.0, 1169.0, 204.0, 1920.0, 1825.0, -372.0);
  SetZoneExtents(g_HatchFire, -1376.0, 1536.0, -112.0, -1520.0, 1680.0, -124.0);
  g_FrontRottenburg.tickrate = 77;
  g_HatchExplode.tickrate = 77;
  g_HatchFire.tickrate = 32;
}

// Used to check if client is an invalid trigger candidate
bool IsClientInvalidForTrigger(int player) { return (!IsClientConnected(player) || !IsPlayerAlive(player) || !IsClientInGame(player)); }

// Used to check if entity is an invalid trigger candidate
bool IsEntityInvalidForTrigger(int ent) { return (IsValidEntity(ent)) ? (!HasEntProp(ent, Prop_Send, "m_vecOrigin")) : true; }

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
  g_FrontRottenburg.TickDamage();
  g_HatchExplode.TickDamage();
  g_HatchFire.TickDamage();
  g_CustomZone.TickDamage();
  for (int i = 0; i < sizeof(g_CustomZoneArray); i++) g_CustomZoneArray[i].TickDamage();
  if (g_IndoorZones[0].canTick()) { for (int i = 1; i <= MaxClients; i++) { if (IsClientInvalidForTrigger(i)) continue; g_IsPlayerIndoors[i] = IsClientIndoors(i); }} // Check if clients are indoors
}

// Damages <ent> for <damage> with <type>
void AttackEntity(int ent, float damage, int type) { SDKHooks_TakeDamage(ent, 0, 0, damage, type); }

// Creates a damaging area over time, for all entities with <damage> and <type> in <zone>, optionally <onlyPlayers>, at <tickrate>, with <start> and <stop> time
// @param damage - The damage to deal
// @param type - The type of damage to deal, must be a valid damagebit
// @param zone - The zone to be set up
// @param onlyPlayers - Only affect players?
// @param tickrate - The rate this trigger attacks. Lower values = faster
// @param startTime - The time to wait before starting, in seconds
// @param stopTime - The time to wait before stopping, in seconds
void SetupAttackZone(float damage, int type, const Zone zone, bool onlyPlayers = false, int tickrate = 33, float startTime = 0.0, float stopTime = 1.0) {
  bool delayed = startTime > 0.0;
  zone.dmg = damage; zone.dmgType = type; zone.onlyPlayers = onlyPlayers; zone.tickrate = tickrate; zone.shouldTick = !delayed;
  if (delayed) { CreateTimer(startTime, START_ATTACKING, zone); }
  CreateTimer(stopTime, STOP_ATTACKING, zone);
}
public Action START_ATTACKING(Handle timer, const Zone zone) { zone.shouldTick = true; return Plugin_Stop; }
public Action STOP_ATTACKING(Handle timer, const Zone zone) { zone.shouldTick = false; return Plugin_Stop; }

// Resets an attack zone to its default values
void RemoveAttackZone(const Zone zone) { zone.dmg = 0.0; zone.dmgType = 0; zone.onlyPlayers = false; StopAttackZone(zone); }

// Stops a zone from ticking manually
void StopAttackZone(const Zone zone) { zone.shouldTick = false; }

// Set a zone's extents
// @param zone - The zone to affect
// @param sx - Start X
// @param sy - Start Y
// @param sz - Start Z
// @param ex - End X
// @param ey - End Y
// @param ez - End Z
void SetZoneExtents(const Zone zone, float sx, float sy, float sz, float ex, float ey, float ez) { zone.start[0] = sx; zone.start[1] = sy; zone.start[2] = sz; zone.end[0] = ex; zone.end[1] = ey; zone.end[2] = ez; LightLogger(LOGLVL_INFO, "[Triggers @ v%s] Set new zone extents %f %f %f to %f %f %f.", TR_VER, zone.start[0], zone.start[1], zone.start[2], zone.end[0], zone.end[1], zone.end[2]); }