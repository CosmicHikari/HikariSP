// Hikari's FastFire2 - Used for interacting with map logic
char FF2_VER[7] = "2.1.2";
int ff2Ref;

// Used for any time we need to fo a LOT of things in quick succession... Or when we want to do something at random using arrayname[index].fireStr.
enum struct BULKFIRE {
  char fireStr[256];
}

// Startup event - Ensure BulkFire config path exists
void OnBulkfireStartup(const char[] path) {
  char finalPath[PLATFORM_MAX_PATH];
  Format(finalPath, sizeof(finalPath), "%s/BulkFire", path);
  CreateDirectory(finalPath);
}

methodmap FASTFIRE2 {
  // Create Entities with optional name
  // @param classname - The entity class name to be created
  // @param targetname - The name of the created entity
  public int CreateCustomEntity(char[] classname, char[] targetname) {
    int cce = FindEntityByTargetname(targetname, classname);
    if (!IsValidEntity(cce) && ICanSummon(1)) {
      cce = CreateEntityByName(classname);
      if (!IsValidEdict(cce)) { LogError("[FastFire2] Could not create %s, aborting!", classname); return -2; }
      DispatchSpawn(cce);
      DispatchKeyValue(cce, "targetname", targetname);
      ActivateEntity(cce);
    }
    return cce;
  }
  // Hikari's ent_fire interface
  // @param target The target entity - if it contains OnUser1, this will become the entire io string. Example: `FastFire2("OnUser1 Operator,Command,sm_example,0.69,1");`
  // @param input The input to execute
  // @param param Parameters if applicable
  // @param delay The delay before executing
  // @param refire How many times this IO will refire, default 1
  // @param any - Can be anything, or nothing! (%i, %f, %b, %s, etc.)
  public void Execute(const char[] target, const char[] input = "", const char[] param = "", float delay = 0.0, int refire = 1, any ...) {
    int ff2 = EntRefToEntIndex(ff2Ref); // Get the entity index from EntRef
    char buffers[3][128]; // Vars used to format input parameters ( any ... )
    for (int i = 0; i < 3; i++) { strcopy(buffers[i], sizeof(buffers[]), i == 0 ? target : i == 1 ? input : param); }
    for (int i = 0; i < 3; i++) { if (StrContains(buffers[i], "%") != -1) VFormat(buffers[i], sizeof(buffers[]), i == 0 ? target : i == 1 ? input : param, 7); }
    char FireStr[512]; // Support using "OnUser1 MyEntity,MyInput,MyParameter,MyDelay,MyRefireCount"
    FormatEx(FireStr, sizeof(FireStr), (StrContains(target, "OnUser1", true) != -1 ? target : "OnUser1 %s,%s,%s,%f,%i"), buffers[0], buffers[1], buffers[2], delay, refire);
    SetVariantString(FireStr); // Execute the IO
    AcceptEntityInput(ff2, "AddOutput");
    AcceptEntityInput(ff2, "FireUser1");
  }
  // Sets keyvalues on entities, experimentally. Requires the EntIndex, not EntRef.
  public void SetKv(int kvEnt, char[] kv, char[] val) { if (IsValidEntity(kvEnt)) { DispatchKeyValue(kvEnt, kv, val); } }
  // Kills an entity or group of entities using wildcards (*)
  // @param target The target to kill
  // @param time The time to kill in seconds
  public void SetEntityLife(const char[] target, float time) { this.Execute(target, "Kill", _, time); }
}
FASTFIRE2 FastFire2;

// Startup event - Inject entity IO logic
public void OnMapStart() {
  LogMessage("[FastFire2] OnMapStart Received: Injecting FastFire2! (v%s)", FF2_VER);
  int ff2 = FindEntityByTargetname("FastFire2", "info_target"); 
  if (!IsValidEntity(ff2)) {
    ff2 = FastFire2.CreateCustomEntity("info_target", "FastFire2");
    if (!IsValidEntity(ff2)) { SetFailState("Could not inject FastFire2, aborting!!!"); } // If the entity still doesn't exist, abort loading.
  }
  ff2Ref = EntIndexToEntRef(ff2);
  OnFastFire2Ready();
}