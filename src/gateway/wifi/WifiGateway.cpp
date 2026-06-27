// ============================================================
// gateway/wifi/WifiGateway.cpp
// ============================================================

#include "WifiGateway.h"

// ダッシュボード HTML（PROGMEM に格納してRAMを節約）
static const char INDEX_HTML[] PROGMEM = R"(<!DOCTYPE html>
<html lang="ja">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Tesla CAN Dashboard</title>
<style>
  body{margin:0;font-family:sans-serif;background:#0f1117;color:#e6edf3}
  .wrap{max-width:480px;margin:0 auto;padding:16px}
  h1{font-size:18px;font-weight:500;margin:0 0 16px}
  .badge{font-size:12px;padding:4px 10px;border-radius:99px;background:#1a3a6b;color:#58a6ff}
  .cards{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin-bottom:12px}
  .card{background:#1a1d27;border-radius:10px;padding:14px 16px}
  .label{font-size:12px;color:#7d8590;margin-bottom:4px}
  .val{font-size:30px;font-weight:500}
  .unit{font-size:13px;color:#7d8590}
  .mode{background:#1a1d27;border-radius:10px;padding:16px;margin-bottom:12px}
  .pills{display:flex;gap:8px;margin-top:10px}
  .pill{flex:1;text-align:center;padding:10px;border-radius:8px;font-size:14px;border:1px solid #2a2d3a;color:#7d8590}
  .chill.active{background:#1a3a6b;color:#58a6ff;border-color:#185fa5}
  .std.active{background:#1a3a1a;color:#39d353;border-color:#3b6d11}
  .sport.active{background:#3a1a1a;color:#ff4d6d;border-color:#a32d2d}
  .edfc{background:#1a1d27;border-radius:10px;padding:16px}
  .row{display:flex;gap:8px}
  .ei{flex:1;background:#0f1117;border-radius:8px;padding:10px;text-align:center}
  .ei.on{background:#1a3a1a}
  .ei-name{font-size:11px;color:#7d8590}
  .ei-val{font-size:15px;font-weight:500}
</style>
</head>
<body>
<div class="wrap">
  <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:16px">
    <h1>Tesla CAN Dashboard</h1>
    <span class="badge" id="conn">接続中</span>
  </div>
  <div class="cards">
    <div class="card"><div class="label">車速</div><div class="val" id="speed">--</div><div class="unit">km/h</div></div>
    <div class="card"><div class="label">CAN受信</div><div class="val" id="rate">--</div><div class="unit">msg/s</div></div>
  </div>
  <div class="mode">
    <div class="label">加速モード</div>
    <div class="pills">
      <div class="pill chill" id="chill">Chill</div>
      <div class="pill std"   id="std">Standard</div>
      <div class="pill sport" id="sport">Sport</div>
    </div>
  </div>
  <div class="edfc">
    <div class="label" style="margin-bottom:10px">EDFC5 ステータス</div>
    <div class="row">
      <div class="ei" id="ei-speed"><div class="ei-name">速度パルス</div><div class="ei-val" id="ei-spd">--</div></div>
      <div class="ei" id="ei1"><div class="ei-name">外部入力1</div><div class="ei-val" id="ei1v">OFF</div></div>
      <div class="ei" id="ei2"><div class="ei-name">外部入力2</div><div class="ei-val" id="ei2v">OFF</div></div>
    </div>
  </div>
</div>
<script>
const ws = new WebSocket('ws://' + location.hostname + '/ws');
ws.onmessage = e => {
  const d = JSON.parse(e.data);
  document.getElementById('speed').textContent = d.speed.toFixed(1);
  document.getElementById('rate').textContent  = d.rate;
  document.getElementById('ei-spd').textContent = d.speed.toFixed(1) + ' km/h';
  ['chill','std','sport'].forEach(m => document.getElementById(m).classList.remove('active'));
  const map = {Chill:'chill', Standard:'std', Sport:'sport'};
  if (map[d.mode]) document.getElementById(map[d.mode]).classList.add('active');
  const e1on = d.mode === 'Sport';
  const e2on = d.mode === 'Chill';
  document.getElementById('ei1').className = 'ei' + (e1on ? ' on' : '');
  document.getElementById('ei2').className = 'ei' + (e2on ? ' on' : '');
  document.getElementById('ei1v').textContent = e1on ? 'ON' : 'OFF';
  document.getElementById('ei2v').textContent = e2on ? 'ON' : 'OFF';
};
ws.onclose = () => { document.getElementById('conn').textContent = '切断'; };
</script>
</body>
</html>
)";

WifiGateway::WifiGateway() : _server(80), _ws("/ws") {}

void WifiGateway::begin() {
    _ws.onEvent([](AsyncWebSocket*, AsyncWebSocketClient*, AwsEventType,
                   void*, uint8_t*, size_t){});
    _server.addHandler(&_ws);
    setupRoutes();
    Serial.println("[WiFi] 初期化完了（AP未起動・オンデマンド待機）");
}

void WifiGateway::startAP() {
    if (_active) return;
    WiFi.softAP(SSID, PASSWORD, 6, HIDDEN ? 1 : 0);
    _server.begin();
    _active  = true;
    _startMs = millis();
    Serial.printf("[WiFi] AP起動: %s  http://192.168.4.1\n", SSID);
}

void WifiGateway::stopAP() {
    if (!_active) return;
    _server.end();
    WiFi.softAPdisconnect(true);
    _active = false;
    Serial.println("[WiFi] AP停止");
}

void WifiGateway::update() {
    if (!_active) return;
    // ON_MINUTES 経過でオートシャットダウン
    if (millis() - _startMs > (uint32_t)ON_MINUTES * 60 * 1000) {
        Serial.println("[WiFi] タイムアウト → 自動停止");
        stopAP();
    }
    _ws.cleanupClients();
}

void WifiGateway::broadcastSignals(const TeslaSignals& signals) {
    if (!_active || _ws.count() == 0) return;
    _ws.textAll(buildJson(signals));
}

void WifiGateway::setupRoutes() {
    _server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send_P(200, "text/html", INDEX_HTML);
    });
}

String WifiGateway::buildJson(const TeslaSignals& s) {
    char buf[128];
    snprintf(buf, sizeof(buf),
        R"({"speed":%.1f,"mode":"%s","rate":%u})",
        s.vehicleSpeedKmh, accelModeStr(s.accelMode), s.canRxRate
    );
    return String(buf);
}
