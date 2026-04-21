/*             WELCOME TO HIKARI'S REQUIEM ROTTENBURG.
 *   A FEW THINGS TO KNOW: ONE.... THIS IS INTENDED TO BE USED WITH UBERUPGRADES.
 *   TWO..... SERVER OPERATORS MAY UPLOAD MUSIC TO BE USED WITH THIS PLUGIN. WE ARE NOT RESPONSIBLE FOR WHAT USERS UPLOAD TO THEIR SERVERS.
 *   THREE..... THIS MOD IS INTENDED FOR USE ON THE HYDROGENHOSTING SERVERS ONLY.
 *   FOUR..... THE DURATION OF MUSIC TIMERS SHOULD BE SET DEPENDING WHAT SONG IS USED. SET THIS USING THE CONFIG FILES. SONG DUR IN SECONDS / 0.0151515151515 = REFIRE TIME.
 *   FIVE..... TIPS AND TRICKS MAY BE ADDED TO THE TIMER, SEE PerformAdverts(Handle timer);
 *        IF IT'S WAR THAT YOU WANT, THEN I'M READY TO PLAY. GLHF!
 */
// Weather manager's sky height needs to be a zone (float 3 start end)...
public char PLUGIN_VERSION[8] = "10.0.0";
#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <tf2_stocks>
#include <fartsy/tf2_damagebits>
#include <fartsy/newcolors>
#include <fartsy/fastfire2>
#include <fartsy/hr_discord>
#include <fartsy/hr_database>
#include <fartsy/hr_serverutils>
#include <fartsy/hr_triggers>
#include <fartsy/hr_enhancer>
#include <fartsy/hr_asshop>
#include <fartsy/hr_bombstate>
#include <fartsy/hr_bosshandler>
#include <fartsy/hr_emergency>
#include <fartsy/hr_configsystem>
#include <fartsy/hr_helper>
#include <fartsy/hr_commands>
#include <fartsy/hr_events>
#include <fartsy/hr_sudo>
#include <fartsy/hr_wavesystem>
#include <tf2attributes>
#pragma newdecls required
#pragma semicolon 1
public Plugin myinfo = {
  name = "Hikari's MvM Framework",
  author = "Fartsy",
  description = "Framework for Hikari's Requiem (MvM Mods)",
  version = PLUGIN_VERSION,
  url = "https://wiki.hydrogenhosting.org"
};
//Check if extensions are loaded, send startup log
public void OnPluginStart() {
  if (GetExtensionFileStatus("smjansson.ext") != 1) { SetFailState("Required extension (smjansson) is not loaded!"); }
  AssLogger(LOGLVL_INFO, "Starting up Fartsy's Framework! Waiting for Map Start...");
}
//Begin executing IO when ready
public void OnFastFire2Ready() {
  AssLogger(LOGLVL_INFO, "####### FASTFIRE2 IS READY! INITIATE STARTUP SEQUENCE... PREPARE FOR THE END TIMES #######");
  core.init_pre();
  RegisterAndPrecacheAllFiles();
  RegisterAllCommands();
  HookAllEvents();
  WaveSystem().update();
  if (WaveSystem().IsDefault()) core.init_post();
  CPrintToChatAll("{fartsyred}Plugin Reloaded. If you do not hear music, please do !sounds and configure your preferences.");
  AudioManager.Reset(true);
  WeatherManager.Reset();
  AssLogger(LOGLVL_INFO, "####### STARTUP COMPLETE (v%s) #######", PLUGIN_VERSION);
}
//Process ticks and requests in real time
public void OnGameFrame() {
  if (WeatherManager.TornadoWarning) WeatherManager.TickSiren();
  AudioManager.TickGlobal();
  if (BossHandler.shouldTick) BossHandler.Tick();
  WaveSystem().Tick();
  WeatherManager.Tick();
  TickAllTriggers();
}