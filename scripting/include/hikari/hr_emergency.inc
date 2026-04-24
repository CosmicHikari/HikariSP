 // Brawler Emergency Mode Rewrite
char BEM_VER[8] = "1.1.0";
int BEM = 0;
bool brawler_emergency;

void Emerge(int stage) {
  switch (stage) {
    case 0: {
      BEM = 0;
      brawler_emergency = true;
      EmitSoundToAll(SFXArray[55].realPath);
      CreateTimer(1.0, EmergeTimer);
      ass_sacPoints = 0;
      ServerCommand("sm_addcash @red 2000000");
      ServerCommand("sm_god @red 1");
      return;
    }
    case 1: {
      ServerCommand("sm_freeze @blue; sm_smash @blue; sm_evilrocket @blue");
      CreateTimer(4.0, EmergeTimer);
      return;
    }
    case 2: {
      EmitSoundToAll(SFXArray[6].realPath);
      CreateTimer(7.0, EmergeTimer);
      return;
    }
    case 3: {
      ServerCommand("hr_send 40;hr_send 40;hr_send 40;hr_send 40;hr_send 40;hr_send 40;hr_send 40;hr_send 40;hr_send 40;hr_send 40");
      CreateTimer(1.0, EmergeTimer);
      return;
    }
    case 4: {
      ServerCommand("hr_send 42;hr_send 42;hr_send 42;hr_send 42;hr_send 42;hr_send 42;hr_send 42;hr_send 42;hr_send 42;hr_send 42");
      sudo(1006);
      CreateTimer(1.0, EmergeTimer);
      return;
    }
    case 5: {
      g_BombIndex = 8;
      BombRegistry[0].stateMax = 10;
      sudo(15);
      CreateTimer(1.0, EmergeTimer);
      return;
    }
    case 6: {
      g_BombIndex = 16;
      BombRegistry[0].stateMax = 18;
      ServerCommand("hr_send 15;hr_send 15");
      CreateTimer(1.0, EmergeTimer);
      return;
    }
    case 7: {
      g_BombIndex = 24;
      BombRegistry[0].stateMax = 26;
      ServerCommand("hr_send 15;hr_send 15;hr_send 15");
      CreateTimer(1.0, EmergeTimer);
      return;
    }
    case 8: {
      g_BombIndex = 32;
      BombRegistry[0].stateMax = 34;
      ServerCommand("hr_send 15;hr_send15;hr_send 15;hr_send 15");
      CreateTimer(1.0, EmergeTimer);
      return;
    }
    case 9: {
      g_BombIndex = 40;
      BombRegistry[0].stateMax = 42;
      ServerCommand("hr_send 15;hr_send 15;hr_send 15;hr_send 15;hr_send 15");
      CreateTimer(1.0, EmergeTimer);
      return;
    }
    case 10: {
      g_BombIndex = 48;
      BombRegistry[0].stateMax = 50;
      ServerCommand("hr_send 15;hr_send 15;hr_send 15;hr_send 15;hr_send 15;hr_send 15");
      ServerCommand("hr_send 30");
      CreateTimer(1.0, EmergeTimer);
      return;
    }
    case 11: {
      g_BombIndex = 5, 
      ServerCommand("hr_send 31");
      CreateTimer(1.0, EmergeTimer);
      return;
    }
    case 12: {
      ServerCommand("hr_send 32");
      CreateTimer(1.0, EmergeTimer);
      return;
    }
    case 13: {
      ServerCommand("hr_send 33");
      CreateTimer(1.0, EmergeTimer);
      return;
    }
    case 14: {
      ServerCommand("hr_send 34");
      CreateTimer(1.0, EmergeTimer);
      return;
    }
    case 15: {
      ServerCommand("hr_send 35");
      CreateTimer(1.0, EmergeTimer);
      return;
    }
    case 16: {
      ServerCommand("hr_send 36");
      CreateTimer(1.0, EmergeTimer);
      return;
    }
    case 17: {
      sudo(37);
      CreateTimer(1.0, EmergeTimer);
      return;
    }
    case 18: {
      ServerCommand("sm_freeze @blue -1; sm_smash @blue");
      CreateTimer(3.0, EmergeTimer);
      return;
    }
    case 19: {
      ServerCommand("hr_send 40; hr_send 42; hr_send 30; hr_send 32; hr_send 34; hr_send 32; hr_send 31; hr_send 42;hr_send 42;hr_send 42;hr_send 31;hr_send 32;hr_send 32;hr_send 31;hr_send 32;hr_send 32");
      CreateTimer(1.0, EmergeTimer);
      return;
    }
    case 20: {
      g_BombIndex = 48;
      BombRegistry[0].stateMax = 50;
      ServerCommand("hr_send 15;hr_send 15;hr_send 15;hr_send 15;hr_send 15;hr_send 15");
      for (int i = 0; i < 12; i++) {
        sudo(GetRandomInt(30, 37));
        sudo(GetRandomInt(40, 44));
        sudo(1003);
      }
      CreateTimer(30.0, EmergeTimer);
      return;
    }
    case 21: {
      ExitEmergencyMode();
      return;
    }
  }
}

public Action EmergeTimer(Handle timer) {
  if (!WaveSystem().IsActive()) { ExitEmergencyMode(); return Plugin_Stop; }
  BEM++;
  Emerge(BEM);
  return Plugin_Stop;
}

//Exit emergency mode!
public void ExitEmergencyMode() {
  CPrintToChatAll(!WaveSystem().IsActive() ? "{darkgreen}[CORE] Exiting emergency mode with CODE -1/WAVE_END (the wave has ended). %i actions executed." : "{darkgreen}[CORE] Exiting emergency mode with CODE 0/SUCCESS. %i actions executed.", BEM + 1); //BEM+1 because BEM actually executes on 0.
  brawler_emergency = false;
  ass_sacPoints = 0;
  ServerCommand("sm_god @red 0");
  BEM = 0;
  return;
}