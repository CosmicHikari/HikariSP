// Entity Hooks / Outputs
public void OnEntityCreated(int ent, const char[] classname) { SDKHookEx(ent, SDKHook_SpawnPost, OnEntitySpawned); }

// A wild CBaseEntity appeared!
public void OnEntitySpawned(int ent) {
  char classname[64], targetname[128];
  GetEntityClassname(ent, classname, sizeof(classname));
  GetEntPropString(ent, Prop_Data, "m_iName", targetname, sizeof(targetname));
  // func_physbox spawned
  if (strcmp(classname, "func_physbox") == 0) {
    if (strcmp(targetname, "FallingStar") == 0) {
      HookSingleEntityOutput(ent, "OnDamaged", OnEntityDamaged, true);
    }
  }
}

public void OnEntityDamaged(const char[] output, int caller, int activator, float delay) {
  // Falling Star did a thing
  if (Photon.MatchEntName(caller, "FallingStar")) {
    CustomSoundEmitter(SFXArray[3].realPath, 65, false, 0, 1.0, 100);
    Photon.Fire("FallingStar", "Break");
    Photon.Fire("FallingStarSprite", "Kill");
    Photon.Fire("FrontMapShake", "Amplitude", "16");
    Photon.Fire("FrontMapShake", "Frequency", "160");
    Photon.Fire("StarExplosion", "Explode");
    Photon.Fire("FrontMapShake", "StartShake");
    Photon.Fire("StarExplosion", "Kill", _, 0.1);
  }
}