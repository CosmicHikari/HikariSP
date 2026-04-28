char SUDO_VER[8] = "1.1.2";
/** This is the plugin netcode. This handles literally everything. */
void sudo(int task) {
  LightLogger(LOGLVL_DEBUG, "Calling sudo with %i", task);
  switch (task) {
    //When the map is complete
    case 0: {
      CPrintToChatAll(core.tacobell ? "WOW, YOU DID IT! The server will restart in 30 seconds, prepare to do it again! LULW" : "{darkviolet}[{forestgreen}CORE{darkviolet}] {white}YOU HAVE SUCCESSFULLY COMPLETED PROF'S ASS ! THE SERVER WILL RESTART IN 10 SECONDS.");
      CreateTimer(10.0, TimedOperator, 100);
    }
    //Prepare yourself!
    case 1: {
      WaveSystem().init();
    }
    case 2 : { WeatherManager.canTornado = true; WaveSystem().SetActive(true); WeatherManager.IssueTornadoWarning(); }
    case 2100 : { WeatherManager.Dissipate(); }
    case 2200: {
      int target = FastFire2.CreateCustomEntity("info_target", "ImADumbass");
      SetVariantString("OnUser2 weather.LightningLaser:TurnOn::0.0:1");
      AcceptEntityInput(target, "AddOutput");
      SetVariantString("OnUser2 weather.LightningLaser:TurnOff::0.1:1");
      AcceptEntityInput(target, "AddOutput");
    }
    case 2201: { FastFire2.Execute("ImADumbass", "FireUser2"); }
    case 2300: { FastFire2.Execute("ImADumbass", "Kill"); }
    case 2500: {
      int l1 = FastFire2.CreateCustomEntity("env_laser", "SpLaser00");
      int l2 = FastFire2.CreateCustomEntity("info_target", "SpLaser00TGT");
      TeleportEntity(l1, {-1503.445923, 1369.923584, -45.968681});
      TeleportEntity(l2, {-1503.445923, 1369.923584, 50.000000});
      FastFire2.SetKv(l1, "LaserTarget", "SpLaser00TGT");
      FastFire2.SetKv(l1, "rendercolor", "255 255 0");
      FastFire2.SetKv(l1, "width", "20");
      FastFire2.SetKv(l1, "texture", "sprites/laserbeam.spr");
      FastFire2.SetKv(l1, "NoiseAmplitude", "1");
    }
    case 2501: {
      float fStart[3] = {-1500.0, 1375.0, -50.0}; float fEnd[3] = {-1500.0, 1375.0, 75.0};
      int ent = CreateEntityByName("env_beam");
      if (ent != -1) {
          TeleportEntity(ent, fStart, NULL_VECTOR, NULL_VECTOR);
          SetEntityModel(ent, "sprites/laserbeam.spr"); // This is where you would put the texture, ie "sprites/laser.vmt" or whatever.
          SetEntPropVector(ent, Prop_Data, "m_vecEndPos", fEnd);
          DispatchKeyValue(ent, "targetname", "beam2");
          DispatchKeyValue(ent, "rendercolor", "255 255 255");
          DispatchKeyValue(ent, "renderamt", "100");
          DispatchSpawn(ent);
          SetEntPropFloat(ent, Prop_Data, "m_fWidth", 4.0); // how big the beam will be, i.e "4.0"
          SetEntPropFloat(ent, Prop_Data, "m_fEndWidth", 4.0); // same as above
          ActivateEntity(ent);
          AcceptEntityInput(ent, "TurnOn");
      }
    }
    case 2550: { FastFire2.Execute("SpLaser00", "TurnOn"); }
    case 3000: {
    float flStart[3] = {-1500.0, 1375.0, -50.0};
    int ent = FindEntityByTargetname("phys_model", "generic_actor") == -1 ? CreateEntityByName("generic_actor") : -1;
    if (ent != -1) {
      DispatchKeyValue(ent, "targetname", "phys_model");
      DispatchKeyValue(ent, "model", "models/player/heavy.mdl");
      DispatchKeyValue(ent, "rendermode", "10");
      DispatchKeyValue(ent, "movetype", "4");
      DispatchSpawn(ent);
      ActivateEntity(ent);
      TeleportEntity(ent, flStart, NULL_VECTOR, NULL_VECTOR);
      int ent2 = FindEntityByTargetname("Spec.Burrito", "prop_dynamic");
      TeleportEntity(ent2, flStart, NULL_VECTOR, NULL_VECTOR);
      FastFire2.Execute("Spec.Burrito", "SetParent", "phys_model");
    }
 }
 case 3001: {
  int ent = FindEntityByTargetname("phys_model", "generic_actor");
  if (ent == -1) { ThrowError("Entity not found!!!"); return; }
  SetEntityMoveType(ent, MOVETYPE_FLY);
  SetEntPropFloat(ent, Prop_Data, "m_flGravity", 0.0);
  SetEntPropVector(ent, Prop_Data, "m_vecVelocity", {0.0,0.0,-15.0});
  TeleportEntity(ent, NULL_VECTOR, NULL_VECTOR, {0.0,0.0,-15.0});
 }
 case 3002:{
  for (int i = 0; i <= sizeof(WBLasers[0]); i++) {
    int ent = CreateEntityByName("env_beam");
    if (ent != -1) {
      TeleportEntity(ent, WBLasers[0][i].sPos, NULL_VECTOR, NULL_VECTOR);
      SetEntityModel(ent, "sprites/laserbeam.spr"); // This is where you would put the texture, ie "sprites/laser.vmt" or whatever.
      SetEntPropVector(ent, Prop_Data, "m_vecEndPos", WBLasers[0][i].ePos);
      DispatchKeyValue(ent, "targetname", WBLasers[0][i].tName);
      DispatchKeyValue(ent, "rendercolor", WBLaserColors[0].cRGB);
      DispatchKeyValue(ent, "renderamt", "100");
      DispatchSpawn(ent);
      SetEntPropFloat(ent, Prop_Data, "m_fWidth", 7.0); // how big the beam will be, i.e "4.0"
      SetEntPropFloat(ent, Prop_Data, "m_fEndWidth", 7.0); // same as above
      ActivateEntity(ent);
      AcceptEntityInput(ent, "TurnOn");
    }
  }
 }
 case 3003:{
  for (int i = 0; i <= sizeof(WBLasers[1]); i++) {
    int ent = CreateEntityByName("env_beam");
    if (ent != -1) {
      TeleportEntity(ent, WBLasers[1][i].sPos, NULL_VECTOR, NULL_VECTOR);
      SetEntityModel(ent, "sprites/laserbeam.spr"); // This is where you would put the texture, ie "sprites/laser.vmt" or whatever.
      SetEntPropVector(ent, Prop_Data, "m_vecEndPos", WBLasers[1][i].ePos);
      DispatchKeyValue(ent, "targetname", WBLasers[1][i].tName);
      DispatchKeyValue(ent, "rendercolor", WBLaserColors[1].cRGB);
      DispatchKeyValue(ent, "renderamt", "100");
      DispatchSpawn(ent);
      SetEntPropFloat(ent, Prop_Data, "m_fWidth", 7.0); // how big the beam will be, i.e "4.0"
      SetEntPropFloat(ent, Prop_Data, "m_fEndWidth", 7.0); // same as above
      ActivateEntity(ent);
      AcceptEntityInput(ent, "TurnOn");
    }
  }
 }
 case 3004:{
  for (int i = 0; i <= sizeof(WBLasers[2]); i++) {
    int ent = CreateEntityByName("env_beam");
    if (ent != -1) {
      TeleportEntity(ent, WBLasers[2][i].sPos, NULL_VECTOR, NULL_VECTOR);
      SetEntityModel(ent, "sprites/laserbeam.spr"); // This is where you would put the texture, ie "sprites/laser.vmt" or whatever.
      SetEntPropVector(ent, Prop_Data, "m_vecEndPos", WBLasers[2][i].ePos);
      DispatchKeyValue(ent, "targetname", WBLasers[2][i].tName);
      DispatchKeyValue(ent, "rendercolor", WBLaserColors[2].cRGB);
      DispatchKeyValue(ent, "renderamt", "100");
      DispatchSpawn(ent);
      SetEntPropFloat(ent, Prop_Data, "m_fWidth", 7.0); // how big the beam will be, i.e "4.0"
      SetEntPropFloat(ent, Prop_Data, "m_fEndWidth", 7.0); // same as above
      ActivateEntity(ent);
      AcceptEntityInput(ent, "TurnOn");
    }
  }
 }
  //Force Tornado
    case 3: {
      if (WeatherManager.CanFormTornado()) {
        CreateTimer(0.1, TimedOperator, 41);
        PrintCenterTextAll("OH NOES... PREPARE YOUR ANUS!");
      } else PrintToServer("Error spawning manual tornado... Perhaps we are not in a wave, tornadoes are banished, or a tornado has already spawned???");
      return;
    }
  //Signal that previous boss should spawn.
    case 4: {
      core.waveFlags--;
    }
  //Signal that a boss should spawn
    case 5: {
      if (core.waveFlags < 0) core.waveFlags = 0;
      switch (core.waveFlags) {
        case 0: {
          PrintToChatAll("Caught unhandled exception: core.waveFlags 0 but operator 5 was invoked.");
          return;
        }
        case 1: {
          BossHandler.OnSpawned(0);
        }
        case 2: {
          BossHandler.OnSpawned(1);
        }
      }
    }
  //Signal that next boss should spawn
    case 6: {
      core.waveFlags++;
    }
  //Signal to fastforward boss spawn.
    case 7: {
      core.waveFlags = 2;
      if (WaveSystem().GetWave() == 8 && !WaveSystem().IsTacoBell()) {
        sudo(1001);
        CreateTimer(0.0, TimedOperator, 8);
      }
    }
  //When a tornado intersects a tank.
    case 8: {
      FastFire2.Execute("FB.FakeTankSpawner", "ForceSpawn", "", 0.1);
    }
    case 9 : { Bombs.OnBombReady(); }
  //Client was Sacrificed.
    case 10: {
      core.sacrificedByClient = true;
    }
  //Damage relay took damage
    case 11: {
      FastFire2.Execute("TankRelayDMG", "Enable", "", 0.0);
      FastFire2.Execute("TankRelayDMG", "Disable", "", 0.5);
    }
  //dmg relay was killed
    case 12: {
      FastFire2.Execute("tank_boss", "SetHealth", "1", 0.0);
      FastFire2.Execute("TankRelayDMG", "Enable", "", 0.1);
      FastFire2.Execute("TankRelayDMG", "Enable", "", 0.5);
      FastFire2.Execute("TankRelayDMG", "Enable", "", 1.0);
      FastFire2.Execute("TankRelayDMG", "Disable", "", 60.0);
    }
  //Tank Destroyed (+1), includes disabling onslaughter.
    case 13: {
      switch (core.waveFlags) {
        case 0: {
          CPrintToChatAll("{darkviolet}[{forestgreen}CORE{darkviolet}] {white}A tank has been destroyed. ({limegreen}+1 pt{white})");
          ass_sacPoints++;
        }
        case 1: {
          core.waveFlags = 0;
          CPrintToChatAll("{darkviolet}[{forestgreen}CORE{darkviolet}] {red}ONSLAUGHTER {white}has been destroyed. ({limegreen}+25 pts{white})");
          BossHandler.OnKilled();
        }
        case 2: {
          CPrintToChatAll("{darkviolet}[{forestgreen}CORE{darkviolet}] {red}SEPHIROTH {white}has been destroyed. ({limegreen}+100 pts{white})");
          FastFire2.Execute("TankRelayDMG", "Enable", "", 1.0);
          FastFire2.Execute("TankRelayDMG", "Disable", "", 60.0);
          FastFire2.Execute("FB.Sephiroth", "Disable", "", 0.0);
          FastFire2.Execute("SephTrain", "TeleportToPathTrack", "Seph01", 0.0);
          FastFire2.Execute("SephTrain", "Stop", "", 0.0);
          SephBoss[0].shouldNuke = false;
          ass_sacPoints += 100;
          core.waveFlags = 0;
          WeatherManager.canTornado = false;
          BossHandler.OnKilled();
        }
      }
      return;
    }
  //Bomb Reset (+5)
    case 14: {
      core.bombReset = true;
      ass_sacPoints += 5;
    }
  //Bomb Deployed
    case 15: {
      FastFire2.Execute("FB.PayloadWarning", "Disable", "", 0.0);
      if (WaveSystem().IsActive()) {
        if (g_BombIndex == 69) {
          Bombs.explode(7, false);
          BombRegistry[0].stateMax = 64;
          CreateTimer(5.0, TimedOperator, 99);
        }
        Bombs.explode(Bombs.getCurBombID(), true);
        BombRegistry[0].stateMax += 10;
        if (g_BombIndex >= BombRegistry[0].stateMax) return;
        else g_BombIndex += 2;
      }
      FastFire2.SetEntityLife("BombPush", 1.0);
      FastFire2.SetEntityLife("BombPushRegen", 1.0);
      FastFire2.SetEntityLife("BombTrain0x*", 1.0);
      FastFire2.SetEntityLife("BombTrainDamage", 1.0);
      return;
    }
  //Tank deployed its bomb or boss reached hatch
    case 16: {
      sudo(18);
      CPrintToChatAll("{darkviolet}[{fartsyred}CORE{darkviolet}] {white}A tank has deployed its bomb!");
      return;
    }
    case 17 : {
      sudo(18);
      CPrintToChatAll("{darkviolet}[{fartsyred}CORE{darkviolet}] {white}A robot has deployed its bomb!");
      return;
    }
    case 18: {
      if(!WaveSystem().IsActive()) {
        CPrintToChatAll("{fartsyred} PLEASE DO NOT ATTEMPT TO USE OPERATOR 18 WITHOUT A WAVE.");
        return;
      }
      FastFire2.Execute("bots_win", "RoundWin", "", 1.0);
      FastFire2.Execute("hatch_magnet_pit", "Enable", "", 1.0);
      FastFire2.Execute("hatch_explo_kill_players", "Enable", "", 1.0);
      FastFire2.Execute("cap_hatch_destroy_animated_prop", "Enable", "", 1.0);
      FastFire2.Execute("cap_hatch_destroy_delete_prop", "Kill", "", 1.0);
      FastFire2.Execute("end_pit_destroy_particle", "Start", "", 1.0);
      FastFire2.Execute("trigger_hurt_hatch_fire", "Enable", "", 1.0);
      FastFire2.Execute("cap_hatch_glasswindow", "Break", "", 1.0);
      FastFire2.Execute("hatch_explo_kill_players", "Disable", "", 1.50);
      FastFire2.Execute("hatch_magnet_pit", "Disable", "", 5.0);
      FastFire2.Execute("cap_hatch_destroy_animated_prop", "SetAnimation", "explode", 1.0);
      FastFire2.Execute("tank_boss", "DestroyIfAtCapturePoint", "1", 1.0);
      FastFire2.Execute("tank_boss", "DestroyIfAtCapturePoint", "2", 1.0);
      FastFire2.Execute("tank_boss", "DestroyIfAtCapturePoint", "3", 1.0);
      EmitSoundToAll("MVM.BombExplodes");
      return;
    }
  //Shark Enable & notify bomb push began
    case 20: {
      Bombs.StartPush();
    }
  //Shark Disable
    case 21: {
      BombRegistry[0].isMoving = false;
      core.canSENTShark = false;
    }
  //HINDENBOOM ACTIVATION
    case 28: {
      EmitSoundToAll(SFXArray[5].realPath);
      FastFire2.Execute("HindenTrain", "StartForward");
      FastFire2.Execute("DeliveryBurg", "Lock");
      FastFire2.Execute("Boom", "Enable");
      FastFire2.Execute("Bombs.TheHindenburg", "Enable");
      FastFire2.Execute("Boom", "Disable", "", 1.0);
    }
  //HINDENBOOM!!!
    case 29: {
      CPrintToChatAll("{darkviolet}[{red}CORE{darkviolet}] {white}OH GOD, THEY'RE {red}CRASHING THE HINDENBURG{white}!!!");
      EmitSoundToAll(SFXArray[36].realPath);
      CreateTimer(4.0, TimedOperator, 21);
      CreateTimer(7.0, TimedOperator, 37);
      FastFire2.Execute("LargeExplosion", "Explode", "", 7.0);
      FastFire2.Execute("LargeExploShake", "StartShake", "", 7.0);
      FastFire2.Execute("NukeAll", "Enable", "", 7.0);
      FastFire2.Execute("FB.Fade", "Fade", "", 7.0);
      FastFire2.Execute("Bombs.TheHindenburg", "Disable", "", 7.0);
      FastFire2.Execute("HindenTrain", "TeleportToPathTrack", "Hinden01", 7.0);
      FastFire2.Execute("HindenTrain", "Stop", "", 7.0);
      FastFire2.Execute("NukeAll", "Disable", "", 9.0);
      CreateTimer(8.0, TimedOperator, 99);
      g_BombIndex = 4;
      BombRegistry[0].stateMax = 8;
    }
  //Bath Salts spend
    case 30: {
      CPrintToChatAll("{darkviolet}[{forestgreen}CORE{darkviolet}] {white}INSTANT BATH SALT DETONATION! BOTS ARE FROZEN FOR 10 SECONDS! ({red}-10 pts{white})");
      ServerCommand("sm_freeze @blue 10");
      ass_sacPoints -= 10;
      Bombs.explode(3, false);
    }
  //Goob/Kirb spend
    case 31: {
      int i = GetRandomInt(1, 2);
      if (i == 1) CreateTimer(1.5, TimedOperator, 21);
      FastFire2.Execute(i == 1 ? "FB.GiantGoobTemplate" : "FB.BlueKirbTemplate", "ForceSpawn");
      CPrintToChatAll(i == 1 ? "{darkviolet}[{forestgreen}CORE{darkviolet}] {white}GOOBBUE COMING IN FROM ORBIT! ({red}-20 pts{white})" : "{darkviolet}[{forestgreen}CORE{darkviolet}] {white}BLUE KIRBY FALLING OUT OF THE SKY! ({red}-20 pts{white})");
      CustomSoundEmitter(i == 1 ? SFXArray[51].realPath : SFXArray[21].realPath, 65, false, 0, 1.0, 100);
      ass_sacPoints -= 20;
    }
  //35K ubup cash spend
    case 32: {
      if (!core.doFailsafe) {
        core.doFailsafe = true;
        CPrintToChatAll("{darkviolet}[{forestgreen}CORE{darkviolet}] {white}Wave fail-safe activated! ({red}-30 pts{white})");
        ass_sacPoints -= 30;
      } else return;
    }
  //Explosive paradise spend
    case 33: {
      CustomSoundEmitter(SFXArray[10].realPath, 65, false, 0, 1.0, 100);
      FastFire2.Execute("FB.FadeBLCK", "Fade");
      ServerCommand("sm_evilrocket @blue");
      CPrintToChatAll("{darkviolet}[{forestgreen}CORE{darkviolet}] {white}We're spending most our lives living in an EXPLOSIVE PARADISE! Robots will be launched into orbit, too! ({red}-40 pts{white})");
    //Add this to explosion array?
      FastFire2.Execute("NukeAll", "Enable", "", 11.50);
      FastFire2.Execute("HUGEExplosion", "Explode", "", 11.50);
      FastFire2.Execute("FB.Fade", "Fade", "", 11.50);
      FastFire2.Execute("FB.ShakeBOOM", "StartShake", "", 11.50);
      FastFire2.Execute("NukeAll", "Disable", "", 12.30);
      ass_sacPoints -= 40;
    }
  //Banish Tornadoes
    case 34: {
      if (WeatherManager.CanFormTornado()) ass_sacPoints -= 50;
      CPrintToChatAll(WeatherManager.CanFormTornado() ? "{darkviolet}[{forestgreen}CORE{darkviolet}] {white}A PINK KIRBY HAS BANISHED TORNADOES FOR THIS WAVE! ({red}-50 pts{white})" : !WeatherManager.canTornado ? "{red}TTV/professorfartsalot {white}: Please do not, the tornado button. Thanks. ({red} -0 pts{white})" : WeatherManager.hasTornado ? "{red}TTV/professorfartsalot {white}:  You fool. It is much too late for you. ({red} -0 pts{white})" : "{red}TTV/professorfartsalot {white}:  To be honest, I never thought I'd get this far... ({red} -0 pts{white})");
      WeatherManager.canTornado = WeatherManager.hasTornado ? true : false;
    }
  //Ass Gas spend
    case 35: {
      CPrintToChatAll("{darkviolet}[{forestgreen}CORE{darkviolet}] {white}ANYTHING BUT THE ASS GAS!!!! ({red}-60 pts{white})");
      ass_sacPoints -= 60;
      FastFire2.Execute("HurtAll", "AddOutput", "damagetype 524288");
      FastFire2.Execute("FB.ShakeBOOM", "StartShake", "", 0.1);
      FastFire2.Execute("HurtAll", "AddOutput", "damage 400");
      FastFire2.Execute("HurtAll", "Enable", "", 0.1);
      FastFire2.Execute("HurtAll", "Disable", "", 4.1); //Add a sound to this in the future. Maybe gas sound from gbombs? Maybe custom fart sounds? hmm....
      FastFire2.Execute("FB.ShakeBOOM", "StopShake", "", 4.1);
    }
  //Nuclear fallout spend
    case 36: {
      CPrintToChatAll("{darkviolet}[{forestgreen}CORE{darkviolet}] {white}INSTANT FAT MAN DETONATION! ({red}-70 pts{white})");
      ass_sacPoints -= 70;
      Bombs.explode(7, false);
    }
  //Meteor shower spend
    case 37: {
      CPrintToChatAll("{darkviolet}[{forestgreen}CORE{darkviolet}] {white}COSMIC DEVASTATION IMMINENT. ({red}-80 pts{white})");
      ass_sacPoints -= 80;
      core.canSENTMeteors = true;
      CreateTimer(1.0, SENTMeteorTimer);
      CreateTimer(30.0, TimedOperator, 12);
    }
  //300K UbUp Cash
    case 38: {
      CPrintToChatAll("{darkviolet}[{forestgreen}CORE{darkviolet}] {white}150K UbUp Cash Granted to {red}RED{white}! ({red}-90 pts{white})");
      ass_sacPoints -= 90;
      ServerCommand("sm_addcash @red 300000");
    }
  //Fartsy of the Seventh Taco Bell
    case 39: {
      CPrintToChatAll("{darkviolet}[{forestgreen}CORE{darkviolet}] {white}NOW PRESENTING... PROFESSOR FARTSALOT OF THE SEVENTH TACO BELL! ({red}-100 points{white})");
      ass_sacPoints -= 100;
      BombRegistry[0].stateMax = 69;
      g_BombIndex = 69;
      FastFire2.Execute("Delivery", "Unlock");
      FastFire2.Execute("BombExplo*", "Disable");
      FastFire2.Execute("Bombs*", "Disable");
      FastFire2.Execute("Bombs.Professor", "Enable", "", 3.0);
    }
  //Found blue ball
    case 40: {
      CPrintToChatAll("{darkviolet}[{forestgreen}CORE{darkviolet}] {forestgreen}What on earth IS that? It appears to be a... \x075050FFBLUE BALL{white}!");
      CustomSoundEmitter(SFXArray[21].realPath, 65, false, 0, 1.0, 100);
      FastFire2.Execute("FB.BlueKirbTemplate", "ForceSpawn", "", 4.0);
      CreateTimer(4.0, TimedOperator, 21);
    }
  //Found burrito
    case 41: {
      CPrintToChatAll("{darkviolet}[{forestgreen}CORE{darkviolet}] {forestgreen}Why would you even eat {red}The Forbidden Burrito{white}?");
      CustomSoundEmitter("vo/sandwicheat09.mp3", 65, false, 0, 1.0, 100);
      FastFire2.Execute("HurtAll", "AddOutput", "damagetype 8");
      FastFire2.Execute("HurtAll", "AddOutput", "damage 2000");
      FastFire2.Execute("HurtAll", "Enable", "", 4.0);
      FastFire2.Execute("HurtAll", "Disable", "", 8.0);
    }
  //Found goobbue
    case 42: {
      isGoobbue = true;
      CustomSoundEmitter(SFXArray[51].realPath, 65, false, 0, 1.0, 100);
      CPrintToChatAll("{darkviolet}[{forestgreen}CORE{darkviolet}] {forestgreen}ALL HAIL \x07FF00FFGOOBBUE{forestgreen}!");
      CreateTimer(4.0, TimedOperator, 21);
      FastFire2.Execute("FB.GiantGoobTemplate","ForceSpawn", "", 4.0);
    }
  //Found Mario
    case 43: {
      CustomSoundEmitter(SFXArray[52].realPath, 65, false, 0, 1.0, 100);
      CPrintToChatAll("{darkviolet}[{forestgreen}CORE{darkviolet}] {forestgreen}Welp, someone is playing \x0700FF00Mario{white}...");
    }
  //Found Waffle
    case 44: {
      CustomSoundEmitter(SFXArray[53].realPath, 65, false, 0, 1.0, 100);
      CPrintToChatAll("{darkviolet}[{forestgreen}CORE{darkviolet}] {forestgreen}Oh no, someone has found and (probably) consumed a {red}WAFFLE OF MASS DESTRUCTION{white}!");
    //Yup, another one for the explosion array
      FastFire2.Execute("HurtAll", "AddOutput", "damage 2000000", 0.0);
      FastFire2.Execute("HurtAll", "AddOutput", "damagetype 262144", 10.0);
      FastFire2.Execute("ShakeBOOM", "StartShake", "", 10.3);
      FastFire2.Execute("HUGEExplosion", "Explode", "", 10.3);
      FastFire2.Execute("FB.Fade", "Fade", "", 10.3);
      FastFire2.Execute("HurtAll", "Enable", "", 10.3);
      FastFire2.Execute("HurtAll", "Disable", "", 12.3);
    }
  //Medium Explosion (defined again, but we aren't using a bomb this time)
    case 51: {
      CustomSoundEmitter(SFXArray[3].realPath, 65, false, 0, 1.0, 100);
    }
  //Probably for the hindenburg... EDIT: NOPE. THIS IS FOR KIRB LANDING ON THE GROUND
    case 52: {
      EmitSoundToAll(SFXArray[35].realPath);
      FastFire2.Execute("FB.BOOM", "StartShake");
      FastFire2.Execute("BlueBall*", "Kill");
      FastFire2.Execute("HUGEExplosion", "Explode");
      FastFire2.Execute("BlueKirb", "Kill");
      FastFire2.Execute("HurtAll", "AddOutput", "damagetype 32768");
      FastFire2.Execute("HurtAll", "AddOutput", "damage 777777.777");
      FastFire2.Execute("HurtAll", "Enable", "", 0.1);
      FastFire2.Execute("HurtAll", "Disable", "", 2.1);
      isGoobbue = false;
    }
  //Giant Goobbue
    case 53: {
      EmitSoundToAll(SFXArray[35].realPath);
      FastFire2.Execute("FB.BOOM", "StartShake");
      FastFire2.Execute("GiantGoob*", "Kill");
      FastFire2.Execute("HUGEExplosion", "Explode");
      FastFire2.Execute("HurtAll", "AddOutput", "damagetype 1048576");
      FastFire2.Execute("HurtAll", "AddOutput", "damage 777777.777");
      FastFire2.Execute("HurtAll", "Enable", "", 0.1);
      FastFire2.Execute("HurtAll", "Disable", "", 2.1);
    }
    case 75:{
      char onsHP[32];
      Format(onsHP, 32, "%i", GetBossHealthModifier(640000));
      PrintToChatAll(onsHP);
    }
  //Prev wave
    case 98: {
      int prev_wave = GetCurWave() - 1;
      if (prev_wave < 1) return;
      JumpToWave(prev_wave);
    }
  //Next wave
    case 99: {
      int ent = FindEntityByClassname(-1, "tf_objective_resource");
      if (ent == -1) {
        LogMessage("tf_objective_resource not found");
        return;
      }
      int max_wave = GetEntData(ent, FindSendPropInfo("CTFObjectiveResource", "m_nMannVsMachineMaxWaveCount"));
      int next_wave = GetCurWave() + 1;
      if (next_wave > max_wave) {
        int flags = GetCommandFlags("tf_mvm_force_victory");
        SetCommandFlags("tf_mvm_force_victory", flags & ~FCVAR_CHEAT);
        ServerCommand("tf_mvm_force_victory 1");
        FakeClientCommand(0, ""); //Not sure why, but this has to be here. Otherwise the specified commands simply refuse to work...
        SetCommandFlags("tf_mvm_force_victory", flags | FCVAR_CHEAT);
        CPrintToChatAll("{darkviolet}[{forestgreen}CORE{darkviolet}] {white}VICTORY HAS BEEN FORCED! THE SERVER WILL RESTART IN 10 SECONDS.");
        CreateTimer(10.0, TimedOperator, 100);
        return;
      }
      JumpToWave(next_wave);
    }
  //Code Entry from FC Keypad
    case 100: {
      if (core.CodeEntry == 17) {
        FastFire2.Execute("FB.BOOM", "StartShake");
        CustomSoundEmitter(SFXArray[3].realPath, 65, false, 0, 1.0, 100);
        FastFire2.Execute("FB.CodeCorrectKill", "Enable");
        FastFire2.Execute("FB.KP*", "Lock");
        FastFire2.Execute("FB.CodeCorrectKill", "Disable", "", 1.0);
      } else {
        core.CodeEntry = 0;
        FastFire2.Execute("FB.CodeFailedKill", "Enable");
        FastFire2.Execute("FB.CodeFailedKill", "Disable", "", 1.0);
        CustomSoundEmitter("fartsy/memes/priceisright_fail.wav", 65, false, 0, 1.0, 100);
      }
    }
    case 101: {
      core.CodeEntry++;
    }
    case 102: {
      core.CodeEntry += 2;
    }
    case 103: {
      core.CodeEntry += 3;
    }
    case 104: {
      core.CodeEntry += 4;
    }
    case 105: {
      core.CodeEntry += 5;
    }
    case 106: {
      core.CodeEntry += 6;
    }
    case 107: {
      core.CodeEntry += 7;
    }
    case 108: {
      core.CodeEntry += 8;
    }
    case 109: {
      core.CodeEntry += 9;
    }
  //Lighting Repairs
    case 110: {
      MapLighting[0].Repair();
    }
    case 111: {
      MapLighting[1].Repair();
    }
    case 112: {
      MapLighting[2].Repair();
    }
    case 113: {
      MapLighting[3].Repair();
    }
    case 114: {
      MapLighting[4].Repair();
    }
    case 115: {
      MapLighting[5].Repair();
    }
    case 116: {
      MapLighting[6].Repair();
    }
    case 117: {
      MapLighting[7].Repair();
    }
    case 118: {
      MapLighting[8].Repair();
    }
    case 119: {
      MapLighting[9].Repair();
    }
    case 120: {
      MapLighting[10].Repair();
    }
    case 121: {
      MapLighting[11].Repair();
    }
  //Spoopy
    case 130: {
      FastFire2.Execute("gTrain", "StartForward");
      FastFire2.Execute("gTelefect", "Start");
      FastFire2.Execute("gTelefect", "Stop", "", 3.0);
      FastFire2.Execute("gTrainSndHaunt", "PlaySound", "", GetRandomFloat(3.0, 7.0));
    }
  //Despawn ghosty boi in like 3 seconds
    case 131: { GhostHandler.Kill(); }
  //Determine what boss just deployed.
    case 132: {
    //if(MetalFaceDidIt){
    //Force phase 4 metal face here
    //}
      switch(BossHandler.bossID) {
      // brute justice
        case 0:{
          FastFire2.Execute("bots_win", "roundwin");
        }
      }
    }
  //Metal Face reached spawn point
    case 133: {
      CustomSoundEmitter(SFXArray[3].realPath, 45, false, 0, 1.0, 100);
      FastFire2.Execute("FB.MetalFace.Train", "Stop");
      FastFire2.Execute("FB.MetalFace.Train", "SetSpeedReal", "50", 3.0);
      FastFire2.Execute("FB.MetalFace.SpawnShake", "StartShake");
      FastFire2.Execute("FB.MetalFace.SpawnParticle", "Stop");
    }
  //Tank deployed its bomb
    case 134: {
      BossHandler.TriggerVictory();
      FastFire2.Execute("boss_deploy_relay", "Trigger", "", 5.0);
    }
  //Spawn metal face
    case 135: {
      BossHandler.OnSpawned(2);
    }
  //Metal Face dead
    case 136: {
      if(BossHandler.iBossPhase != 4){
        BossHandler.iBossPhase = 4;
      }
    }
  //Sephiroth dead (future code)
    case 138: {
    }
  //Wipe mechanic down (MetalFace)
    case 139: {
      BossHandler.shouldTick = false;
      FastFire2.Execute("FB.MetalFace", "Kill");
      FastFire2.Execute("FB.MetalFace.Train", "Kill");
      FastFire2.Execute("FB.MetalFace.SpawnParticle", "Kill");
      FastFire2.Execute("FB.MetalFace.SummonRockets", "Kill");
      FastFire2.Execute("FB.MetalFace.GigaBusterSNDTimer", "Kill");
      FastFire2.Execute("FB.MetalFace.GigaBuster.Explosion", "Kill");
      FastFire2.Execute("FB.MetalFace.GigaBusterMDL", "Kill");
      FastFire2.Execute("FB.MetalFace.GigaBusterSND", "Kill");
      BossHandler.OnKilled();
    }
  //Debug, somethingsomething aoe blah blah blah
    case 200: {
      CreateTimer(0.1, TimedOperator, 201);
    }
  //Debug, somethingsomething siren explosion thingy
    case 201: {
      WeatherManager.sirenPitch = 300.0;
    }
    case 203: {
      BossHandler.bDoTankBuster = true;
    }
    case 205: {
      PrintToChatAll("%N has the max emnity.", GetEmnityMax());
    }
  //Taco Bell Edition
    case 210: {
      core.tacobell = true;
      ServerCommand("fb_startmoney 200000");
      CPrintToChatAll("{darkviolet}[{orange}INFO{darkviolet}] {white}You have chosen {red}DOVAH'S ASS - TACO BELL EDITION{white}. Warning: All upgrade progress will be lost if you fail the wave. DO NOT FAIL US.");
    }
  //Reset Map
    case 300: {
      AudioManager.Reset(true);
      BossHandler.shouldTick = false;
      core.Reset();
      for (int i = 0; i < sizeof(EmnityManager); i++) EmnityManager[i].Clear();
      for (int i = 0; i < sizeof(MapLighting); i++) MapLighting[i].Repair();
      WeatherManager.Reset();
      BombRegistry[0].isReady = false;
      BombRegistry[0].stateMax = 7;
      g_BombIndex = 5;
      iDmgHealingTotal = 0;
      CreateTimer(1.0, PerformAdverts);
      CreateTimer(40.0, SelectAdminTimer);
      FastFire2.Execute("Barricade_Rebuild_Relay", "Trigger");
      FastFire2.Execute("FB.KP*", "Lock");
      FastFire2.Execute("OldSpawn", "Disable");
      FastFire2.Execute("NewSpawn", "Enable");
      FastFire2.Execute("CommonSpells", "Disable");
      FastFire2.Execute("RareSpells", "Disable");
      FastFire2.Execute("dovahsassprefer", "Disable");
      FastFire2.Execute("bombpath_left_arrows", "Disable");
      FastFire2.Execute("bombpath_right_arrows", "Disable");
      sudo(1000);
      sudo(1002);
      sudo(1007);
    }
  //TEMP FUNCTIONS
    case 301: {
      EmitSoundToAll(BGMArray[5].realPath, _, g_chanBGM, BGMArray[5].SNDLVL, SND_CHANGEVOL, 0.05, _, _, _, _, _, _);
    }
  //TEMP FUNCTIONS
    case 302: {
      EmitSoundToAll(BGMArray[5].realPath, _, g_chanBGM, BGMArray[5].SNDLVL, SND_CHANGEVOL, 1.0, _, _, _, _, _, _);
    }
    case 304: {
      EmitSoundToAll(BGMArray[6].realPath, _, g_chanBGM, BGMArray[6].SNDLVL, SND_CHANGEVOL, 0.05, _, _, _, _, _, _);
    }
    case 305: {
      EmitSoundToAll(BGMArray[6].realPath, _, g_chanBGM, BGMArray[6].SNDLVL, SND_CHANGEVOL, 1.0, _, _, _, _, _, _);
    }
    case 425: {
      for (int i = 0; i < 32; i++) {
        if (IsValidClient(i)) BossHandler.bustNukeTarget = i;
      }
      BossHandler.tickBusterNuclear = true;
    }
    case 426: {
    // FastFire2.Execute("NukeTankBusterThing", "ForceSpawn");
      FastFire2.Execute("PL.CannonPitch", "SetPosition", "1.0");
      FastFire2.Execute("PL.SentCart", "ForceSpawn", "", 5.0);
      FastFire2.Execute("PL.SentPhys", "Wake", "", 5.1);
      FastFire2.Execute("PL.CannonFodder", "Enable", "", 5.1);
      FastFire2.Execute("PL.CannonSND", "PlaySound", "", 5.1);
      FastFire2.Execute("PL.CannonFodder", "Disable", "", 5.3);
      FastFire2.Execute("PL.SentCartPhys", "Break", "", 6.0);
    }
    case 427: {
      FastFire2.Execute("PL.NukeCart", "ForceSpawn");
      FastFire2.Execute("PL.NukeCartPhys", "Break", "", 1.25);
      FastFire2.Execute("PL.NukeCartParticle", "Start", "", 1.25);
      FastFire2.Execute("PL.NukeCartShake", "StartShake", "", 1.25);
      FastFire2.Execute("PL.NukeCartExplo", "Explode", "", 1.25);
    }
  //LOOP SYSTEM
    case 500: {
      AudioManager.setBGM_Global(9, false);
    }
    case 501: {
      AudioManager.setBGM_Global(10, false);
    }
    case 502: {
      PrintToChatAll("Got 502 but not implemented in popfile, please report this to fartsy!");
      PrintToConsoleAll("[CORE] Phase Change started... phase 4!");
      AudioManager.setBGM_Global(11, false);
    }
  //Increment Wave Null
    case 700: {

    }
  // FINAL Music system rewrite (again) AGAINNNNNNNNNNNN.... and again!... and again :p and again!
    case 1000: {
      AudioManager.UpdateBGM();
    }
  //Stop current song
    case 1001: {
      for (int i = 0; i <= MaxClients; i++) {
        if (StrEqual(BGMArray[AudioState[i].indexBGM].realPath, "null")) continue;
        StopSound(i, g_chanBGM, BGMArray[AudioState[i].indexBGM].realPath);
      }
      return;
    }
  //Feature an admin
    case 1002: {
      char index[2];
      int i = GetRandomInt(1, 10);
      if (i == core.lastAdmin) i = GetRandomInt(1, 10);
      IntToString(i, index, sizeof(index));
      FastFire2.Execute("AdminPicker", "SetTextureIndex", index);
      core.lastAdmin = i;
      return;
    }
  //Flash Lightning
    case 1003: {
      for (int i = 0; i < sizeof(lightningFlash); i++) FastFire2.Execute(lightningFlash[i].fireStr);
    }
  //Activate Tornado Timer
    case 1004: {
      if (!WaveSystem().IsActive() || !WeatherManager.canTornado || WeatherManager.hasTornado) return;
      WeatherManager.TornadoTimerActive = true;
      WeatherManager.StartTornadoTimer(WaveSystem().GetWave() == 4 ? GetRandomFloat(30.0, 90.0) : GetRandomFloat(90.0, 210.0));
    }
  //Despawn the tornado
    case 1005: {
      if (GetRandomInt(0, 9) != 9){
        CPrintToChatAll("{red}TTV/professorfartsalot{white}: Nope :>");
        return;
      }
      WeatherManager.Dissipate();
      return;
    }
  //Crusader
    case 1006: {
      if (ICanSummon(84)) {
        AudioManager.SetEnabled(false);
        sudo(1001);
        crusader = true;
        LightLogger(LOGLVL_INFO, "Starting Crusader!");
        FastFire2.Execute("TheCrusaderTemplate", "ForceSpawn");
        FastFire2.SetEntityLife("CrusaderTrack*", 80.0);
        FastFire2.SetEntityLife("CrusaderTrain", 80.0);
        FastFire2.SetEntityLife("FB.CrusaderNuke", 80.0);
        FastFire2.SetEntityLife("FB.CRUSADER", 80.0);
        FastFire2.SetEntityLife("Target.FBLaserCore", 80.0);
        FastFire2.SetEntityLife("Target.FBLaserInner*", 80.0);
        FastFire2.SetEntityLife("Target.FBLaserOuterMost*", 80.0);
        FastFire2.SetEntityLife("CrusaderLaserBase", 80.0);
        FastFire2.SetEntityLife("CrusaderLaserBaseAlt", 80.0);
        FastFire2.SetEntityLife("CrusaderLaserBaseAlt1", 80.0);
        FastFire2.SetEntityLife("CrusaderLaserBaseAlt2", 80.0);
        FastFire2.SetEntityLife("CrusaderLaserBaseAlt3", 80.0);
        FastFire2.SetEntityLife("FB.LaserInnerMost", 80.0);
        FastFire2.SetEntityLife("FB.LaserOuterMost", 80.0);
        FastFire2.SetEntityLife("FB.LaserCore", 80.0);
        CreateTimer(1.75, CRUSADERINCOMING);
        CreateTimer(25.20, TimedOperator, 9);
        CreateTimer(25.20, TimedOperator, 78);
        CreateTimer(42.60, TimedOperator, 10);
        CreateTimer(63.20, TimedOperator, 80);
        EmitSoundToAll("fartsy/misc/fartsyscrusader_bgm_locus.mp3");
        CreateTimer(80.0, TimedOperator, 79);
        for (int i = 0; i < sizeof(crusaderAtk); i++) FastFire2.Execute(crusaderAtk[i].fireStr);
      }
    }
  //Choose bomb path
    case 1007: {
      FastFire2.Execute("Nest_*", "Disable");
      FastFire2.Execute("bombpath_right_prefer_flankers", "Disable");
      FastFire2.Execute("bombpath_left_prefer_flankers", "Disable");
      FastFire2.Execute("bombpath_left_navavoid", "Disable");
      FastFire2.Execute("bombpath_right_navavoid", "Disable");
      FastFire2.Execute("bombpath_right_arrows", "Disable");
      FastFire2.Execute("bombpath_left_arrows", "Disable");
      switch (GetRandomInt(1, 3)) {
        case 1: {
          FastFire2.Execute("Nest_R*", "Enable", "", 0.25);
          FastFire2.Execute("bombpath_right_prefer_flankers", "Enable", "", 0.25);
          FastFire2.Execute("bombpath_right_navavoid", "Enable", "", 0.25);
          FastFire2.Execute("bombpath_right_arrows", "Enable", "", 0.25);
        }
        case 2: {
          FastFire2.Execute("Nest_L*", "Enable", "", 0.25);
          FastFire2.Execute("bombpath_left_prefer_flankers", "Enable", "", 0.25);
          FastFire2.Execute("bombpath_left_navavoid", "Enable", "", 0.25);
          FastFire2.Execute("bombpath_left_arrows", "Enable", "", 0.25);
        }
        case 3: {
          FastFire2.Execute("dovahsassprefer", "Enable", "",  0.25);
          FastFire2.Execute("Nest_EN*", "Enable","",  0.25);
          FastFire2.Execute("bombpath_right_prefer_flankers", "Enable", "", 0.25);
          FastFire2.Execute("bombpath_right_navavoid", "Enable", "", 0.25);
          FastFire2.Execute("bombpath_right_arrows", "Enable", "", 0.25);
        }
      }
    }
  //Monitor power up/down!
    case 1008: {
      core.monitorOn = (!core.monitorOn ? true : false);
      FastFire2.Execute("FB.MonitorSprite", "Color", !core.monitorColor && core.monitorOn ? "0 0 255" : core.monitorOn ? "0 255 0" : "255 0 0", 0.0);
      FastFire2.Execute(!core.monitorColor && core.monitorOn ? "FB.MonitorBW" : core.monitorOn ? "FB.Monitor" : "FB.Monitor*", core.monitorOn ? "Enable" : "Disable");
      FastFire2.Execute(!core.monitorColor && core.monitorOn ? "FB.Monitor" : core.monitorOn ? "FB.MonitorBW" : "FB.MonitorBlank", core.monitorOn ? "Disable" : "Enable");
      FastFire2.Execute("FB.MonitorBlank", core.monitorOn ? "Disable" : "Enable", "", 0.2);
    }
  //Cycle monitor forward
    case 1009: {
      core.camSel++;
      if (core.camSel > 5) core.camSel = 0;
      FastFire2.Execute("FB.Monitor", "SetCamera", SelectedCamera[core.camSel]);
      FastFire2.Execute("FB.MonitorBW", "SetCamera", SelectedCamera[core.camSel]);
    }
  //Cycle monitor back
    case 1010: {
      core.camSel--;
      if (core.camSel < 0) core.camSel = 5;
      FastFire2.Execute("FB.Monitor", "SetCamera", SelectedCamera[core.camSel]);
      FastFire2.Execute("FB.MonitorBW", "SetCamera", SelectedCamera[core.camSel]);
    }
  //Toggle black and white mode.
    case 1011: {
      if (!core.monitorOn) return;
      core.monitorColor = (!core.monitorColor ? true : false);
      FastFire2.Execute("FB.MonitorSprite", "Color", core.monitorColor ? "0 255 0" : "0 0 255");
      FastFire2.Execute(core.monitorColor ? "FB.MonitorBW" : "FB.Monitor", "Disable");
      FastFire2.Execute(core.monitorColor ? "FB.Monitor" : "FB.MonitorBW", "Enable");
    }
    case 6942: {
      ass_sacPoints = 2147483647;
    }
  //Do not EVER EVER execute this unless it's an emergency...
    case 6969: {
      CPrintToChatAll(!WaveSystem().IsActive() ? "{darkred} [CORE] ERROR, attempted to invoke function without an active wave." : brawler_emergency ? "{darkred}[CORE] Failed to enter emergency mode, it is already active." : WaveSystem().IsWaveNull() ? "{darkred} [CORE] Failed to enter emergency mode, it has been disabled by an unknown entity." : "{darkred}[CORE] EMERGENCY MODE ACTIVATED... (v%s)", BEM_VER);
      if (!WaveSystem().IsActive() || brawler_emergency || WaveSystem().IsWaveNull()) return;
      Emerge(0);
    }
  //DEBUG
    case 7500:{
      FastFire2.Execute("LargeExplosion", "Explode", "", 0.5);
    }
    case 8000: {
      SephBoss[GetRandomInt(0, 6)].attack();
    }
    case 9000: {
      CreateTimer(10.0, BossHPTimer);
    }
  /*case 9010: {
    CustomSoundEmitter(TBGM6, 65, true, 1, 1.0, 100);
    CustomSoundEmitter(TBGM4, 65, true, 1, 0.05, 100);
    CustomSoundEmitter(TBGM5, 65, true, 1, 0.05, 100);
    CustomSoundEmitter(TBGM3, 65, true, 1, 0.05, 100);
  }
  case 9011: {
    CustomSoundEmitter(TBGM6, 65, true, 1, 1.0, 100);
    CustomSoundEmitter(TBGM4, 65, true, 1, 1.0, 100);
    CustomSoundEmitter(TBGM5, 65, true, 1, 0.05, 100);
    CustomSoundEmitter(TBGM3, 65, true, 1, 0.05, 100);
  }
  case 9012: {
    CustomSoundEmitter(TBGM6, 65, true, 1, 1.0, 100);
    CustomSoundEmitter(TBGM4, 65, true, 1, 1.0, 100);
    CustomSoundEmitter(TBGM5, 65, true, 1, 1.0, 100);
    CustomSoundEmitter(TBGM3, 65, true, 1, 0.05, 100);
  }
  case 9013: {
    CustomSoundEmitter(TBGM6, 65, true, 1, 1.0, 100);
    CustomSoundEmitter(TBGM4, 65, true, 1, 1.0, 100);
    CustomSoundEmitter(TBGM5, 65, true, 1, 1.0, 100);
    CustomSoundEmitter(TBGM3, 65, true, 1, 1.0, 100);
  }
  case 9014: {
    CustomSoundEmitter(TBGM6, 65, true, 1, 0.05, 100);
    CustomSoundEmitter(TBGM4, 65, true, 1, 1.0, 100);
    CustomSoundEmitter(TBGM5, 65, true, 1, 1.0, 100);
    CustomSoundEmitter(TBGM3, 65, true, 1, 1.0, 100);
  }
  case 9015: {
    CustomSoundEmitter(TBGM6, 65, true, 1, 0.05, 100);
    CustomSoundEmitter(TBGM4, 65, true, 1, 0.05, 100);
    CustomSoundEmitter(TBGM5, 65, true, 1, 1.0, 100);
    CustomSoundEmitter(TBGM3, 65, true, 1, 1.0, 100);
  }
  case 9016: {
    CustomSoundEmitter(TBGM6, 65, true, 1, 0.05, 100);
    CustomSoundEmitter(TBGM4, 65, true, 1, 0.05, 100);
    CustomSoundEmitter(TBGM5, 65, true, 1, 0.05, 100);
    CustomSoundEmitter(TBGM3, 65, true, 1, 1.0, 100);
  }
  //Play Instrumental
  case 9020: {
    CustomSoundEmitter(TBGM0, 65, true, 1, 1.0, 100);
    CustomSoundEmitter(TBGM1, 65, true, 1, 0.05, 100);
  }
  //Play Both
  case 9021: {
    CustomSoundEmitter(TBGM0, 65, true, 1, 1.0, 100);
    CustomSoundEmitter(TBGM1, 65, true, 1, 1.0, 100);
  }
  //Play vocal only
  case 9022: {
    CustomSoundEmitter(TBGM0, 65, true, 1, 0.05, 100);
    CustomSoundEmitter(TBGM1, 65, true, 1, 1.0, 100);
  }*/
  //Test
    case 10000:{
      int ent = FindEntityByClassname(-1, "tf_objective_resource");
      if (ent != -1){
        PrintToChatAll("%i / 3", GetEntData(ent, FindSendPropInfo("CTFObjectiveResource", "m_nFlagCarrierUpgradeLevel")));
      }
    }
    case 20000:{
      BossHandler.EmitSpawnSound(2);
    }
    case 21000:{
      int i = FindTarget(0, "Professor", true, false);
      AudioManager.setEventMode(i, 44, true);
    }
    case 22000:{
      int i = FindTarget(0, "Professor", true, false);
      AudioManager.setEventMode(i, 44, false);
    }
    case 42690:{
      BossHandler.bossID = 69;
      BossHandler.shouldTick = true;
      BossHandler.isAlive = true;
      EmitSoundToAll("fartsy/misc/brawler/fartsy_dll/intro.mp3");
      PrintToConsoleAll("Loading FARTSY.DLL from C:\\Windows\\System32 ...");
      PrintToServer("Loading FARTSY.IMG from /boot/efi ...");
      PrintToChatAll("FARTSY.DLL has joined the game");
      ServerCommand("sm_mortal @all");
      AddCommandListener(CommandOverride, "sm_buddha");
      AddCommandListener(CommandOverride, "sm_gimme");
      AddCommandListener(CommandOverride, "sm_givew");
      AddCommandListener(CommandOverride, "sm_gi");
      AddCommandListener(CommandOverride, "sm_god");
      AddCommandListener(CommandOverride, "sm_admin");
      AddCommandListener(CommandOverride, "sm_noclip");
      AddCommandListener(CommandOverride, "sm_noclipme");
      CreateTimer(6.1, TimedOperator, 50000);
    }
    case 42696:{
      RemoveCommandListener(CommandOverride, "sm_buddha");
      RemoveCommandListener(CommandOverride, "sm_god");
      RemoveCommandListener(CommandOverride, "sm_gimme");
      RemoveCommandListener(CommandOverride, "sm_givew");
      RemoveCommandListener(CommandOverride, "sm_gi");
      RemoveCommandListener(CommandOverride, "sm_admin");
      RemoveCommandListener(CommandOverride, "sm_noclip");
      RemoveCommandListener(CommandOverride, "sm_noclipme");
    }
    default: {
      char LogData[256];
      Format(LogData, sizeof(LogData), "No valid operator code %i found.", task);
      LightLogger(LOGLVL_ERROR, LogData);
    }
  }
  return;
}

public Action CommandOverride(int client, const char[] command, int argc)
{
  ServerCommand(GetRandomInt(0, 1 == 1) ? "sm_slay #%i" : "sm_smite #i", GetClientUserId(client));
  PrintToChatAll ("[FARTSY.DLL] Did you REALLY think I would let you cheat, %N? How about instead of using %s you actually play the game, loser!", client, command);
  return Plugin_Handled;
}

//Timed commands
public Action TimedOperator(Handle timer, int job) {
  switch (job) {
    case 0: {
      for (int i = 1; i < MaxClients; ++i) if(IsValidClient(i)) CPrintToChat(i, AudioState[i].VIPBGM >= 0 && AudioState[i].VIPIndex > 0 ? "{darkviolet}[{forestgreen}CORE{darkviolet}] {white}Wave %i: {forestgreen}%s{white} (requested by VIP {forestgreen}%N{white})" : "{darkviolet}[{forestgreen}CORE{darkviolet}] {white}Wave %i: {forestgreen}%s", WaveSystem().GetWave(), AudioState[i].songName, AudioState[i].VIPIndex);
    }
    //Boss script
    case 2: {
      sudo(1001); // Stop all bgm.
      CreateTimer(0.0, TimedOperator, 3);
    }
    //Boss script pt 2
    case 3: {
      AudioManager.setBGM_Global(16, true);
      FastFire2.Execute("FB.FadeTotalBLCK", "Fade");
      FastFire2.Execute("FB.FadeTotalBLCK", "Fade", "", 3.0);
      FastFire2.Execute("FB.FadeTotalBLCK", "Fade", "", 7.0);
      FastFire2.Execute("FB.FadeTotalBLCK", "Fade", "", 12.0);
      FastFire2.Execute("SephMeteor", "ForceSpawn", "", 19.6);
      CreateTimer(23.0, TimedOperator, 4);
    }
    //Boss script pt 3
    case 4: {
      CustomSoundEmitter(SFXArray[9].realPath, 65, false, 0, 1.0, 100);
      CreateTimer(4.1, TimedOperator, 5);
    }
    //Boss script pt 4
    case 5: {
      CustomSoundEmitter(SFXArray[58].realPath, 65, false, 0, 1.0, 100);
      FastFire2.Execute("SephNuke", "ForceSpawn", "", 3.0);
      CreateTimer(3.2, TimedOperator, 6);
    }
    //Boss script pt 5
    case 6: {
      CustomSoundEmitter(SFXArray[35].realPath, 65, false, 0, 1.0, 100);
      FastFire2.Execute("FB.Fade", "Fade");
      CreateTimer(1.0, SephATK);
      CreateTimer(1.7, TimedOperator, 7);
    }
    //DO THE THING ALREADY
    case 7: {
      core.sephiroth = true;
      //AudioState.ticksBGM = 0;
      //AudioState.loopSeconds = BGMArray[16].loopSeconds;
    }
    //Signal boss to actually spawn after delay.
    case 8: {
      CustomSoundEmitter(SFXArray[57].realPath, 65, false, 0, 1.0, 100);
      CPrintToChatAll("{darkgreen}[CORE] You did it!!! {darkred}Or... did you...?");
      FastFire2.Execute("FB.FadeBLCK", "Fade");
      CreateTimer(4.8, TimedOperator, 2);
    }
    //Crusader Nuke activation
    case 9: {
      core.canCrusaderNuke = true;
      CreateTimer(1.0, NukeTimer, 1);
    }
    //Crusader Nuke Deactivation
    case 10: {
      core.canCrusaderNuke = false;
      return Plugin_Stop;
    }
    //Seph Nuke Deactivation
    case 11: {
      SephBoss[0].shouldNuke = false;
      return Plugin_Stop;
    }
    //SENTMeteor Timeout
    case 12: {
      core.canSENTMeteors = false;
      return Plugin_Stop;
    }
    //SENTStars Timeout
    case 14: {
      core.canSENTStars = false;
      return Plugin_Stop;
    }
    //Incoming
    case 21: {
      CustomSoundEmitter(SFXArray[37].realPath, 65, false, 0, 1.0, 100);
      return Plugin_Stop;
    }
    case 37: {
      EmitSoundToAll(SFXArray[35].realPath);
      return Plugin_Stop;
    }
    //Severe storm coming!!!
    case 40: {
      WeatherManager.IssueTornadoWarning();
      return Plugin_Stop;
    }
    // start tornado
    case 41: {
      WeatherManager.TryFormTornado();
      return Plugin_Stop;
    }
    case 42: {
      WeatherManager.TornadoWarning = false;
      WeatherManager.Dissipate();
      sudo(1004);
    }
    case 60: {
      CustomSoundEmitter(SFXArray[40].realPath, SFXArray[40].SNDLVL, false, 0, 1.0, 100);
      return Plugin_Stop;
    }
    case 70: {
      if (WaveSystem().IsActive()) {
        FastFire2.Execute("FB.KP*", "Unlock");
        CustomSoundEmitter(SFXArray[0].realPath, SFXArray[0].SNDLVL, false, 0, 1.0, 100);
      }
      return Plugin_Stop;
    }
    case 71: {
      CustomSoundEmitter(SFXArray[62].realPath, SFXArray[62].SNDLVL, false, 0, 1.0, 100);
      return Plugin_Stop;
    }
    case 78: {
      EmitSoundToAll(SFXArray[7].realPath);
      return Plugin_Handled;
    }
    case 79: {
      crusader = false;
      if (WaveSystem().IsActive() && (WaveSystem().GetWave() == 3 || WaveSystem().GetWave() == 5)) { sudo(99); }
      return Plugin_Stop;
    }
    case 80: {
      EmitSoundToAll(SFXArray[59].realPath);
      return Plugin_Stop;
    }
    case 99: {
      if (WaveSystem().IsActive()) sudo(99);
      return Plugin_Stop;
    }
    case 100: {
      ServerCommand("_restart");
    }
    case 200:{
      for (int i = 0; i < sizeof(CircleAOEenabled); i++){
        CircleAOEenabled[i] = true;
      }
    }
    case 201:{
      int i = GetRandomInt(0, sizeof(CircleAOEpos)-1);
      if (!CircleAOEenabled[i]){
        if (attemptSpawn >= sizeof(CircleAOEpos)) return Plugin_Stop;
        attemptSpawn++;
        sudo(200);
        return Plugin_Stop;
      }
      attemptSpawn = 0;
      DispatchCircleAOE(CircleAOEpos[i]);
      PrintToChatAll("AOE %i CircleAOEen[i] = %i attempt %i sizeof %i", i, CircleAOEenabled[i], attemptSpawn, sizeof(CircleAOEpos));
      CircleAOEenabled[i] = false;
      CreateTimer(3.0, TimedOperator, 200);
      return Plugin_Stop;
    }
    case 202:{
      CustomSoundEmitter(SFXArray[60].realPath, SFXArray[60].SNDLVL, false, 0, 1.0, 100);
      return Plugin_Stop;
    }
    case 42690: {
      BossHandler.sOverflow = false;
      return Plugin_Stop;
    }
    case 50000: {
      ServerCommand("sv_cheats 1");
      for (int client = 1; client <= MaxClients; client++) if (IsClientInGame(client)) ClientCommand(client, "r_screenoverlay fartsy/fartsy_dll_core");
      ServerCommand("sv_cheats 0");
      FastFire2.Execute("FB.FadeBLCK", "Alpha", "127", 0.0);
      FastFire2.Execute("FB.FadeBLCK", "Fade");
      SendHudTextAll(1, "Did you think this would be easy...?", 0.1, 0.15, 9.0, {0, 160, 0, 255}, {0, 255, 0, 255}, 2, 0.5, 0.05, 0.75);
      EmitSoundToAll("fartsy/misc/brawler/fartsy_dll/terror.mp3");
      CreateTimer(3.0, TimedOperator, 50001);
      return Plugin_Stop;
    }
    case 50001: {
      FastFire2.Execute("FB.FadeBLCK", "Alpha", "127", 0.0);
      FastFire2.Execute("FB.FadeBLCK", "Fade");
      SendHudTextAll(2, "You can't possibly expect the same old tricks to work.... can you?~", 0.2, 0.2, 3.0, {0, 160, 0, 192}, {0, 255, 0, 192}, 2, 0.75, 0.05, 0.75);
      CreateTimer(5.0, TimedOperator, 50002);
      return Plugin_Stop;
    }
    case 50002: {
      FastFire2.Execute("FB.FadeBLCK", "Alpha", "127", 0.0);
      FastFire2.Execute("FB.FadeBLCK", "Fade");
      SendHudTextAll(1, "LAUGH IT UP. You and I both know this is 'just another boss fight'...", 0.15, 0.25, 7.0, {0, 160, 0, 192}, {0, 255, 0, 192}, 2, 0.5, 0.05, 0.75);
      CreateTimer(5.0, TimedOperator, 50003);
      return Plugin_Stop;
    }
    case 50003: {
      FastFire2.Execute("FB.FadeBLCK", "Alpha", "192", 0.0);
      FastFire2.Execute("FB.FadeBLCK", "Fade");
      SendHudTextAll(2, "...or is it?~", 0.2, 0.3, 2.0, {0, 160, 0, 192}, {0, 255, 0, 192}, 2, 0.5, 0.05, 0.75);
      CreateTimer(4.0, TimedOperator, 50004);
      return Plugin_Stop;
    }
    case 50004:{
      FastFire2.Execute("FB.FadeBLCK", "Alpha", "224", 0.0);
      FastFire2.Execute("FB.FadeBLCK", "Fade");
      SendHudTextAll(1, "ARE YOU HAPPY? All of you who knew what was happening...~", 0.25, 0.35, 7.0, {0, 160, 0, 192}, {0, 255, 0, 192}, 2, 0.5, 0.05, 0.75);
      CreateTimer(5.0, TimedOperator, 50005);
      return Plugin_Stop;
    }
    case 50005:{
      FastFire2.Execute("FB.FadeBLCK", "Alpha", "224", 0.0);
      FastFire2.Execute("FB.FadeBLCK", "Fade");
      SendHudTextAll(2, "Every last one of you...", 0.35, 0.4, 5.0, {0, 160, 0, 192}, {0, 255, 0, 192}, 1, 0.5, 0.5, 0.75);
      CreateTimer(5.0, TimedOperator, 50006);
      return Plugin_Stop;
    }
    case 50006:{
      FastFire2.Execute("FB.FadeBLCK", "Alpha", "192", 0.0);
      FastFire2.Execute("FB.FadeBLCK", "Fade");
      SendHudTextAll(1, "I hope you're as happy as I am...~", 0.15, 0.45, 7.0, {0, 160, 0, 192}, {0, 255, 0, 192}, 2, 0.5, 0.05, 0.75);
      CreateTimer(4.0, TimedOperator, 50007);
      return Plugin_Stop;
    }
    case 50007:{
      FastFire2.Execute("FB.FadeBLCK", "Fade");
      SendHudTextAll(2, "...knowing you all just spent all your time here~", 0.25, 0.5, 5.0, {0, 160, 0, 192}, {0, 255, 0, 192}, 2, 0.5, 0.05, 0.75);
      CreateTimer(5.0, TimedOperator, 50008);
      return Plugin_Stop;
    }
    case 50008:{
      FastFire2.Execute("FB.FadeBLCK", "Alpha", "255", 0.0);
      FastFire2.Execute("FB.FadeBLCK", "Fade");
      ServerCommand("sv_cheats 1");
      for (int client = 1; client <= MaxClients; client++) if (IsClientInGame(client)) ClientCommand(client, "r_screenoverlay fartsy/fartsy_dll_anon");
      ServerCommand("sv_cheats 0");
      FastFire2.Execute("FB.FadeBLCK", "Fade", "", 3.0);
      SendHudTextAll(1, "JUST TO DIE!!!", 0.4, 0.6, 7.0, {255, 0, 0, 255}, {255, 0, 0, 255}, 1, 0.5, 0.05, 0.75);
      CreateTimer(5.75, TimedOperator, 50009);
      return Plugin_Stop;
    }
    case 50009:{
      ServerCommand("sv_cheats 1");
      for (int client = 1; client <= MaxClients; client++) if (IsClientInGame(client)) ClientCommand(client, "r_screenoverlay \\");
      ServerCommand("sv_cheats 0");
      EmitSoundToAll("fartsy/music/tcmusic/humanhypothesis.wav");
      return Plugin_Stop;
    }
    case 51000:{

    }
    case 99999:{
      LightLogger(LOGLVL_INFO, "Server empty for 600s, restarting!");
      ServerCommand("_restart");
    }
  }
  return Plugin_Stop;
}