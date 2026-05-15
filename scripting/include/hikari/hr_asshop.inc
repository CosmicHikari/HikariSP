// Hikari's Annihilation Supply Shop
char HRASS_VER[8] = "1.0.0";
// Annihilation supply shop entries
enum struct ASShop {
  char name[64];
  int price;
  int purchase;
}
ASShop ass[10];

int ass_sacPoints;
int ass_sacPointsMax;

// Set up the Annihilation Supply Shop
public void SetupASShop() {
  LightLogger(LOGLVL_INFO, "[ASShop v%s] Setting up the Annihilation Supply Shop...", HRASS_VER);
  RegConsoleCmd("sm_sacpoints", Command_SacrificePointShop, "Hikari's Annihilation Supply Shop");
  ass[0].name = "[10] Bath Salts";
  ass[1].name = "[20] Summon Goobbue or Kirby";
  ass[2].name = "[30] Robot Trap (Wave Fail-safe)";
  ass[3].name = "[40] Explosives Paradise";
  ass[4].name = "[50] Banish Tornadoes";
  ass[5].name = "[60] Ass Gas";
  ass[6].name = "[70] Instant Fat Man";
  ass[7].name = "[80] Meteorites";
  ass[8].name = "[90) 300,000 UbUp Cash";
  ass[9].name = "[100] Unleash DISORDER";
}

// Update SacPoints every 0.1 seconds
public Action SacrificePointsUpdater(Handle timer) {
  if (WaveSystem().IsActive()) {
  CreateTimer(0.1, SacrificePointsUpdater);
  if (ass_sacPoints > ass_sacPointsMax) ass_sacPoints = ass_sacPointsMax; }
  return Plugin_Stop;
}

// SacPoints (Add points to Sacrifice Points occasionally)
public Action SacrificePointsTimer(Handle timer) {
  if (WaveSystem().IsActive() && (ass_sacPoints < ass_sacPointsMax)) {
    ass_sacPoints++;
    CreateTimer(GetRandomFloat(5.0, 30.0), SacrificePointsTimer);
  }
  return Plugin_Stop;
}

// Hikari's A.S.S - Command
public Action Command_SacrificePointShop(int client, int args) { ShowASS(client); return Plugin_Handled; }

// Hikari's A.S.S
public void ShowASS(int client) {
  CPrintToChat(client, (!WaveSystem().IsActive() ? "{darkviolet}[{forestgreen}CORE{darkviolet}] {forestgreen}The sacrificial points counter is currently at %i of %i maximum for this wave." : ass_sacPoints <= 9 ? "{darkviolet}[{forestgreen}CORE{darkviolet}] {red}You do not have enough sacrifice points to use this shop. You have %i / 10 required." : ""), ass_sacPoints, ass_sacPointsMax);
  if (!WaveSystem().IsActive() || ass_sacPoints < 10) return;
  Menu menu = new Menu(MenuHandlerASS, MENU_ACTIONS_DEFAULT);
  menu.SetTitle("Hikari's Annihilation Supply Shop"); menu.ExitButton = true;
  for (int i = 0; i < RoundToFloor(ass_sacPoints / 10.0); i++) menu.AddItem("", ass[i].name);
  menu.Display(client, 20);
}
public int MenuHandlerASS(Menu menu, MenuAction action, int client, int item) {
  if (action == MenuAction_Select) {
  if (ass_sacPoints < ass[item].price) return 0;
    if (WeatherManager.hasTornadoWarning() && item == 5) { CPrintToChatAll("{darkred}[Hikari's Operator] {limegreen}-> {red}%N {darkred}I'm sorry, it is too late for that.. You are doomed...", client); }
    else sudo(ass[item].purchase);
    LightLogger(LOGLVL_INFO, "%N opted for %s via the A.S.S.", client, ass[item].name);
    SCR_SendEvent("SacPoints Item Purchased", "%N opted for %s via the A.S.S.", client, ass[item].name);
  }
  else if (action == MenuAction_End) CloseHandle(menu);
  return 0;
}