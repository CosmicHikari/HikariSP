// Hikari's Photon - Map I/O & Entity Management Interface
char PHOTON_VER[7] = "3.0.5";
int photonRef;

enum struct PHOTON_BULK {
  char fireStr[256];
}

void OnPhotonStartup(const char[] path) {
  char finalPath[PLATFORM_MAX_PATH];
  Format(finalPath, sizeof(finalPath), "%s/Photon", path);
  CreateDirectory(finalPath);
}

methodmap PHOTON {
  public bool CanCreate(int intent) {
    int count = 0;
    int maxEntities = GetMaxEntities();
    for (int i = 1; i <= maxEntities; i++) { if (IsValidEntity(i)) { count++; } }
    return (count + intent) < maxEntities;
  }

  public int Create(char[] classname, char[] targetname) {
    int ent = this.Find(targetname, classname);
    if (!IsValidEntity(ent) && this.CanCreate(1)) {
      ent = CreateEntityByName(classname);
      if (!IsValidEdict(ent)) { LogError("[Photon] Failed to create %s!", classname); return -1; }
      DispatchSpawn(ent);
      DispatchKeyValue(ent, "targetname", targetname);
      ActivateEntity(ent);
    }
    return ent;
  }

  public int Find(char[] targetname, char[] classname, bool caseSensitive = false) {
    char namebuf[128]; int index = -1;
    while((index = FindEntityByClassname(index, classname)) != -1) { 
      GetEntPropString(index, Prop_Data, "m_iName", namebuf, sizeof(namebuf));
      if (strcmp(namebuf, targetname, caseSensitive) == 0) return index;
    }
    return -1;
  }

  public void Fire(const char[] target, const char[] input = "", const char[] param = "", float delay = 0.0, int refire = 1, any ...) {
    int ent = EntRefToEntIndex(photonRef);
    char buffers[3][128];
    for (int i = 0; i < 3; i++) { strcopy(buffers[i], sizeof(buffers[]), i == 0 ? target : i == 1 ? input : param); }
    for (int i = 0; i < 3; i++) { if (StrContains(buffers[i], "%") != -1) VFormat(buffers[i], sizeof(buffers[]), i == 0 ? target : i == 1 ? input : param, 7); }
    char fireStr[512];
    FormatEx(fireStr, sizeof(fireStr), (StrContains(target, "OnUser1", true) != -1 ? target : "OnUser1 %s,%s,%s,%f,%i"), buffers[0], buffers[1], buffers[2], delay, refire);
    SetVariantString(fireStr);
    AcceptEntityInput(ent, "AddOutput");
    AcceptEntityInput(ent, "FireUser1");
  }

  public bool MatchEntName(int ent, const char[] name) {
    bool valid = IsValidEntity(ent);
    if (valid) { char targetname[128]; GetEntPropString(ent, Prop_Data, "m_iName", targetname, sizeof(targetname)); return (strcmp(targetname, name) == 0); }
    return false;
  }

  public void SetKv(int ent, char[] kv, char[] val, any...) {
    char value[256]; strcopy(value, sizeof(value), val);
    if (StrContains(value, "%") != -1) VFormat(value, sizeof(value), val, 5);
    if (IsValidEntity(ent)) { DispatchKeyValue(ent, kv, value); }
  }
  
  public void SetEntityLife(const char[] target, float time) { this.Fire(target, "Kill", _, time); }
}
PHOTON Photon;

public void OnMapStart() {
  LogMessage("[Photon] Initializing... (v%s)", PHOTON_VER);
  int ent = Photon.Find("Photon_Relay", "info_target"); 
  if (!IsValidEntity(ent)) {
    ent = Photon.Create("info_target", "Photon_Relay");
    if (!IsValidEntity(ent)) { SetFailState("Photon could not initialize entity relay!"); }
  }
  photonRef = EntIndexToEntRef(ent);
  OnPhotonReady();
}