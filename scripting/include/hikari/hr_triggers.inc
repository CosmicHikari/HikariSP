enum struct Zone {
    float start[3];
    float end[3];
}
Zone g_IndoorZones[4];
Zone g_WeatherZone;

public void InitAllZones() {
  g_IndoorZones[0].start = {-4440.0, -200.0, -850.0};
  g_IndoorZones[0].end = {-2400.0, 2400.0, 500.0};
  g_IndoorZones[1].start = {-882.0, 1400.0, -630.0};
  g_IndoorZones[1].end = {-320.0, 3350.0, -285.0};
  g_IndoorZones[2].start = {400.0, 1190.0, -425.0};
  g_IndoorZones[2].end = {790.0, 1410.0, -230.0};
  g_WeatherZone.start = {-3200.0, -4600.0, 1200.0}
  g_WeatherZone.end = {4000.0, 6000.0, 4000.0}
}

// Check if client is indoors
public bool IsClientIndoors(int client) {
  bool inside = false; float dest[3]; GetClientAbsOrigin(client, dest); // set up vars and client origin
  for (int i = 0; i < sizeof(g_IndoorZones); i++) { if (g_IndoorZones[i].start[0] == 0.0 && g_IndoorZones[i].end[0] == 0.0) continue; if (ZoneCheck(dest, g_IndoorZones[i])) inside = true; } // Check if client is in any of the zones
  return inside; // return the result
}

// Check if pos is in Zone
public bool ZoneCheck(float pos[3], Zone z) { return (pos[0] >= z.start[0] && pos[0] <= z.end[0] && pos[1] >= z.start[1] && pos[1] <= z.end[1] && pos[2] >= z.start[2] && pos[2] <= z.end[2]); }

// Use to check if client index is indoors
public bool g_IsPlayerIndoors[MAXPLAYERS+1] = { true }

public void TickAllTriggers() {
  for (int i = 1; i <= MaxClients; i++) {
    if (!IsClientConnected(i) || !IsClientInGame(i)) continue;
    g_IsPlayerIndoors[i] = IsClientIndoors(i);
  }
}

// Damages all entities for <damage> with <type>
void AttackInZone(float damage, int type, Zone z, bool onlyPlayers = false) {
  for (int i = 0; i <= (onlyPlayers ? MaxClients : GetMaxEntities()); i++) if (IsValidEntity(i)) if (HasEntProp(i, Prop_Send, "m_vecOrigin")) {
    float pos[3];
    GetEntPropVector(i, Prop_Send, "m_vecOrigin", pos);
    if (ZoneCheck(pos, z)) AttackEntity(i, damage, type);
  }
}

// Damages <ent> for <damage> with <type>
void AttackEntity(int ent, float damage, int type) { SDKHooks_TakeDamage(ent, 0, 0, damage, type); }