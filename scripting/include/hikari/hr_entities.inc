// Hikari's Entity Hooks / Outputs v1.0.0 
// Hook all spawned entities, automatically unhooking them when they poof.
public void OnEntityCreated(int ent, const char[] classname) { SDKHookEx(ent, SDKHook_SpawnPost, OnEntitySpawned); }

// A wild CBaseEntity appeared!
public void OnEntitySpawned(int ent) {
  char classname[64]; GetEntityClassname(ent, classname, sizeof(classname));
  if (strcmp(classname, "func_physbox") == 0) { HookSingleEntityOutput(ent, "OnDamaged", OnEntityDamaged, true); } // func_physbox spawned
}

// Called when entities are damaged
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