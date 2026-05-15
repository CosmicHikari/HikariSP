// Hikari's Entity Hooks / Outputs v1.0.0 

// Hook all spawned entities, automatically unhooking them when they poof.
public void OnEntityCreated(int ent, const char[] classname) { SDKHookEx(ent, SDKHook_SpawnPost, OnEntitySpawned); }

// A wild CBaseEntity appeared!
public void OnEntitySpawned(int ent) {
  char classname[64]; GetEntityClassname(ent, classname, sizeof(classname)); // Who's that Entity???
  if (strcmp(classname, "func_physbox") == 0) { HookSingleEntityOutput(ent, "OnDamaged", OnEntityDamaged, false); } // func_physbox damaged
  if (strcmp(classname, "path_track") == 0) { HookSingleEntityOutput(ent, "OnPass", OnPassTrack, false); } // path_track passed
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
  // Mini Meteor made landfall!
  if (Photon.MatchEntName(caller, "FB.MiniMeteor")) OnMeteoriteImpact();
}

// Called when a path_track has been passed
public void OnPassTrack(const char[] output, int caller, int activator, float delay) {
  // Meteorite made landfall!
  if (Photon.MatchEntName(caller, "MeteoritePath02")) OnMeteoriteImpact();
}

// The dinosaurs are extinct! (again)
public void OnMeteoriteImpact() {
  Photon.Fire("ShakeGlobal", "StartShake"); Photon.Fire("ShakeGlobal", "StopShake", _, 0.5);
  CustomSoundEmitter(SFXArray[124].realPath, SFXArray[124].SNDLVL, false, 0, 1.0, 100);
  Photon.Fire("FB.Meteorite00", "Kill"); Photon.Fire("FB.MiniMeteor", "Kill");
  Photon.Fire("MeteoritePath*", "Kill");
  Photon.Fire("FB.MeteoriteTrain", "Kill");
  SetupAttackZone(50000.0, DMG_CRUSH, g_FrontRottenburg, _, 3, _, 0.5);
}