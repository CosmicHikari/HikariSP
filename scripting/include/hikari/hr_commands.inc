// Misc Commands
// Gets the client's statistics
public Action Command_MyStats(int client, int args) {
  int steamID = GetSteamAccountID(client);
  if (!Ass_Database || !steamID || steamID <= 10000) return Plugin_Stop;
  char queryID[256];Format(queryID, sizeof(queryID), "SELECT * from ass_activity WHERE steamid = %i;", steamID);
  Ass_Database.Query(MyStats, queryID, client);
  return Plugin_Continue;
}
public void MyStats(Database db, DBResultSet results, const char[] error, int client) {
  if (!results) { LogError("Failed to query database: %s", error); return; }
  char name[64], class[64], lastkilledname[128], lastusedweapon[128], killedbyname[128], killedbyweapon[128];
  int  health, healthMax, steamID, damagedealt, damagedealtsession, kills, killssession, deaths, deathssession, bombsreset, bombsresetsession, sacrifices, sacrificessession;
  if (results.FetchRow()) {
    results.FetchString(0, name, 64);                                   // name
    steamID = results.FetchInt(1);                                      // steamid
    results.FetchString(4, class, 64);                                  // class
    health             = results.FetchInt(5);                           // health
    healthMax          = results.FetchInt(6);                           // health
    damagedealt        = results.FetchInt(7);                           // damage dealt
    damagedealtsession = results.FetchInt(8);                           // damage dealt session
    kills              = results.FetchInt(9);                           // kills
    killssession       = results.FetchInt(10);                          // kills session
    deaths             = results.FetchInt(11);                          // deaths
    deathssession      = results.FetchInt(12);                          // deaths session
    bombsreset         = results.FetchInt(13);                          // bombs reset
    bombsresetsession  = results.FetchInt(14);                          // bombs reset session
    sacrifices         = results.FetchInt(15);                          // sacrifices
    sacrificessession  = results.FetchInt(16);                          // sacrifices session
    results.FetchString(17, lastkilledname, sizeof(lastkilledname));    // last client killed
    results.FetchString(18, lastusedweapon, sizeof(lastusedweapon));    // using weapon
    results.FetchString(19, killedbyname, sizeof(killedbyname));        // last client that killed
    results.FetchString(20, killedbyweapon, sizeof(killedbyweapon));    // using weapon
    CPrintToChat(client, "\x07AAAAAA[CORE] Showing stats of %s   [%s, %i/%i hp] || SteamID: %i ", name, class, health, healthMax, steamID);
    CPrintToChat(client, "{white}Damage Dealt: %i (Session: %i) || Kills: %i (Session: %i) || Deaths: %i (Session: %i) || Bombs Reset: %i (Session: %i)", damagedealt, damagedealtsession, kills, killssession, deaths, deathssession, bombsreset, bombsresetsession);
    CPrintToChat(client, "Sacrifices: %i(Session:%i) || Killed %s (using %s) || Last killed by: %s (using %s)", sacrifices, sacrificessession, lastkilledname, lastusedweapon, killedbyname, killedbyweapon);
  }
}

// Return the client to spawn
public Action Command_Return(int client, int args) {
  if (!IsPlayerAlive(client)) { CPrintToChat(client, "{red}[Core] You must be alive to use this command..."); return Plugin_Handled; }
  char name[128]; GetClientName(client, name, sizeof(name));
  CPrintToChatAll("{darkviolet}[{forestgreen}CORE{darkviolet}] {white}Client {red}%s {white}began casting {darkviolet}/return{white}.", name);
  CustomSoundEmitter(SFXArray[41].realPath, 65, false, 0, 1.0, 100);
  CreateTimer(5.0, ReturnClient, client);
  return Plugin_Handled;
}
public Action ReturnClient(Handle timer, int clientID) {
  TeleportEntity(clientID, Return, NULL_VECTOR, NULL_VECTOR);
  CSEClient(clientID, SFXArray[42].realPath, 65, false, 0, 1.0, 100);
  return Plugin_Handled;
}

// Prints our Discord server to the client and shows MOTD panel to them
public Action Command_Discord(int client, int args) {
  CPrintToChat(client, "{darkviolet}[{forestgreen}CORE{darkviolet}] {white}Our Discord server URL is {darkviolet}https://discord.com/invite/ZfUUQWxCmw{white}.");
  ShowMOTDPanel(client, "Discord | Hydrogen Hosting Community", "https://discord.com/invite/RM35cmXfQg", MOTDPANEL_TYPE_URL);
  return Plugin_Handled;
}