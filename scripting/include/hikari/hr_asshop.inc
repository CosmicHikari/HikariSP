// Annihilation supply shop entries
enum struct ASShop {
  char name[64];
  int price;
  int purchase;
}
ASShop ass[10];

int ass_sacPoints;
int ass_sacPointsMax;

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