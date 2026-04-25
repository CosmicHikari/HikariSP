// Hikari's Scene Enhancer (Inspired by Mrbt0907/Weather2Remastered)
char FSE_VER[8] = "2.3.2";

// All background music
enum struct BGM {
  char realPath[64];
  char songName[64];
  float introSeconds;
  float loopSeconds;
  int SNDLVL;
}
BGM BGMArray[48];

// All sound effects
enum struct SFXARRAY {
  char realPath[64];
  int SNDLVL;
}
SFXARRAY SFXArray[128];

// Sound preference menu
char sndPrefs[][128] = {
  "Sounds are currently DISABLED",
  "Sounds are currently MUSIC ONLY",
  "Sounds are currently SOUND EFFECTS ONLY",
  "Sounds are currently ALL ON",
  "Somehow your sound preference was stored as non-existent 5... Please configure your sounds."
};
Handle cvarSNDDefault = INVALID_HANDLE;
Handle cvarSirenEnabledDefault = INVALID_HANDLE;
Handle cvarSirenMutesMusicDefault = INVALID_HANDLE;
int soundPreference[MAXPLAYERS + 1];
bool g_SirenMutesMusic[MAXPLAYERS + 1];
bool g_SirenEnabled[MAXPLAYERS + 1];

//Get client sound prefs
public void SQL_SNDPrefs(Database db, DBResultSet results, const char[] error, int client) {
  if (!results) { LogError("Failed to query database: %s", error); return; }
  if (!IsValidClient(client)) return;
  if (results.FetchRow()) { soundPreference[client] = results.FetchInt(0); g_SirenEnabled[client] = results.FetchInt(1) == 1 ? true : false; g_SirenMutesMusic[client] = results.FetchInt(2) == 1 ? true : false; }
}

// Music system rewrite for the 9th time.
/* I have rewritten this stupid thing 9 times now. It now exists in its final form. Aka, perfect alexander.
 * This is the audio manager. It plays music. It plays music PER CLIENT. It is good at playing MUSIC.
 * It syncs music. It tracks the EXACT MILLISECOND POSITION of the currently playing song.
 * It is... PERFECT.
*/
int g_chanBGM;
int g_indexBGM;
enum struct AUDIOSTATE {
  bool bgmPlaying;
  bool eventMode;
  bool shouldTick;
  bool stopBGM;
  bool hasTimeOffset;
  bool clientIsFresh;
  char cachedPath[128];
  char songName[128];
  float timeSeconds;
  float savedTimeSeconds;
  float loopSeconds;
  float savedLoopSeconds;
  float introSeconds;
  float savedIntroSeconds;
  int client;
  int savedIndexBGM;
  int indexBGM;
  int loops;
  int savedLoops;
  int VIPBGM;
  int VIPIndex;
  int volume;
  int volume_sfx;
  float volume_real;
  float volume_real_sfx;
}
AUDIOSTATE AudioState[MAXPLAYERS+1];

methodmap AUDIOMANAGER {
  // Check if client is in event mode
  public bool isClientInEventMode(int client) { return AudioState[client].eventMode; }

  /** Gets the engine time for a client, accounting for intro offset seconds **/
  public float engineSecondsAdjusted(int client) {
    return GetEngineTime() + AudioState[client].introSeconds;
  }

  // Gets the default BGM
  public int GetDefaultBGM() {
    return WaveSystem().IsActive() ? (GetCoreData().sephiroth ? 16 : GetDefaultsManager().GetDefaults(WaveSystem().GetWave()).defBGMIndex) : GetRandomInt(1, 4);
  }

  // Initialise all AudioStates
  public void init() {
    AssLogger(LOGLVL_INFO, "Initializing Audio System...");
    g_chanBGM = 6;
    for (int i = 1; i < MaxClients; ++i) {
      AudioState[i].bgmPlaying = false;
      AudioState[i].stopBGM = false;
      AudioState[i].shouldTick = false;
      AudioState[i].cachedPath = "null";
      AudioState[i].songName = "null";
      AudioState[i].indexBGM = 0;
      AudioState[i].loopSeconds = 0.0;
      AudioState[i].clientIsFresh = true;
      AudioState[i].VIPIndex = -1;
      if (IsValidClient(i)) {
        AudioState[i].client = i;
        for (int x = 0; x < sizeof(BGMArray); ++x) StopSound(i, g_chanBGM, BGMArray[x].realPath);
      }
    }
    g_indexBGM = GetRandomInt(1, 4);
    this.UpdateBGM();
    CreateTimer(1.0, EnableAudio);
  }

  /* Reset all AudioStates
  * @param fullReset - Reset globally? true/false
  * @param client - client index to reset for
  */
  public void Reset(bool fullReset, int client = 0) {
    if (fullReset) {
      g_indexBGM = !WaveSystem().IsWaveNull() ? GetRandomInt(1, 4) : 28;
      for (int i = 0; i < MaxClients; ++i) this.Reset(false, i);
      AssLogger(LOGLVL_DEBUG, "All AudioStates have been reset!");
    }
    else {
      AudioState[client].stopBGM = true;
      AudioState[client].loops = 0;
      AudioState[client].indexBGM = 0;
      AudioState[client].timeSeconds = 0.0;
      if (IsValidClient(client)) for (int s = AudioState[client].indexBGM; s < sizeof(BGMArray); s++) StopSound(client, g_chanBGM, BGMArray[s].realPath);
      AssLogger(LOGLVL_DEBUG, "AudioState %i has been reset!", client);
    }
  }

  /* Changes the current music globally.
   * @param bgm - The index of the music to be played
   * @param instant - True = stops music NOW and changes, False = play current song then change
  */
  public void setBGM_Global(int bgm, bool instant) {
    for (int i = 0; i < MaxClients; ++i) if (IsValidClient(i)) { this.setBGM(i, bgm, instant); }
  }

  /* Changes the current music for <client>.
   * @param client - The client index to change music for
   * @param bgm - The index of the music to be played
   * @param instant - True = stops music NOW and changes, False = play current song then change
  */
  public void setBGM(int client, int bgm, bool instant) {
    //g_indexBGM = bgm == 0 ? GetRandomInt(1, 4) : bgm;
    if (IsValidClient(client)) PrintToChat(client, "Your BGM was changed. %i = %i > 0 ? %i : %i == 0 ? %i : %i, playing at sndlvl %i volume %f", AudioState[client].indexBGM, AudioState[client].VIPBGM, AudioState[client].VIPBGM, bgm, g_indexBGM, bgm, GetSNDLVL(client, true, BGMArray[AudioState[client].indexBGM].SNDLVL), GetVolume(client, true, 1.0));
    AudioState[client].indexBGM = AudioState[client].VIPBGM > 0 ? AudioState[client].VIPBGM : bgm == 0 ? g_indexBGM : bgm;
    PrintToServer("Set %i for %N", AudioState[client].indexBGM, client);
    if (instant) {
      AudioState[client].timeSeconds = 0.0;
      CreateTimer(1.0, SyncMusic, client);
    }
    PrintToServer("%f", GetEngineTime());
    AudioState[client].shouldTick = true;
    AudioState[client].hasTimeOffset = BGMArray[AudioState[client].indexBGM].introSeconds > 0 ? true : false;
    AudioState[client].stopBGM = !StrEqual(AudioState[client].cachedPath, BGMArray[AudioState[client].indexBGM].realPath) && !this.isClientInEventMode(client) ? true : false;
    PrintToServer(!StrEqual(AudioState[client].cachedPath, BGMArray[AudioState[client].indexBGM].realPath) && !this.isClientInEventMode(client) ? "true" : "false");
    AudioState[client].bgmPlaying = true;
    PrintToServer("Setting %N's BGM to %i", client, bgm);
  }

  /* Sets the VIP's chosen song for all clients */
  public void setBGM_VIP(int bgm) {
    bgm = bgm <= 0 ? this.GetDefaultBGM() : bgm;
    for (int i = 0; i < MaxClients; ++i)  if (IsValidClient(i) && !this.isClientInEventMode(i)) { AudioState[i].VIPBGM = bgm; }
  }

  /** Sets event mode for a client
   * @param client - The client index to set event mode for
   * @param bgm - The bgm index to set them to
   * @param activate - true or false (active/inactive)
   */
  public void setEventMode(int client, int bgm, int activate) {
    int curBGM = AudioState[client].indexBGM;
    if (activate == 1) {
      PrintToChat(client, "Saving %i for you", AudioState[client].indexBGM);
      AudioState[client].VIPBGM = 0;
      AudioState[client].savedIndexBGM = AudioState[client].indexBGM;
      AudioState[client].savedTimeSeconds = AudioState[client].timeSeconds;
      AudioState[client].savedLoopSeconds = AudioState[client].loopSeconds;
      AudioState[client].savedIntroSeconds = AudioState[client].introSeconds;
      AudioState[client].savedLoops = AudioState[client].loops;
      AudioState[client].eventMode = true;
      CSEClientEx(client, BGMArray[curBGM].realPath, -2, g_chanBGM, BGMArray[curBGM].SNDLVL, true, 1, 0.05, 100); // Set old bgm to low volume
      PrintToServer("[AudioState] Entering event mode with song %s", BGMArray[bgm].songName);
      this.setBGM(client, bgm, true);
    }
    else {
      StopSound(client, g_chanBGM, BGMArray[curBGM].realPath);
      AudioState[client].indexBGM = AudioState[client].savedIndexBGM;
      AudioState[client].loops = AudioState[client].savedLoops;
      AudioState[client].introSeconds = AudioState[client].savedIntroSeconds;
      AudioState[client].loopSeconds = AudioState[client].savedLoopSeconds;
      AudioState[client].timeSeconds = AudioState[client].savedTimeSeconds;
      int new_bgm = AudioState[client].indexBGM;
      CSEClientEx(client, BGMArray[new_bgm].realPath, -2, g_chanBGM, BGMArray[new_bgm].SNDLVL, true, 1, 1.0, 100);
      AudioState[client].eventMode = false;
      CreateTimer(1.0, SyncMusic, client);
      PrintToServer("[AudioState] Exiting event mode with song %s", BGMArray[new_bgm].songName);
    }
  }

  // Stops all music
  public void StopAll() { for (int i = 0; i < MaxClients; ++i) this.Stop(i); }

  // Stops for a client
  public void Stop(int client) {
    StopSound(client, g_chanBGM, BGMArray[AudioState[client].indexBGM].realPath);
    StopSound(client, g_chanBGM, AudioState[client].cachedPath);
    AudioState[client].shouldTick = false;
    AudioState[client].bgmPlaying = false;
    AudioState[client].indexBGM = 0;
    AudioState[client].loops = 0;
    AudioState[client].stopBGM = true;
    AudioState[client].timeSeconds = 0.0;
  }

  // Tick audio if a client should be ticking
  public void TickGlobal() { for (int i = 0; i < MaxClients; ++i) if (IsValidClient(i) && AudioState[i].shouldTick) this.TickClient(i); }

  // Tick audio for a client
  public void TickClient(int client) {
    if (!IsValidClient(client) || !AudioState[client].shouldTick) { this.Stop(client); return; }
    if (this.isClientInEventMode(client)) {
      if (AudioState[client].savedIndexBGM != g_indexBGM && AudioState[client].VIPBGM <= 0) AudioState[client].savedIndexBGM = g_indexBGM;
      if (this.engineSecondsAdjusted(client) > AudioState[client].savedTimeSeconds + AudioState[client].savedLoopSeconds) {
        AudioState[client].savedTimeSeconds = GetEngineTime();
        AudioState[client].savedLoops++;
        AudioState[client].savedIntroSeconds = AudioState[client].savedLoops >= 1 ? BGMArray[AudioState[client].savedIndexBGM].introSeconds : 0.0;
        AudioState[client].savedLoopSeconds = BGMArray[AudioState[client].savedIndexBGM].loopSeconds;
      }
    }
    AudioState[client].indexBGM = (!this.isClientInEventMode(client) && AudioState[client].VIPBGM > 0) ? AudioState[client].VIPBGM : (AudioState[client].indexBGM == 0 ? g_indexBGM : AudioState[client].indexBGM);
    if (this.engineSecondsAdjusted(client) > AudioState[client].timeSeconds + AudioState[client].loopSeconds) {
      PrintToServer("Playing %i for %N", AudioState[client].indexBGM, client);
      AudioState[client].timeSeconds = GetEngineTime();
      AudioState[client].loops++;
      if (AudioState[client].stopBGM) {
        PrintToServer("Stopping %s", AudioState[client].cachedPath);
        StopSound(client, g_chanBGM, AudioState[client].cachedPath);
        AudioState[client].loops = 0;
      }
      AudioState[client].stopBGM = StrContains(BGMArray[AudioState[client].indexBGM].realPath, "mp3", false) != -1;
      CSEClientEx(client, BGMArray[AudioState[client].indexBGM].realPath, -2, g_chanBGM, BGMArray[AudioState[client].indexBGM].SNDLVL, true, 1, 1.0, 100);
      strcopy(AudioState[client].songName, sizeof(AudioState[client].songName), BGMArray[AudioState[client].indexBGM].songName);
      AudioState[client].introSeconds = AudioState[client].loops >= 1 ? BGMArray[AudioState[client].indexBGM].introSeconds : 0.0;
      AudioState[client].loopSeconds = BGMArray[AudioState[client].indexBGM].loopSeconds;
      CreateTimer(1.0, SyncMusic, client);
    }
  }

  // Sets EVERYONE'S BGM in sync! Unless they're in an event...
  public void UpdateBGM() {
    for (int i = 0; i < MaxClients; ++i) {
      if (IsValidClient(i)) {
        if (this.isClientInEventMode(i) || !IsValidClient(i)) {
          PrintToServer("Client %N valid? %s Event Mode? %s", i, IsValidClient(i) ? "true" : "false", this.isClientInEventMode(i) ? "true" : "false");
          continue;
        }
        this.setBGM(i, AudioState[i].VIPBGM > 0 && !this.isClientInEventMode(i) ? AudioState[i].VIPBGM : AudioState[i].indexBGM == 0 ? g_indexBGM : AudioState[i].indexBGM, true);
        AudioState[i].shouldTick = GetWeatherManager().TornadoWarning && g_SirenMutesMusic[i] ? false : true;
      }
    }
  }

  // Enables AudioStates
  public void SetEnabled(bool state) {
    for (int i = 0; i < MaxClients; i++) { AudioState[i].shouldTick = state; }
  }

  /* When a client joins the server, set this audio manager to theirs */
  public void OnClientConnected(int client) {
    if (!IsValidClient(client) || IsFakeClient(client)) { PrintToServer("Invalid client %N", client); return; }
    AudioState[client].client = client;
    AudioState[client].clientIsFresh = true;
  }

  /* Handles client disconnects */
  public void OnClientDisconnect(int client) {
    PrintToServer("[AudioState @ %i] Client %N disconnected, shutting down gracefully!", client, client);
    AudioState[client].client = -1;
  }

  /* When a client respawns */
  public void OnClientRespawned(int client) {
    if (AudioState[client].clientIsFresh && IsValidClient(client)) {
      if (soundPreference[client] == 1 || soundPreference[client] == 3) CPrintToChat(client, "[AudioState v%s] Welcome! We are listening to %s", FSE_VER, BGMArray[g_indexBGM].songName);
      this.setBGM(client, g_indexBGM, true);
      AudioState[client].clientIsFresh = false;
    }
  }

  /* When a tornado warning is issued */
  public void OnTornadoWarning(bool issued) {
    for (int i = 0; i < MaxClients; i++) if (IsValidClient(i) && g_SirenEnabled[i] && g_SirenMutesMusic[i]) issued ? this.Stop(i) : this.setBGM(i, g_indexBGM, true);
  }
  
  /* When a client sets the volume */
  public void SetVolume(int client, int vol) { AudioState[client].volume = vol; PrintToChat(client, "Set your volume to %i", AudioState[client].volume); }

  /* When a client sets the volume */
  public void SetVolumeSFX(int client, int vol) { AudioState[client].volume_sfx = vol; PrintToChat(client, "Set your volume to %i", AudioState[client].volume_sfx); }

  /* This should be obvious by now */
  public void SetVolumeReal(int client, float vol) { AudioState[client].volume_real = vol; PrintToChat(client, "Set your volume to %f%", AudioState[client].volume_real * 100); }

  /* This should be obvious by now */
  public void SetVolumeRealSFX(int client, float vol) { AudioState[client].volume_real_sfx = vol; PrintToChat(client, "Set your volume to %f%%", AudioState[client].volume_real_sfx * 100); }

  public void OnStartup(int event, const char[] path = "") {
    switch (event) {
      case 0: {
        char finalPath[PLATFORM_MAX_PATH];
        Format(finalPath, sizeof(finalPath), "%s/Files", path);
        CreateDirectory(finalPath);
      }
    }
  }
}
AUDIOMANAGER AudioManager;

public Action EnableAudio(Handle timer) {
  AudioManager.SetEnabled(true);
  return Plugin_Stop;
}

// Custom sound emitter, I don't know how many f-ing times I've rewritten this! +2! See potato.sp
// @param flags
//  `SND_NOFLAGS (0)` Nothing,
//  `SND_CHANGEVOL (1)` Changes volume,
//  `SND_CHANGEPITCH (2)` Changes pitch,
//  `SND_STOP (3)` Stops the sound,
//  `SND_SPAWNING (4)` Used in some cases for ambients,
//  `SND_DELAY (5)`, Sound has an initial delay,
//  `SND_STOPLOOPING (6)` Stop looping all sounds on the entity,
//  `SND_SPEAKER (7)` Being played by a mic through a speaker,
//  `SND_SHOULDPAUSE (8)` Pause if game is paused.
void CustomSoundEmitter(char[] sndName, int TSNDLVL, bool isBGM, int flags, float vol, int pitch) {
  for (int i = 1; i <= MaxClients; i++) {
    if (!IsValidClient(i)) continue;
    if (isBGM && (soundPreference[i] == 1 || soundPreference[i] == 3) || !isBGM && soundPreference[i] >= 2) EmitSoundToClient(i, sndName, _, isBGM ? g_chanBGM : 0, GetSNDLVL(i, isBGM, TSNDLVL), flags, GetVolume(i, isBGM, vol), pitch, _, _, _, _, _);
  }
}

// Play sound to client. Ripped straight from potato. Allows us to play sounds directly to people when they join.
void CSEClient(int client, char[] sndName, int TSNDLVL, bool isBGM, int flags, float vol, int pitch) {
  if (!IsValidClient(client)) return;
  if (isBGM && (soundPreference[client] == 1 || soundPreference[client] == 3) || !isBGM && soundPreference[client] >= 2) EmitSoundToClient(client, sndName, _, isBGM ? g_chanBGM : 0, GetSNDLVL(client, isBGM, TSNDLVL), flags, GetVolume(client, isBGM, vol), pitch, _, _, _, _, _);
}

// Play sound to client. Now supports source entities, channels, and positions.
void CSEClientEx(int client, char[] sndName, int entity = -2, int channel, int TSNDLVL, bool isBGM, int flags, float vol, int pitch, float pos[3] = NULL_VECTOR) {
  if (!IsValidClient(client)) return;
  if (isBGM && (soundPreference[client] == 1 || soundPreference[client] == 3) || !isBGM && soundPreference[client] >= 2) EmitSoundToClient(client, sndName, entity, channel, GetSNDLVL(client, isBGM, TSNDLVL), flags, GetVolume(client, isBGM, vol), pitch, _, pos, _, _, _);
}

// Gets the client's preferred sound level - see the Valve Developer Wiki for valid SNDLVL indexes.
public int GetSNDLVL(int client, bool isBGM, int _default) { return isBGM ? (AudioState[client].volume != 0 ? AudioState[client].volume : _default) : !isBGM ? (AudioState[client].volume_sfx != 0 ? AudioState[client].volume_sfx : _default) : _default; }

// Gets the client's preferred REAL sound level - between 0.0 and 1.0;
public float GetVolume(int client, bool isBGM, float _default) { return isBGM ? (AudioState[client].volume_real != 0.0 ? AudioState[client].volume_real : _default) : !isBGM ? (AudioState[client].volume_real_sfx != 0.0 ? AudioState[client].volume_real_sfx : _default) : _default; }

// VIP Music Menu
public Action Command_Music(int client, int args) {
  int steamID = GetSteamAccountID(client);
  if (!steamID || steamID <= 10000) return Plugin_Handled;
  else ShowHikariMusicMenu(client);
  return Plugin_Handled;
}

// VIP Music Menu
public void ShowHikariMusicMenu(int client) {
  Menu menu = new Menu(MenuHandlerHikariMusic, MENU_ACTIONS_DEFAULT);
  char buffer[100];
  menu.SetTitle("Hikari's Music Menu");
  for (int i = 0; i < sizeof(BGMArray); i++) menu.AddItem(buffer, BGMArray[i].songName);
  menu.Display(client, MENU_TIME_FOREVER);
  menu.ExitButton = true;
}

// VIP Music Menu Handler
public int MenuHandlerHikariMusic(Menu menu, MenuAction action, int client, int bgm) {
  if (action == MenuAction_Select) {
    AssLogger(LOGLVL_WARN, "WE SHOULDNT BE USING TACO BELL BGM INDEX ANYMORE DUE TO NEW CONFIGS!!!");
    CPrintToChat(client, (bgm == 0 ? "{darkgreen}[CORE] Confirmed. Next song set to {aqua}Default{darkgreen}." : "{limegreen}[CORE] Confirmed. Next song set to {aqua}%s{limegreen}."), BGMArray[bgm].songName);
    AudioManager.setBGM_VIP(bgm == 0 ? -1 : bgm);
    for (int i = 0; i < MaxClients; ++i) { AudioState[i].stopBGM = (!StrEqual(AudioState[i].cachedPath, BGMArray[bgm].realPath) ? true : false); AudioState[i].VIPIndex = client; }
  } else if (action == MenuAction_End) CloseHandle(menu);
  return 0;
}

//Set Hikari Sound menu
public void HikarisSNDSelected(int client, MenuAction action, any info, char[] buffer, int maxlen) {
  if (action == MenuAction_Select) ShowHikariMenu(client);
}

// Get clients sound preferences then send them the menu
public Action Command_Sounds(int client, int args) {
  int steamID = GetSteamAccountID(client);
  if (!steamID || steamID <= 10000) return Plugin_Handled;
  else {
    char queryID[256];
    Format(queryID, sizeof(queryID), "SELECT soundprefs, sirenenabled, sirenmutesmusic FROM ass_activity WHERE steamid = '%d';", steamID);
    Ass_Database.Query(SQL_SNDPrefs, queryID, client);
    ShowHikariMenu(client);
    PrintToChat(client, "%s | Tornado Siren Enabled: %s | Tornado Siren Mutes Music: %s.", sndPrefs[soundPreference[client]], g_SirenEnabled[client] ? "True" : "False", g_SirenMutesMusic[client] ? "True" : "False");
    return Plugin_Handled;
  }
}

//Send client sound menu
public void ShowHikariMenu(int client) {
  Menu menu = new Menu(MenuHandlerHikari, MENU_ACTIONS_DEFAULT);
  char buffer[100];
  menu.SetTitle("Hikari's Sound Menu");
  menu.AddItem(buffer, "Disable ALL");
  menu.AddItem(buffer, "Music Only");
  menu.AddItem(buffer, "Sound Effects Only");
  menu.AddItem(buffer, "Enable ALL");
  menu.AddItem(buffer, g_SirenEnabled[client] ? "Disable Tornado Siren" : "Enable Tornado Siren")
  menu.AddItem(buffer, g_SirenMutesMusic[client] ? "Siren Mutes Music" : "Siren Does NOT Mute Music");
  menu.Display(client, 20);
  menu.ExitButton = true;
}

// Handle client choices for sound preference
public int MenuHandlerHikari(Menu menu, MenuAction action, int param1, int param2) {
  if (action == MenuAction_Select) {
    char query[256];
    int steamID = GetSteamAccountID(param1);
    if (!Ass_Database || !steamID) return 0;
    if (param2 < 4) {
      Format(query, sizeof(query), "UPDATE ass_activity SET soundprefs = '%i' WHERE steamid = '%d';", param2, steamID);
      soundPreference[param1] = param2;
    }
    if (param2 == 4) { Format(query, sizeof(query), "UPDATE ass_activity SET sirenenabled = CASE sirenenabled WHEN 0 THEN 1 ELSE 0 END WHERE steamid = '%d';", steamID); g_SirenEnabled[param1] = !g_SirenEnabled[param1] ? true : false; }
    if (param2 == 5) { Format(query, sizeof(query), "UPDATE ass_activity SET sirenmutesmusic = CASE sirenmutesmusic WHEN 0 THEN 1 ELSE 0 END WHERE steamid = '%d';", steamID); g_SirenMutesMusic[param1] = !g_SirenMutesMusic[param1] ? true : false; }
    Ass_Database.Query(Database_FastQuery, query);
    Command_Sounds(param1, 0);
  }
  else if (action == MenuAction_End) CloseHandle(menu);
  return 0;
}

//Light Entities
bool g_PowerOutage;
enum struct MAPLIGHTING {
  bool hasCustomPattern;
  bool isBroken;
  char arcs[32];
  char beam[32];
  char buzz[32];
  char explosion[35];
  char light[32];
  char status[32];
  char stun[32];
  char zap[32];
  void Explode() {
    if (this.isBroken) return;
    this.isBroken = true;
    FastFire2.Execute(this.arcs, "StartSpark", "", 0.0);
    FastFire2.Execute(this.arcs, "StartSpark", "", 0.1);
    FastFire2.Execute(this.beam, "LightOff", "", 0.0);
    FastFire2.Execute(this.buzz, "PlaySound", "", 0.0);
    FastFire2.Execute(this.explosion, "Explode", "", 0.0);
    FastFire2.Execute(this.light, "TurnOff", "", 0.0);
    FastFire2.Execute(this.status, "Color", "255 0 0", 0.0);
    FastFire2.Execute(this.stun, "Enable", "", 0.0);
    FastFire2.Execute(this.zap, "Enable", "", 0.0);
  }
  void Repair() {
    this.isBroken = false;
    FastFire2.Execute(this.arcs, "StopSpark", "", 0.0);
    FastFire2.Execute(this.buzz, "StopSound", "", 0.25);
    FastFire2.Execute(this.status, "Color", "0 255 0", 0.0);
    FastFire2.Execute(this.stun, "Disable", "", 0.0);
    FastFire2.Execute(this.zap, "Disable", "", 0.0);
    if (!g_PowerOutage) {
      FastFire2.Execute(this.light, "SetPattern", "", 0.0);
      FastFire2.Execute(this.light, "TurnOff", "", 0.0);
      FastFire2.Execute(this.light, "TurnOn", "", 0.1);
      FastFire2.Execute(this.light, "SetPattern", "", 0.1);
      FastFire2.Execute(this.beam, "LightOff", "", 0.1);
      FastFire2.Execute(this.beam, "LightOn", "", 0.2);
    }
  }
  void RestorePower() {
    if (!this.isBroken || strcmp(this.light, "MapLighting.StreetLamp0C.light") == 0) {
      FastFire2.Execute(this.light, "TurnOn", "", 0.0);
      FastFire2.Execute(this.beam, "LightOn", "", 0.0);
      FastFire2.Execute(this.beam, "LightOn", "", 1.0);
      FastFire2.Execute(this.beam, "LightOn", "", 3.0);
      FastFire2.Execute(this.light, "SetPattern", "", 0.0);
      this.hasCustomPattern = false;
    }
  }
}
MAPLIGHTING MapLighting[13];

//Ghosties
methodmap GHOSTHANDLER {
  public void SpawnRandom() {
    FastFire2.Execute("GhostTemplate", "ForceSpawn");
    switch (GetRandomInt(0, 3)) {
      case 0: { FastFire2.Execute("gTrain", "TeleportToPathTrack", "gPath00_0"); }
      case 1: { FastFire2.Execute("gTrain", "TeleportToPathTrack", "gPath01_0"); }
      case 2: { FastFire2.Execute("gTrain", "TeleportToPathTrack", "gPath02_0"); }
      case 3: { FastFire2.Execute("gTrain", "TeleportToPathTrack", "gPath03_0"); }
    }
  }
  public void Kill() {
    FastFire2.Execute("gTelefect", "Start", "", 3.0);
    FastFire2.Execute("gTelefect", "Stop", "", 6.0);
    FastFire2.Execute("gTrain", "TeleportToPathTrack", "gPathHome", 3.0);
    FastFire2.SetEntityLife("gTrain", 7.0);
    FastFire2.SetEntityLife("gTrainTriggerStun", 7.0);
    FastFire2.SetEntityLife("gGhost", 7.0);
    FastFire2.SetEntityLife("gTelefect", 7.0);
    FastFire2.SetEntityLife("gTrainSnd*", 7.0);
    FastFire2.SetEntityLife("gPath0*", 7.0);
  }
}
GHOSTHANDLER GhostHandler;

//Weather Manager
int sirenRef;
BULKFIRE lightningStrike[16];
BULKFIRE lightningFlash[17];
enum struct WEATHERMANAGER {
  bool canTornado;
  bool hasTornado;
  bool powerSurging;
  bool reset;
  bool sirenExploded;
  bool tickWeather;
  bool TornadoTimerActive;
  bool TornadoWarning;
  char defFogStartDist[5];
  char defFogEndDist[5];
  char defRainDensity;
  char rainDensity;
  float fogDensity;
  float defFogDensity;
  float fogTarget;
  float fogChangeRate;
  float fogChangeRateRGB;
  float sirenPitch;
  float sirenPitchRate;
  float sirenPitchTarget;
  float fogColorR;
  float fogColorG;
  float fogColorB;
  float fogColorRTarget;
  float fogColorGTarget;
  float fogColorBTarget;
  float sirenPos[3];
  float skyHeight;
  int intensity;
  int mIntensity;
  int iStormMin;
  int iStormMax;
  int ticks;
  int ticksSiren;
  bool CanFormTornado() {
    return WaveSystem().IsActive() && this.canTornado && !this.hasTornado;
  }
  void Activate() {
    this.Reset();
    if (!WaveSystem().IsWaveNull()) this.tickWeather = true; // Start weather system ONLY IF NOT IN GAMEMODE 2.
  }
  void doLightning() {
    sudo(1003);
    FastFire2.Execute(lightningStrike[GetRandomInt(0, sizeof(lightningStrike) - 1)].fireStr);
    FastFire2.Execute("Weather.LightningHurt*", "Disable", "", 0.07);
    CustomSoundEmitter(SFXArray[GetRandomInt(27, 34)].realPath, 65, false, 0, 1.0, 100);
    // Affect lights
    int i = GetRandomInt(this.intensity, 15);
    AssLogger(LOGLVL_DEBUG, "Doing light interactions with %i", i);
    switch (i) {
      case 2, 3, 5: {
        int index = GetRandomInt(0, 12);
        float f = GetRandomFloat(0.5, 3.0);
        FastFire2.Execute(MapLighting[index].light, "TurnOff");
        FastFire2.Execute(MapLighting[index].beam, "LightOff");
        FastFire2.Execute(MapLighting[index].light, "TurnOn", "", f);
        FastFire2.Execute(MapLighting[index].beam, "LightOn", "", f);
      }
      case 11, 15: {
        g_PowerOutage = true;
        if (!this.hasTornado)CreateTimer(GetRandomFloat(5.0, 30.0), RestorePower, GetRandomInt(0, 3));
        FastFire2.Execute("MapLighting.*", "LightOff");
        FastFire2.Execute("MapLighting.*", "TurnOff");
        FastFire2.Execute("MapLighting.*", "LightOff", "", 1.0);
        FastFire2.Execute("MapLighting.*", "LightOff", "", 2.0);
        FastFire2.Execute("MapLighting.*", "LightOff", "", 3.0);
        FastFire2.Execute("MapLighting.*", "LightOff", "", 4.0);
        FastFire2.Execute("MapLighting.*", "LightOff", "", 5.0);
      }
    }
  }
  void Dissipate() {
    this.sirenPitchTarget = 0.0;
    this.sirenPitchRate = 0.25;
    this.fogTarget = this.defFogDensity;
    g_PowerOutage = false;
    this.hasTornado = false;
    if (WaveSystem().IsActive()) {
      this.mIntensity = 5;
      this.intensity = 7;
    } else {
      this.mIntensity = 0;
      this.intensity = 0;
    }
    if (this.sirenExploded) {
      this.sirenPitch = 0.0;
      this.sirenExploded = false;
    }
    this.TornadoWarning = false;
    CreateTimer(GetRandomFloat(45.0, 135.0), ResetTornado);
    for (int i = 0; i < sizeof(MapLighting); i++)if (!MapLighting[i].isBroken)FastFire2.Execute(MapLighting[i].light, "TurnOn");
    FastFire2.Execute("Weather.FogOutdoor", "SetStartDistLerp", this.defFogStartDist);
    FastFire2.Execute("Weather.FogOutdoor", "SetEndDistLerp", this.defFogEndDist);
    FastFire2.Execute("Weather.FogOutdoor", "StartFogTransition");
    FastFire2.Execute("tornadof1", "Stop");
    FastFire2.Execute("TornadoKill", "Disable");
    FastFire2.Execute("tornadof1wind", "Disable");
    FastFire2.Execute("tornadowindf1", "StopSound");
    FastFire2.Execute("shaketriggerf1", "Disable");
    FastFire2.Execute("tornadobutton", "Unlock", "", 30.0);
    FastFire2.Execute("FB.FakeTankTank01", "Kill");
    FastFire2.Execute("FB.FakeTankPhys01", "Kill");
    FastFire2.Execute("Weather.EASScreen", "Disable");
    FastFire2.Execute("Weather.Siren.ExplodeParticle", "Stop");
    FastFire2.Execute("Weather.Siren", "StopSound", "", 5.0);
    AudioManager.OnTornadoWarning(false);
  }
  // Gets a safe tick delay to prevent divide by 0 errors / stal5ls
  int getTickDelay() { return GetRandomInt(this.iStormMin < 250 ? 250 : this.iStormMin, this.iStormMax < 300 ? 300 : this.iStormMax); }
  void IssueTornadoWarning() {
    if (WaveSystem().IsActive() && this.canTornado && !this.sirenExploded) {
      this.TornadoWarning = true;
      this.sirenPitchTarget = 100.0;
      this.sirenPitchRate = 0.1;
      FastFire2.Execute("Weather.EASScreen", "Enable");
      FastFire2.Execute("Weather.EAS", "PlaySound");
      AudioManager.OnTornadoWarning(true);
    }
  }
  void Reset() {
    AssLogger(LOGLVL_DEBUG, "[Hikari's Enhancer] WeatherManager has been reset!");
    this.fogDensity = 0.50;
    this.fogColorRTarget = 35.0;
    this.fogColorGTarget = 55.0;
    this.fogColorBTarget = 55.0;
    this.Dissipate();
    this.reset = true;
    this.tickWeather = false;
    FastFire2.Execute("rain", "Alpha", "0");
    FastFire2.Execute("Weather.Sky", "Enable");
    FastFire2.Execute("Weather.Sky", "Skin", "0");
    FastFire2.Execute("Weather.FogSky", "Enable", "", 2.0);
    for (int i = 0; i < sizeof(MapLighting); i++)MapLighting[i].Repair();
    this.sirenPos = {-2305.0, 2095.0, 1102.0};
    int siren = FindEntityByTargetname("WeatherSiren", "info_target");
    if (!IsValidEntity(siren)) {
      siren = CreateEntityByName("info_target");
      if (!IsValidEdict(siren)) SetFailState("Could not inject Siren Entity, aborting!!!");
      DispatchSpawn(siren);
      DispatchKeyValue(siren, "targetname", "WeatherSiren");
      ActivateEntity(siren);
      TeleportEntity(siren, this.sirenPos, NULL_VECTOR, NULL_VECTOR);
      AssLogger(LOGLVL_INFO, "Injected info_target WeatherSiren to map!");
    }
    sirenRef = EntIndexToEntRef(siren);
  }
  void SetFogStartQueued(const char[] fsq) { FastFire2.Execute("Weather.FogOutdoor", "SetStartDistLerpTo", fsq); }
  void SetFogEndQueued(const char[] feq) { FastFire2.Execute("Weather.FogOutdoor", "SetEndDistLerpTo", feq); }
  void StartFogTransition() { FastFire2.Execute("Weather.FogOutdoor", "StartFogTransition"); }
  void StartTornadoTimer(float t) {
    CreateTimer(t - 20.0, TimedOperator, 40);
    CreateTimer((t + GetRandomFloat(5.0, 25.0)), TimedOperator, 41);
  }
  void Tick() {
    if (this.TornadoWarning) this.TickSiren();
    if (++this.ticks % this.getTickDelay() == 0) {
      this.ticks = 0;
      if (!this.tickWeather) return;
      AssLogger(LOGLVL_DEBUG, "[Hikari's Enhancer] Storm Intensity @ %f% (%i)", (this.intensity / 15.0), this.intensity);
      this.doLightning();
      this.updateIntensity();
      switch (GetRandomInt(this.mIntensity, this.intensity)) {
        case 0: {
          this.SetFogStartQueued("500");
          this.SetFogEndQueued("3000");
          this.StartFogTransition();
          this.iStormMin = 1000;
          this.iStormMax = 3000;
          this.fogTarget = 0.55;
          FastFire2.Execute("rain", "Alpha", "15");
          CustomSoundEmitter(SFXArray[108].realPath, 65, false, 1, 0.1, 100); //Rain, no wind, reduced volume
        }
        case 1: {
          this.SetFogStartQueued("500");
          this.SetFogEndQueued("2700");
          this.StartFogTransition();
          this.iStormMin = 800;
          this.iStormMax = 2700;
          this.fogTarget = 0.60;
          FastFire2.Execute("rain", "Alpha", "30");
          CustomSoundEmitter(SFXArray[108].realPath, 65, false, 1, 0.1, 100); //Rain, no wind, reduced volume
        }
        case 2: {
          this.SetFogStartQueued("500");
          this.SetFogEndQueued("2500");
          this.StartFogTransition();
          this.mIntensity = 1;
          this.iStormMin = 700;
          this.iStormMax = 2500;
          FastFire2.Execute("rain", "Alpha", "50");
          CustomSoundEmitter(SFXArray[108].realPath, 65, false, 1, 0.1, 100); //Rain, no wind, increased volume
        }
        case 3: {
          this.SetFogStartQueued("500");
          this.SetFogEndQueued("2500");
          this.StartFogTransition();
          this.iStormMin = 600;
          this.iStormMax = 2200;
          this.fogTarget = 0.70;
          FastFire2.Execute("rain", "Alpha", "65");
          CustomSoundEmitter(SFXArray[108].realPath, 65, false, 1, 0.1, 100); //Rain, no wind, increased volume
        }
        case 4: {
          this.iStormMin = 500;
          this.iStormMax = 2000;
          FastFire2.Execute("rain", "Alpha", "75");
          CustomSoundEmitter(SFXArray[108].realPath, 45, false, 1, 1.0, 100); //Rain, no wind, increased volume
          CustomSoundEmitter(SFXArray[107].realPath, 65, false, 1, 0.1, 100); //Rain, wind, decreased volume
          //CustomSoundEmitter(SFXArray[109].realPath, 45, false, 1, 0.01, 100); //Extra wind
        }
        case 5: {
          this.SetFogStartQueued("500");
          this.SetFogEndQueued("2100");
          this.StartFogTransition();
          this.mIntensity = 3;
          this.iStormMin = 450;
          this.iStormMax = 1750;
          this.fogTarget = 0.75;
          FastFire2.Execute("rain", "Alpha", "95");
          CustomSoundEmitter(SFXArray[108].realPath, 65, false, 1, 0.1, 100); //Rain, no wind, decreased volume
          CustomSoundEmitter(SFXArray[107].realPath, 45, false, 1, 0.15, 100); //Rain, wind
          //CustomSoundEmitter(SFXArray[109].realPath, 45, false, 1, 0.05, 100); //Extra wind
        }
        case 6: {
          this.mIntensity = 4;
          this.iStormMin = 400;
          this.iStormMax = 1650;
          FastFire2.Execute("rain", "Alpha", "125");
          CustomSoundEmitter(SFXArray[108].realPath, 65, false, 1, 0.05, 100); //Rain, no wind, decreased volume
          CustomSoundEmitter(SFXArray[107].realPath, 65, false, 1, 0.21, 100); //Rain, wind, increased volume
          //CustomSoundEmitter(SFXArray[109].realPath, 45, false, 1, 0.125, 100); //Extra wind
        }
        case 7: {
          this.SetFogStartQueued("500");
          this.SetFogEndQueued("2000");
          this.StartFogTransition();
          this.iStormMin = 375;
          this.iStormMax = 1550;
          this.fogTarget = 0.80;
          FastFire2.Execute("rain", "Alpha", "155");
          CreateTimer(GetRandomFloat(0.1, 1.25), GetRandomWind, 0);
          CustomSoundEmitter(SFXArray[108].realPath, 65, false, 1, 0.05, 100); //Rain, no wind, decreased volume
          CustomSoundEmitter(SFXArray[107].realPath, 65, false, 1, 0.26, 100); //Rain, wind, increased volume
          //CustomSoundEmitter(SFXArray[109].realPath, 45, false, 1, 0.25, 100); //Extra wind
        }
        case 8: {
          this.mIntensity = 6;
          this.SetFogStartQueued("470");
          this.SetFogEndQueued("2000");
          this.StartFogTransition();
          this.iStormMin = 325;
          this.iStormMax = 1450;
          this.fogTarget = 0.875;
          FastFire2.Execute("rain", "Alpha", "175");
          CreateTimer(GetRandomFloat(0.1, 1.25), GetRandomWind, 1);
          CustomSoundEmitter(SFXArray[107].realPath, 65, false, 1, 0.37, 100); //Rain, wind, increased volume
          //CustomSoundEmitter(SFXArray[109].realPath, 45, false, 1, 0.35, 100); //Extra wind
        }
        case 9: {
          this.SetFogStartQueued("425");
          this.SetFogEndQueued("1500");
          this.StartFogTransition();
          this.mIntensity = 7;
          this.iStormMin = 300;
          this.iStormMax = 1375;
          FastFire2.Execute("rain", "Alpha", "200");
          CreateTimer(GetRandomFloat(0.1, 1.25), GetRandomWind, 2);
          CustomSoundEmitter(SFXArray[107].realPath, 65, false, 1, 0.41, 100); //Rain, wind, increased volume
          //CustomSoundEmitter(SFXArray[109].realPath, 45, false, 1, 0.42, 100); //Extra wind
        }
        case 10: {
          this.SetFogStartQueued("375");
          this.SetFogEndQueued("1500");
          this.StartFogTransition();
          this.iStormMin = 300;
          this.iStormMax = 1300;
          this.fogTarget = 0.925;
          FastFire2.Execute("rain", "Alpha", "215", 0.0);
          CreateTimer(GetRandomFloat(0.1, 1.25), GetRandomWind, 3);
          CustomSoundEmitter(SFXArray[107].realPath, 65, false, 1, 0.45, 100); //Rain, wind, increased volume
          //CustomSoundEmitter(SFXArray[109].realPath, 45, false, 1, 0.56, 100); //Extra wind
        }
        case 11: {
          this.mIntensity = 9;
          this.iStormMin = 300;
          this.iStormMax = 1200;
          FastFire2.Execute("rain", "Alpha", "235");
          CreateTimer(GetRandomFloat(0.1, 1.25), GetRandomWind, 4);
          CustomSoundEmitter(SFXArray[107].realPath, 65, false, 1, 0.63, 100); //Rain, wind, increased volume
          //CustomSoundEmitter(SFXArray[109].realPath, 45, false, 1, 0.69, 100); //Extra wind
        }
        case 12: {
          this.mIntensity = 10;
          this.iStormMin = 300;
          this.iStormMax = 1150;
          this.fogTarget = 1.0;
          this.SetFogStartQueued("225");
          this.SetFogEndQueued("700");
          this.StartFogTransition();
          FastFire2.Execute("rain", "Alpha", "255");
          CreateTimer(GetRandomFloat(0.1, 1.25), GetRandomWind, 5);
          CustomSoundEmitter(SFXArray[107].realPath, 65, false, 1, 0.72, 100); //Rain, wind, increased volume
          //CustomSoundEmitter(SFXArray[109].realPath, 45, false, 1, 0.76, 100); //Extra wind
          if (!this.TornadoTimerActive) sudo(1004);
        }
        case 13: {
          this.SetFogStartQueued("200");
          this.SetFogEndQueued("700");
          this.StartFogTransition();
          this.mIntensity = 12;
          this.iStormMin = 300;
          this.iStormMax = 1000;
          this.fogTarget = 1.0;
          FastFire2.Execute("rain", "Alpha", "255");
          CreateTimer(GetRandomFloat(0.1, 1.25), GetRandomWind, 6);
          CustomSoundEmitter(SFXArray[107].realPath, 65, false, 1, 0.85, 100); //Rain, wind, increased volume
          //CustomSoundEmitter(SFXArray[109].realPath, 45, false, 1, 0.83, 100); //Extra wind
          if (!this.TornadoTimerActive) sudo(1004);
        }
        case 14: {
          this.mIntensity = 13;
          this.SetFogStartQueued("100");
          this.SetFogEndQueued("700");
          this.StartFogTransition();
          this.iStormMin = 300;
          this.iStormMax = 850;
          this.fogTarget = 1.0;
          FastFire2.Execute("rain", "Alpha", "255");
          CreateTimer(GetRandomFloat(0.1, 1.25), GetRandomWind, 7);
          CustomSoundEmitter(SFXArray[107].realPath, 65, false, 1, 0.9, 100); //Rain, wind, increased volume
          //CustomSoundEmitter(SFXArray[109].realPath, 45, false, 1, 0.92, 100); //Extra wind
          if (!this.TornadoTimerActive) sudo(1004);
        }
        case 15: {
          this.mIntensity = 14;
          this.SetFogStartQueued("50");
          this.SetFogEndQueued("500");
          this.StartFogTransition();
          this.iStormMin = 300;
          this.iStormMax = 700;
          this.fogTarget = 1.0;
          FastFire2.Execute("rain", "Alpha", "255");
          CreateTimer(GetRandomFloat(0.1, 1.25), GetRandomWind, 7);
          CustomSoundEmitter(SFXArray[107].realPath, 65, false, 1, 1.0, 100); //Rain, wind, increased volume
          //CustomSoundEmitter(SFXArray[109].realPath, 45, false, 1, 1.0, 100); //Extra wind
          if (!this.TornadoTimerActive) sudo(1004);
        }
        default: {}
      }
    }
    if (this.fogDensity != this.fogTarget || this.reset) {
      char targetAlpha[4];
      char targetDensity[24];
      this.fogDensity = (this.fogDensity < this.fogTarget) ? FloatMin(Maths().Adjust(this.fogDensity, this.fogChangeRate), this.fogTarget) : FloatMax(Maths().Adjust(this.fogDensity, -this.fogChangeRate), this.fogTarget);
      IntToString(RoundFloat(FloatMin(255.0 * (this.fogDensity * 1.20), 255.0)), targetAlpha, sizeof(targetAlpha));
      FloatToString(this.fogDensity, targetDensity, sizeof(targetDensity));
      FastFire2.Execute("Weather.FogOutdoor", "SetMaxDensity", targetDensity);
      FastFire2.Execute("Weather.FogSky", "Alpha", targetAlpha);
    }
    if (this.fogColorR != this.fogColorRTarget || this.fogColorG != this.fogColorGTarget || this.fogColorB != this.fogColorBTarget || this.reset) {
      char target[24];
      this.fogColorR = (this.fogColorR < this.fogColorRTarget) ? FloatMin(this.fogColorR += this.fogChangeRateRGB, this.fogColorRTarget) : FloatMax(this.fogColorR -= this.fogChangeRateRGB, this.fogColorRTarget);
      this.fogColorG = (this.fogColorG < this.fogColorGTarget) ? FloatMin(this.fogColorG += this.fogChangeRateRGB, this.fogColorGTarget) : FloatMax(this.fogColorG -= this.fogChangeRateRGB, this.fogColorGTarget);
      this.fogColorB = (this.fogColorB < this.fogColorBTarget) ? FloatMin(this.fogColorB += this.fogChangeRateRGB, this.fogColorBTarget) : FloatMax(this.fogColorB -= this.fogChangeRateRGB, this.fogColorBTarget);
      Format(target, sizeof(target), "%i %i %i", RoundFloat(this.fogColorR), RoundFloat(this.fogColorG), RoundFloat(this.fogColorB));
      FastFire2.Execute("Weather.FogIndoor", "SetColor", target);
      FastFire2.Execute("Weather.FogIndoor", "SetColorSecondary", target);
      FastFire2.Execute("Weather.FogOutdoor", "SetColor", target);
      FastFire2.Execute("Weather.FogOutdoor", "SetColorSecondary", target);
      FastFire2.Execute("Weather.FogSky", "Color", target);
      this.reset = false;
    }
    if (this.ticks % 240 == 0) for (int i = 1; i < MaxClients; i++) { if (!IsClientInGame(i) || !IsClientConnected(i)) continue; else { SetVariantString(g_IsPlayerIndoors[i] ? "Weather.FogIndoor" : "Weather.FogOutdoor"); AcceptEntityInput(i, "SetFogController", i, i); }}
    if (this.hasTornado) { AttackAllEnts(9000000.0, DMG_FALL, g_WeatherZone[0]); this.TickTornado(); }
  }
  void TickSiren() {
    if (this.TornadoWarning) {
      if (!this.sirenExploded && (this.sirenPitch > 165.0)) {
        FastFire2.Execute("Weather.Siren.ExplodeParticle", "Start");
        CustomSoundEmitter(SFXArray[121].realPath, SFXArray[121].SNDLVL, false, 0, 1.0, 100);
        for (int i = 0; i < MaxClients; i++) if (IsValidClient(i) && g_SirenEnabled[i]) {
          StopSound(i, 10, SFXArray[119].realPath);
          StopSound(i, 10, SFXArray[120].realPath);
          StopSound(i, 3, SFXArray[119].realPath);
          StopSound(i, 3, SFXArray[120].realPath);
          StopSound(i, 2, SFXArray[119].realPath);
          StopSound(i, 2, SFXArray[120].realPath);
          StopSound(i, 6, SFXArray[119].realPath);
          StopSound(i, 6, SFXArray[120].realPath);
        }
        FastFire2.Execute("Weather.Siren.ExplodeParticle", "Stop", "", 5.0);
        FastFire2.Execute("Weather.Siren.ExplodeParticle", "Start", "", 5.1);
        this.sirenExploded = true;
      }
      if (this.sirenPitch != this.sirenPitchTarget && !this.sirenExploded) {
        char targetPitch[24];
        this.sirenPitch = (this.sirenPitch < this.sirenPitchTarget) ? FloatMin(this.sirenPitch += this.sirenPitchRate, this.sirenPitchTarget) : FloatMax(this.sirenPitch -= this.sirenPitchRate, this.sirenPitchTarget);
        FloatToString(this.sirenPitch, targetPitch, sizeof(targetPitch));
        int siren = EntRefToEntIndex(sirenRef);
        for (int i = 0; i < MaxClients; i++) if (IsValidClient(i) && g_SirenEnabled[i]) {
          CSEClientEx(i, SFXArray[119].realPath, siren, 10, SNDLEVEL_RAIDSIREN, false, 1026, 1.0, RoundFloat(this.sirenPitch), this.sirenPos); 
          CSEClientEx(i, SFXArray[120].realPath, siren, 10, SNDLEVEL_RAIDSIREN, false, 1026, 1.0, RoundFloat(this.sirenPitch), this.sirenPos);
          CSEClientEx(i, SFXArray[119].realPath, siren, 3, SNDLEVEL_RAIDSIREN, false, 1026, 1.0, RoundFloat(this.sirenPitch), this.sirenPos); 
          CSEClientEx(i, SFXArray[120].realPath, siren, 3, SNDLEVEL_RAIDSIREN, false, 1026, 1.0, RoundFloat(this.sirenPitch), this.sirenPos);
          CSEClientEx(i, SFXArray[119].realPath, siren, 2, SNDLEVEL_RAIDSIREN, false, 1026, 1.0, RoundFloat(this.sirenPitch), this.sirenPos); 
          CSEClientEx(i, SFXArray[120].realPath, siren, 2, SNDLEVEL_RAIDSIREN, false, 2, 1.0, RoundFloat(this.sirenPitch), this.sirenPos);
          CSEClientEx(i, SFXArray[119].realPath, siren, 6, SNDLEVEL_RAIDSIREN, false, 2, 0.7, RoundFloat(this.sirenPitch), this.sirenPos); 
          CSEClientEx(i, SFXArray[120].realPath, siren, 6, SNDLEVEL_RAIDSIREN, false, 2, 1.0, RoundFloat(this.sirenPitch), this.sirenPos);
        }
      }
      if (++this.ticksSiren % GetRandomInt(70, 224) == 0) {
        this.ticksSiren = 0;
        if (this.TornadoWarning && !this.sirenExploded) {
          this.sirenPitchTarget = this.powerSurging ? GetRandomFloat(115.0, 210.0) : GetRandomFloat(65.0, 105.0);
          this.sirenPitchRate = this.powerSurging ? GetRandomFloat(0.0525, 0.25) : GetRandomFloat(0.01, 0.125);
        }
      }
    }
  }
  void TickTornado() {
    float tornadoPos[3];
    bool found = false;
    int tornado = -1;
    while ((tornado = FindEntityByTargetname("tornado", "func_tracktrain")) != -1) { GetEntPropVector(tornado, Prop_Send, "m_vecOrigin", tornadoPos); found = true; break; }
    if (!found) { PrintToServer("No tornado found."); return; }
    for (int i = 0; i <= GetMaxEntities(); i++) if (IsValidEntity(i)) if (HasEntProp(i, Prop_Send, "m_vecOrigin")) {
      float entPos[3]; GetEntPropVector(i, Prop_Send, "m_vecOrigin", entPos);
      float dx = tornadoPos[0] - entPos[0];
      float dy = tornadoPos[1] - entPos[1];
      float distance2D = SquareRoot(dx*dx + dy*dy);
      if (distance2D <= 200.0) { PrintToServer("<Enhancer:929> Found entity that matches search pattern <= 200.0D"); }
    }
  }
  void TryFormTornado() {
    if (this.CanFormTornado() && ICanSummon(7)) {
      this.hasTornado = true;
      this.mIntensity = 15;
      this.intensity = 15;
      CreateTimer(1.5, TornadoShaker);
      FastFire2.Execute("TornadoKill", "Enable");
      FastFire2.Execute("tornadobutton", "Lock");
      FastFire2.Execute("tornadof1", "Start", "", 20.0);
      FastFire2.Execute("shaketriggerf1", "Enable", "", 20.0);
      FastFire2.Execute("tornadowindf1", "PlaySound", "", 20.0);
      FastFire2.Execute("tornadof1wind", "Enable", "", 21.50);
      float f = GetRandomFloat(60.0, 120.0);
      CreateTimer(f, TimedOperator, 42);
      FastFire2.SetEntityLife("tornado", f);
      FastFire2.SetEntityLife("TorTrack*", f);
      FastFire2.SetEntityLife("tornadotrain", f);
      FastFire2.SetEntityLife("tornadof1", f);
      FastFire2.SetEntityLife("tornadoshake_f1", f);
      FastFire2.SetEntityLife("tornadowindf1", f);  
    }
  }
  void updateIntensity() {
    switch (GetRandomInt(0, 10)) {
      case 0, 4, 8: {
        if (this.intensity > 9) GhostHandler.SpawnRandom();
        if (this.intensity > 0 && (this.intensity - 1 > this.mIntensity)) this.intensity--;
      }
      case 1, 2, 3, 5, 6, 7, 9, 10: { if (this.intensity < 15) this.intensity++; }
    }
  }
  void OnStartup(int event, const char[] path) {
    switch (event){
      case 0:{
        char finalPath[PLATFORM_MAX_PATH];
        Format(finalPath, sizeof(finalPath), "%s/Weather", path);
        CreateDirectory(finalPath);
      }
    }

  }
}
WEATHERMANAGER WeatherManager;
WEATHERMANAGER GetWeatherManager() { return WeatherManager; }

//Timers
public Action GetRandomWind(Handle timer, int intensity) {
  CustomSoundEmitter(SFXArray[GetRandomInt(113, intensity <  3 ? 114 : intensity < 6 ? 115 : 116)].realPath, intensity < 2 ? 35 : intensity < 4 ? 42 : intensity < 6 ? 45 : 50, true, 1, 0.125, 100);
  return Plugin_Stop;
}

public Action ResetTornado(Handle timer) {
  WeatherManager.canTornado = true;
  WeatherManager.TornadoTimerActive = false;
  return Plugin_Stop;
}

public Action RestorePower(Handle timer, int surge) {
  g_PowerOutage = false;
  WeatherManager.powerSurging = ((surge == 1) ? true : false);
  if (surge == 1) {
    CreateTimer(1.0, SurgePower);
  }
  for (int i = 0; i < sizeof(MapLighting); i++) MapLighting[i].RestorePower();
  FastFire2.Execute("MapLighting.Indoor*", "TurnOn");
  FastFire2.Execute("MapLighting.Indoor*", "LightOn");
  return Plugin_Stop;
}

char LightingPatterns[][25] =  {
  /////Allistor/////
  "sduehrjkihwerte",
  "ihqopeiruhiqwer",
  "sadnpiudghsfiod",
  "kjahbfihkabweoi",
  "djfohoaeiufgawt",
  "ewrtyvghbvfczfr",
  "aesrergtafdcgvz",
  "aeradyjdghnyjxc",
  "oihaecpnefijanle",
  "oaihbewrpoijnae",
  //////Jeffy///////
  "gewsaadgfhgtfsr",
  "kuyijyterytdfsadfvgzs",
  "bvcxfgtertyaetr",
  "gyukkjtyasde",
  "bxcvmvbnhkgfj",
  "ewrhrtmhgjf",
  "bfgmghntjy",
  "afsdgdsfayrwte",
  "hfgjgfdiyet",
  "fsdbvxfgkytestry"
};

public Action SurgePower(Handle timer) {
  if (WeatherManager.powerSurging) {
    int target = GetRandomInt(0, 12);
    if (MapLighting[target].isBroken)target = GetRandomInt(0, 12);
    if (!MapLighting[target].isBroken) {
      if (GetRandomInt(0, 7) == 5)MapLighting[target].Explode();
      FastFire2.Execute(MapLighting[target].light, "SetPattern", LightingPatterns[GetRandomInt(0, 19)]);
      FastFire2.Execute(MapLighting[target].light, "TurnOn");
      FastFire2.Execute(MapLighting[target].beam, "LightOn");
      CreateTimer(GetRandomFloat(7.5, 15.0), RestorePower, 0);
      CreateTimer(GetRandomFloat(0.1, 3.0), SurgePower);
      CreateTimer(GetRandomFloat(1.5, 2.0), SurgePowerFX);
    }
  }
  return Plugin_Stop;
}

public Action SurgePowerFX(Handle timer) {
  for (int i = 0; i < sizeof(MapLighting); i++) {
    if (WeatherManager.powerSurging && !MapLighting[i].isBroken) {
      FastFire2.Execute(MapLighting[i].light, "SetPattern", LightingPatterns[GetRandomInt(0, 19)]);
      FastFire2.Execute(MapLighting[i].beam, "LightOff");
      FastFire2.Execute(MapLighting[i].beam, "LightOn", "", GetRandomFloat(0.1, 0.35));
      FastFire2.Execute(MapLighting[i].beam, "LightOff", "", GetRandomFloat(0.4, 0.75));
      FastFire2.Execute(MapLighting[i].beam, "LightOn", "", GetRandomFloat(1.0, 1.35));
    }
  }
  return Plugin_Stop;
}
public Action TornadoShaker(Handle timer) {
  if (WeatherManager.hasTornado) {
    FastFire2.Execute("tornadoshake_f1", "StartShake");
    CreateTimer(1.5, TornadoShaker);
  }
  return Plugin_Stop;
}

//Used to keep StopSound from stopping the music we're trying to play - now with support for loop offsets!
public Action SyncMusic(Handle timer, int client) {
  int index = AudioState[client].indexBGM;
  strcopy(AudioState[client].cachedPath, sizeof(AudioState[client].cachedPath), BGMArray[index].realPath);
  strcopy(AudioState[client].songName, sizeof(AudioState[client].songName), BGMArray[index].songName);
  AssLogger(LOGLVL_INFO, "[AudioState for %N]: We are on wave %i, now playing: %s (from %s) for %f seconds. It will start looping at %f seconds after the first cycle.", client, WaveSystem().GetWave(), AudioState[client].songName, AudioState[client].cachedPath, AudioState[client].loopSeconds, BGMArray[index].introSeconds);
  return Plugin_Stop;
}

public Action Command_SetVolume(int client, int args) {
  char arg1[4];
  GetCmdArg(1, arg1, sizeof(arg1));
  int steamID = GetSteamAccountID(client);
  if (!Ass_Database || !steamID || steamID <= 10000) return Plugin_Stop;
  //char query[256];
  //Format(query, sizeof(query), "SELECT * from ass_activity WHERE steamid = %i;", steamID);
  //Ass_Database.Query(SetVolume, query, client);
  AudioManager.SetVolume(client, StringToInt(arg1));
  return Plugin_Continue;
}

public Action Command_SetVolumeSFX(int client, int args) {
  char arg1[4];
  GetCmdArg(1, arg1, sizeof(arg1));
  int steamID = GetSteamAccountID(client);
  if (!Ass_Database || !steamID || steamID <= 10000) return Plugin_Stop;
  //char query[256];
  //Format(query, sizeof(query), "SELECT * from ass_activity WHERE steamid = %i;", steamID);
  //Ass_Database.Query(SetVolume, query, client);
  AudioManager.SetVolumeSFX(client, StringToInt(arg1));
  return Plugin_Continue;
}

public Action Command_SetVolumeReal(int client, int args) {
  char arg1[8];
  char arg2[4];
  GetCmdArg(1, arg1, sizeof(arg1));
  GetCmdArg(2, arg2, sizeof(arg2));
  PrintToChat(client, "%s, %s", arg1, arg2);
  if (StringToInt(arg2) > 100 | StringToInt(arg2) < 0) { ReplyToCommand(client, "ERROR: Value must be between 0 and 100."); return Plugin_Handled; }
  if (StrContains(arg1, "music", false) > -1) { AudioManager.SetVolumeReal(client, (StringToInt(arg2) * 1.0) / 100.0); return Plugin_Handled; }
  if (StrContains(arg1, "sfx", false)> -1) { AudioManager.SetVolumeRealSFX(client, (StringToInt(arg2) * 1.0) / 100.0); return Plugin_Handled; }
  ReplyToCommand(client, "Usage: sm_setvolumereal <sfx / music> <int 0-100>"); return Plugin_Handled;
}

enum struct LaserData {
  char tName[16];
  float sPos[3];
  float ePos[3];
}
LaserData WBLasers[3][28];

enum struct LaserColors {
  char cRGB[16];
}
LaserColors WBLaserColors[3];