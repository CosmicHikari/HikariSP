char TR_VER[7] = "1.6.0";

// Used to set up zones and allow them to tick
enum struct Zone {
  bool hasStatusEffect;
  bool onlyPlayers;
  bool shouldTick;
  bool onlyOnce;
  float start[3];
  float end[3];
  float dmg;
  int dmgType;
  int team;
  int ticks;
  int tickrate;
  TFCond statuseffect;
  bool canTick() { return !this.isInvalid() && this.shouldTick && (++this.ticks % this.tickrate == 0); }
  bool isTriggeredByEntityAt(const float pos[3]) {
    if (pos[0] < Maths().FloatMin(this.start[0], this.end[0]) || pos[0] > Maths().FloatMax(this.start[0], this.end[0])) return false;
    if (pos[1] < Maths().FloatMin(this.start[1], this.end[1]) || pos[1] > Maths().FloatMax(this.start[1], this.end[1])) return false;
    if (pos[2] < Maths().FloatMin(this.start[2], this.end[2]) || pos[2] > Maths().FloatMax(this.start[2], this.end[2])) return false;
    return true;
  }
  bool isInvalid() { return (this.start[0] == 0.0 && this.end[0] == 0.0 || this.start[1] == 0.0 && this.end[1] == 0.0 || this.start[2] == 0.0 && this.end[2] == 0.0); }
  void Tick() {
    if (this.canTick()) {
      float pos[3];
      for (int i = 0; i < (this.onlyPlayers ? MaxClients : GetMaxEntities()); i++) {
        if (i < MaxClients) {
          if (!(0 < i) || IsClientInvalidForTrigger(i)) continue;
          if (this.team == 0 || (GetClientTeam(i) == this.team)) GetClientAbsOrigin(i, pos);
          // Tick status effects
          if (this.hasStatusEffect && !TF2_IsPlayerInCondition(i, this.statuseffect) && this.isTriggeredByEntityAt(pos)) {
            switch(this.statuseffect) {
              case TFCond_MVMBotRadiowave: { if (GetClientTeam(i) == 3) TF2_AddCondition(i, TFCond_MVMBotRadiowave); }
              default: { if (this.statuseffect != TFCond_MVMBotRadiowave) TF2_AddCondition(i, this.statuseffect); }
            }
          }
        }
        // Tick other entities
        if (i >= MaxClients && !IsEntityInvalidForTrigger(i)) GetEntPropVector(i, Prop_Send, "m_vecOrigin", pos);
        if (!IsEntityInvalidForTrigger(i) && this.isTriggeredByEntityAt(pos)) {
          if (this.dmg > 0) { AttackEntity(i, this.dmg, this.dmgType); }
          if (this.onlyOnce) { SetZoneActive(this, false); }
        }
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
Zone g_Special[6];
Zone g_CustomZone;
Zone g_CustomZoneArray[7];
Zone g_Lights[13];

public void InitAllZones() {
  LightLogger(LOGLVL_INFO, "[Triggers @ v%s] Starting up triggers!", TR_VER);
  SetZoneExtents(g_IndoorZones[0], -4440.0, -200.0, -850.0, -2400.0, 2400.0, 500.0);
  SetZoneExtents(g_IndoorZones[1], -882.0, 1400.0, -630.0, -320.0, 3350.0, -285.0);
  SetZoneExtents(g_IndoorZones[2], 400.0, 1190.0, -425.0, 790.0, 1410.0, -230.0);
  SetZoneExtents(g_WeatherZone, -3200.0, -4600.0, 1200.0, 4000.0, 6000.0, 4000.0);
  SetZoneExtents(g_FrontRottenburg, -260.0, -574.0, 1024.0, 3600.0, -4500.0, -256.0);
  SetZoneExtents(g_Special[0], 2500.0, 1550.0, -340.0, 2564.0, 1500.0, -420.0);
  SetZoneExtents(g_Special[1], 700.0, 1940.0, -128.0, 764.0, 2000.0, -192.0);
  SetZoneExtents(g_Special[2], 420.0, 1332.0, -392.0, 470.0, 1290.0, -325.0);
  SetZoneExtents(g_Special[3], 400.0, -1145.0, -16.0, 450.0, -1092.0, -380.0);// Adds tf cond halloween giant for 60s
  SetZoneExtents(g_Special[4], 1950.0, 1272.0, -316.0, 1890.0, 1332.0, -390.0);
  SetZoneExtents(g_BombPickup, -3860.0, 1970.0, 24.0, -3524.0, 2280.0, -128.0);
  SetZoneExtents(g_HatchExplode, -1024.0, 1169.0, 204.0, 1920.0, 1825.0, -372.0);
  SetZoneExtents(g_HatchFire, -1376.0, 1536.0, -112.0, -1520.0, 1680.0, -124.0);
  SetZoneExtents(g_Lights[0], -564.0, 992.0, 0.0, -694.0, 1120.0, -224.0);
  SetZoneExtents(g_Lights[1], -564.0, 1575.0, 69.0, -694.0, 1704.0, -224.0);
  SetZoneExtents(g_Lights[2], -564.0, 2304.0, 69.0, -694.0, 2427.0, -204.0);
  SetZoneExtents(g_Lights[3], 367.0, 2625.0, -20.0, 494.0, 2756.0, -192.0);
  SetZoneExtents(g_Lights[4], 1108.0, 2680.0, 10.0, 1238.0, 2554.0, -192.0);
  SetZoneExtents(g_Lights[5], 1108.0, 2068.0, 10.0, 1238.0, 1940.0, -192.0);
  SetZoneExtents(g_Lights[6], 1008.0, 1458.0, 10.0, 880.0, 1586.0, -192.0);
  SetZoneExtents(g_Lights[7], 1038.0, 648.0, -148.0, 900.0, 772.0, -300.0);
  SetZoneExtents(g_Lights[8], 367.0, 1710.0, -224.0, 492.0, 1838.0, -384.0);
  SetZoneExtents(g_Lights[9], 1704.0, 1780.0, -200.0, 1830.0, 1960.0, -330.0);
  SetZoneExtents(g_Lights[10], 2230.0, 1512.0, -243.0, 2356.0, 1384.0, -384.0);
  SetZoneExtents(g_Lights[11], 2252.0, 967.0, -240.0, 2380.0, 840.0, -376.0);
  g_FrontRottenburg.tickrate = 77;
  g_HatchExplode.tickrate = 77;
  g_HatchFire.tickrate = 32;
  g_BombPickup.tickrate = 32;
  g_BombPickup.onlyPlayers = true;
  for (int i = 0; i < sizeof(g_Lights); i++) { g_Lights[i].tickrate = 33; g_Lights[i].statuseffect = TFCond_MVMBotRadiowave; g_Lights[i].hasStatusEffect = true; }
  for (int i = 0; i < sizeof(g_Special); i++) { g_Special[i].onlyPlayers = true; g_Special[i].team = 2; }
  g_Special[0].statuseffect = TFCond_OnFire;
  g_Special[3].statuseffect = TFCond_HalloweenGiant;
  g_Special[4].statuseffect = TFCond_MarkedForDeath;
  g_WeatherZone.tickrate = 70;
}

// Used to check if client is an invalid trigger candidate
bool IsClientInvalidForTrigger(int player) { return (IsClientInGame(player) ? !IsPlayerAlive(player) : true); }

// Used to check if entity is an invalid trigger candidate
bool IsEntityInvalidForTrigger(int ent) { return (IsValidEntity(ent)) ? (!HasEntProp(ent, Prop_Send, "m_vecOrigin")) : true; }

// Use to check if client index is indoors
public bool g_IsPlayerIndoors[MAXPLAYERS+1] = { true }

// Check if client is indoors
public bool IsClientIndoors(int client) {
  if (IsClientInvalidForTrigger(client)) return false;
  bool inside = false; float dest[3]; GetClientAbsOrigin(client, dest); // set up vars and client origin
  for (int i = 0; i < sizeof(g_IndoorZones); i++) { if (g_IndoorZones[i].isInvalid()) continue; if (g_IndoorZones[i].isTriggeredByEntityAt(dest)) inside = true; } // Check if client is in any of the zones
  return inside; // return the result
}

// Tick every trigger
public void TickAllTriggers() {
  g_FrontRottenburg.Tick();
  g_HatchExplode.Tick();
  g_HatchFire.Tick();
  g_CustomZone.Tick();
  g_WeatherZone.Tick();
  g_BombPickup.Tick();
  for (int i = 0; i < sizeof(g_CustomZoneArray); i++) g_CustomZoneArray[i].Tick();
  for (int i = 0; i < sizeof(g_Lights); i++) g_Lights[i].Tick();
  if (g_IndoorZones[0].canTick()) { for (int i = 1; i <= MaxClients; i++) { if (IsClientInvalidForTrigger(i)) continue; g_IsPlayerIndoors[i] = IsClientIndoors(i); }} // Check if clients are indoors

  // Manually tick special entities that should disable after providing their effect.
  for (int i = 0; i < sizeof(g_Special); i++) {
    if (!g_Special[i].canTick()) continue;
    for (int c = 1; c <= MaxClients; i++) {
      if (IsClientInvalidForTrigger(c)) continue;
      float dest[3]; GetClientAbsOrigin(c, dest);
      if (g_Special[i].isTriggeredByEntityAt(dest) && TF2_IsPlayerInCondition(c, g_Special[i].statuseffect)) SetZoneActive(g_Special[i], false);
    }
  }
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
// @param stopTime - The time to wait before stopping, in seconds Use a value higher than 9999.0 to make this remain active until manually set inactive.
void SetupAttackZone(float damage, int type, Zone zone, bool onlyPlayers = false, int tickrate = 33, float startTime = 0.0, float stopTime = 1.0) {
  bool delayed = startTime > 0.0;
  zone.dmg = damage; zone.dmgType = type; zone.onlyPlayers = onlyPlayers; zone.tickrate = tickrate < 1 ? 1 : tickrate; zone.shouldTick = !delayed;
  if (delayed) { CreateTimer(startTime, START_ATTACKING, zone); }
  if (stopTime < 9999.0) CreateTimer(stopTime, STOP_ATTACKING, zone);
}
public Action START_ATTACKING(Handle timer, Zone zone) { SetZoneActive(zone, true); return Plugin_Stop; }
public Action STOP_ATTACKING(Handle timer, Zone zone) { SetZoneActive(zone, false); return Plugin_Stop; }

// Resets an attack zone to its default values
void RemoveAttackZone(Zone zone) { zone.dmg = 0.0; zone.dmgType = 0; zone.onlyPlayers = false; SetZoneActive(zone, false); }

// Sets whether a zone is enabled or disabled
void SetZoneActive(Zone zone, bool active) { zone.shouldTick = active; }

// Set a zone's extents
// @param zone - The zone to affect
// @param sx - Start X
// @param sy - Start Y
// @param sz - Start Z
// @param ex - End X
// @param ey - End Y
// @param ez - End Z
void SetZoneExtents(Zone zone, float sx, float sy, float sz, float ex, float ey, float ez) { zone.start[0] = sx; zone.start[1] = sy; zone.start[2] = sz; zone.end[0] = ex; zone.end[1] = ey; zone.end[2] = ez; LightLogger(LOGLVL_DEBUG, "[Triggers @ v%s] Set new zone extents %f %f %f to %f %f %f.", TR_VER, zone.start[0], zone.start[1], zone.start[2], zone.end[0], zone.end[1], zone.end[2]); }