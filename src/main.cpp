#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
const char sayfa[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="tr">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 Panel</title>
<style>
  :root { --bg:#0d0f17; --cizgi:#262b3d; --yazi:#eceef6; --soluk:#8d95ad; --mor:#b79cff; --nane:#7ee6a1; --kirmizi:#ff6b6b; }
  * { box-sizing: border-box; margin: 0; }
  body {
    font-family: "Segoe UI", system-ui, Arial, sans-serif; color: var(--yazi); min-height: 100vh;
    background:
      radial-gradient(1100px 600px at 8% -10%, #2a2150 0%, transparent 60%),
      radial-gradient(900px 500px at 100% 0%, #14304a 0%, transparent 55%),
      var(--bg);
  }
  .uygulama { max-width: 1180px; margin: 0 auto; padding: 18px; }

  /* ust bar */
  .ust { display: flex; align-items: center; justify-content: space-between; gap: 12px; flex-wrap: wrap; margin-bottom: 22px; }
  .logo { display: flex; align-items: center; gap: 10px; font-weight: 700; font-size: 18px; }
  .logo i { width: 34px; height: 34px; border-radius: 10px; display: inline-block; background: linear-gradient(135deg, var(--mor), var(--nane)); }
  .menu { display: flex; gap: 4px; background: #171a26; border: 1px solid var(--cizgi); padding: 5px; border-radius: 999px; }
  .menu button { background: none; border: none; color: var(--soluk); padding: 8px 18px; border-radius: 999px; font-size: 14px; cursor: pointer; }
  .menu button.aktif { background: #fff; color: #12141c; font-weight: 600; }
  .rozetler { display: flex; gap: 8px; align-items: center; }
  .durumrozet { font-size: 12px; padding: 6px 12px; border-radius: 999px; background: #171a26; border: 1px solid var(--cizgi); color: var(--soluk); }
  .durumrozet b { display: inline-block; width: 8px; height: 8px; border-radius: 50%; background: var(--kirmizi); margin-right: 6px; }
  .durumrozet.canli b { background: var(--nane); }

  h1 { font-size: 34px; font-weight: 600; margin-bottom: 18px; letter-spacing: -.5px; }
  h2.baslik { font-size: 26px; font-weight: 600; margin-bottom: 16px; }

  /* yerlesim */
  .govde { display: grid; grid-template-columns: 1fr 340px; gap: 16px; }
  .sol { display: grid; grid-template-columns: repeat(3, 1fr); gap: 16px; align-content: start; }
  .span2 { grid-column: span 2; }
  .gorunum { display: none; }
  .gorunum.ac { display: block; }

  /* kartlar */
  .kart { background: linear-gradient(160deg, #1a1e2c, #151824); border: 1px solid var(--cizgi); border-radius: 24px; padding: 20px; position: relative; }
  .etiket { color: var(--soluk); font-size: 14px; margin-bottom: 8px; display: flex; justify-content: space-between; align-items: center; }
  .buyuk { font-size: 34px; font-weight: 600; letter-spacing: -.5px; margin-bottom: 8px; }
  .devasa { font-size: 44px; font-weight: 600; letter-spacing: -1px; margin: 6px 0 4px; }
  .alt { color: var(--soluk); font-size: 13px; }
  .rozet { display: inline-block; font-size: 12px; font-weight: 600; padding: 4px 10px; border-radius: 999px; }
  .yesil { background: #b6f28a; color: #1b2a0b; }
  .kirmizi-r { background: #ff7d7d; color: #3a0b0b; }
  .gri-r { background: #2d3248; color: #c3c9de; }
  .bos { color: var(--soluk); font-size: 14px; padding: 40px 0; text-align: center; }

  /* hero */
  .hero { grid-column: span 2; grid-row: span 2; overflow: hidden; min-height: 260px; display: flex; flex-direction: column; }
  .hero::before {
    content: ""; position: absolute; right: -70px; top: -70px; width: 320px; height: 320px; border-radius: 50%;
    background: var(--led, #2a2f45); filter: blur(55px); opacity: .85; transition: background-color .4s;
  }
  .hero > * { position: relative; z-index: 1; }
  .butonlar { margin-top: auto; padding-top: 18px; display: flex; gap: 10px; flex-wrap: wrap; }
  .cipler { display: flex; gap: 8px; flex-wrap: wrap; margin-top: 14px; }
  .cipler span { font-size: 12px; background: rgba(255,255,255,.07); border-radius: 999px; padding: 5px 11px; color: #c9cee0; }

  .btn { border: none; border-radius: 999px; padding: 11px 20px; font-size: 14px; cursor: pointer; font-weight: 600; margin: 3px 6px 3px 0; }
  .btn.koyu { background: #fff; color: #12141c; }
  .btn.acik { background: #2a2f45; color: #fff; }
  .btn.kirmizi { background: #e5484d; color: #fff; }
  .btn.mor { background: var(--mor); color: #1a1230; }
  .btn.kucuk { padding: 7px 12px; font-size: 13px; }

  /* donut */
  .donutkap { display: flex; align-items: center; gap: 14px; }
  .donutkap svg { width: 130px; height: 130px; flex: none; }
  .donutkap .dm { font-size: 22px; font-weight: 700; }
  #dArc { transition: stroke-dasharray .6s; }
  .lejant { font-size: 13px; color: var(--soluk); line-height: 1.8; }

  /* olay listesi */
  .olay { display: flex; align-items: center; gap: 12px; padding: 10px 0; border-bottom: 1px solid #232839; font-size: 14px; }
  .olay:last-child { border-bottom: none; }
  .olay .nokta { width: 34px; height: 34px; border-radius: 50%; background: #2a2f45; display: flex; align-items: center; justify-content: center; flex: none; }
  .olay .nokta svg { width: 16px; height: 16px; stroke: #fff; fill: none; stroke-width: 2; stroke-linecap: round; stroke-linejoin: round; }
  .olay .zaman { margin-left: auto; color: var(--soluk); font-size: 13px; }

  /* sag kolon */
  .proje { display: flex; align-items: center; gap: 12px; padding: 12px; border-radius: 16px; background: #1e2232; margin-bottom: 10px; cursor: pointer; border: 1px solid transparent; }
  .proje:hover { border-color: #3b4262; }
  .ikon { width: 40px; height: 40px; border-radius: 12px; display: flex; align-items: center; justify-content: center; flex: none; }
  .ikon svg { width: 20px; height: 20px; stroke: #fff; fill: none; stroke-width: 2; stroke-linecap: round; stroke-linejoin: round; }
  .proje .ad { font-weight: 600; font-size: 15px; }
  .proje .ks { font-size: 12px; color: var(--soluk); }
  .proje .dg { margin-left: auto; font-weight: 700; font-size: 15px; }
  .ekle { border: 1.5px dashed #3b4262; background: none; color: var(--soluk); justify-content: center; font-size: 14px; }
  .ekle:hover { border-color: var(--mor); color: var(--mor); }

  /* segment menu */
  .seg { display: inline-flex; gap: 4px; background: #12141d; border: 1px solid var(--cizgi); padding: 4px; border-radius: 999px; margin-bottom: 16px; flex-wrap: wrap; }
  .seg button { background: none; border: none; color: var(--soluk); padding: 9px 18px; border-radius: 999px; cursor: pointer; font-size: 14px; }
  .seg button.aktif { background: var(--mor); color: #1a1230; font-weight: 600; }

  /* LED sayfasi */
  .satir { display: inline-flex; align-items: center; margin: 4px 10px 4px 0; background: #1e2232; border-radius: 14px; padding: 6px; }
  input[type=color] { width: 54px; height: 44px; border: none; background: none; padding: 0; cursor: pointer; }
  input[type=number] { width: 100px; padding: 10px; border-radius: 12px; border: 1px solid #343a4d; background: #12141d; color: var(--yazi); font-size: 15px; }
  input[type=range] { width: 100%; accent-color: var(--mor); }
  label { display: block; margin: 14px 0 6px; font-size: 14px; color: var(--soluk); }
  .renksec { width: 100%; height: 190px; border-radius: 18px; }
  .istatlar { display: grid; grid-template-columns: repeat(4, 1fr); gap: 10px; margin-top: 14px; }
  .istat { background: #1e2232; border-radius: 14px; padding: 12px; }
  .istat small { color: var(--soluk); display: block; font-size: 12px; }
  .istat b { font-size: 20px; }

  #toast { position: fixed; left: 50%; bottom: 24px; transform: translateX(-50%); background: #fff; color: #12141c; padding: 10px 18px; border-radius: 999px; font-size: 14px; font-weight: 600; opacity: 0; pointer-events: none; transition: opacity .25s; z-index: 9; }
  #toast.goster { opacity: 1; }

  @media (max-width: 980px) {
    .govde { grid-template-columns: 1fr; }
  }
  @media (max-width: 640px) {
    .sol { grid-template-columns: 1fr; }
    .span2, .hero { grid-column: span 1; grid-row: span 1; }
    .istatlar { grid-template-columns: repeat(2, 1fr); }
    h1 { font-size: 28px; }
  }
</style>
</head>
<body>
<div class="uygulama">

  <div class="ust">
    <div class="logo"><i></i>ESP32 Panel</div>
    <nav class="menu">
      <button id="m0" class="aktif" onclick="git('ana')">Panel</button>
      <button id="m1" onclick="git('led')">LED</button>
      <button id="m2" onclick="git('sensor')">Sensörler</button>
    </nav>
    <div class="rozetler">
      <span class="durumrozet" id="simRozet" style="display:none"></span>
      <span class="durumrozet" id="bagRozet"><b></b><span id="bagYazi">Bağlanıyor</span></span>
    </div>
  </div>

  <!-- ===================== ANA PANEL ===================== -->
  <section id="g-ana" class="gorunum ac">
    <h1>Kontrol Paneli</h1>
    <div class="govde">
      <div class="sol">

        <div class="kart hero" id="hero">
          <div class="etiket">LED durumu</div>
          <div class="devasa" id="heroMod">--</div>
          <div class="alt" id="heroRenk">--</div>
          <div class="butonlar">
            <button class="btn koyu" onclick="git('led')">LED'i yönet</button>
            <button class="btn acik" onclick="kapat()">Kapat</button>
          </div>
          <div class="cipler"><span id="cIp">IP --</span><span id="cWifi">Wi-Fi --</span><span id="cSure">Çalışma --</span></div>
        </div>

        <div class="kart">
          <div class="etiket">Sıcaklık <span class="rozet gri-r" id="tRozet">--</span></div>
          <div class="buyuk" id="tDeger">--</div>
          <div class="alt">Son ölçüm</div>
        </div>

        <div class="kart">
          <div class="etiket">Nem <span class="rozet gri-r" id="hRozet">--</span></div>
          <div class="buyuk" id="hDeger">--</div>
          <div class="alt">Son ölçüm</div>
        </div>

        <div class="kart span2">
          <div class="etiket">Sıcaklık geçmişi <span class="rozet gri-r">10 sn aralıkla</span></div>
          <div id="anaGrafik"></div>
        </div>

        <div class="kart">
          <div class="etiket">Nem oranı</div>
          <div class="donutkap">
            <svg viewBox="0 0 140 140">
              <defs>
                <linearGradient id="gr" x1="0" y1="0" x2="1" y2="1">
                  <stop offset="0" stop-color="#b79cff"/><stop offset="1" stop-color="#7ee6a1"/>
                </linearGradient>
              </defs>
              <circle cx="70" cy="70" r="52" fill="none" stroke="#262b3d" stroke-width="16"/>
              <circle id="dArc" cx="70" cy="70" r="52" fill="none" stroke="url(#gr)" stroke-width="16" stroke-linecap="round" transform="rotate(-90 70 70)" stroke-dasharray="0 400"/>
              <text id="dYazi" x="70" y="77" text-anchor="middle" fill="#eceef6" font-size="24" font-weight="700">--</text>
            </svg>
            <div class="lejant">
              <div id="konfor" style="color:#eceef6;font-weight:600">--</div>
              <div>0-30: Kuru</div><div>30-60: İdeal</div><div>60+: Nemli</div>
            </div>
          </div>
        </div>

        <div class="kart">
          <div class="etiket">Titreşim <span class="rozet gri-r" id="vRozet">--</span></div>
          <div class="buyuk" id="vDeger">--</div>
          <div class="alt" id="vAlt">--</div>
        </div>

        <div class="kart span2">
          <div class="etiket">Son titreşim olayları</div>
          <div id="anaOlay"></div>
        </div>

      </div>

      <aside>
        <div class="kart">
          <div class="etiket" style="font-size:17px;color:#eceef6;font-weight:600">Projelerim <span class="rozet gri-r">4</span></div>
          <div class="proje" onclick="git('led')">
            <div class="ikon" style="background:#7b5cff"><svg viewBox="0 0 24 24"><path d="M9 18h6M10 21h4M12 3a6 6 0 0 0-4 10.5c.7.7 1 1.5 1 2.5h6c0-1 .3-1.8 1-2.5A6 6 0 0 0 12 3z"/></svg></div>
            <div><div class="ad">LED Yakma</div><div class="ks">RGB LED kontrolü</div></div>
            <div class="dg" id="pLed">--</div>
          </div>
          <div class="proje" onclick="git('sensor','sicaklik')">
            <div class="ikon" style="background:#ff7a59"><svg viewBox="0 0 24 24"><path d="M14 14.76V3.5a2.5 2.5 0 0 0-5 0v11.26a4.5 4.5 0 1 0 5 0z"/></svg></div>
            <div><div class="ad">Sıcaklık</div><div class="ks">Canlı ölçüm</div></div>
            <div class="dg" id="pSic">--</div>
          </div>
          <div class="proje" onclick="git('sensor','nem')">
            <div class="ikon" style="background:#2fa4ff"><svg viewBox="0 0 24 24"><path d="M12 2.7l5.7 5.7a8 8 0 1 1-11.4 0z"/></svg></div>
            <div><div class="ad">Nem</div><div class="ks">Canlı ölçüm</div></div>
            <div class="dg" id="pNem">--</div>
          </div>
          <div class="proje" onclick="git('sensor','titresim')">
            <div class="ikon" style="background:#22b573"><svg viewBox="0 0 24 24"><path d="M22 12h-4l-3 9L9 3l-3 9H2"/></svg></div>
            <div><div class="ad">Titreşim</div><div class="ks">Olay sayacı</div></div>
            <div class="dg" id="pTit">--</div>
          </div>
          <div class="proje ekle" onclick="toast('Yeni proje ekleyince buraya gelecek')">+ Yeni proje ekle</div>
        </div>
      </aside>
    </div>
  </section>

  <!-- ===================== LED PROJESI ===================== -->
  <section id="g-led" class="gorunum">
    <h2 class="baslik">LED Yakma Projesi</h2>
    <div class="seg">
      <button id="lt0" class="aktif" onclick="ledSekme(0)">Sıralı renkler</button>
      <button id="lt1" onclick="ledSekme(1)">Renk seçici</button>
      <button id="lt2" onclick="ledSekme(2)">Efektler</button>
    </div>

    <div class="kart" id="lp0">
      <div class="etiket">3 veya daha fazla renk seç (en fazla 10)</div>
      <div id="liste"></div>
      <button class="btn acik kucuk" onclick="renkEkle()">+ Renk ekle</button>
      <label>Her renkte bekleme süresi (saniye)</label>
      <input type="number" id="gecikme" value="1" min="0.05" max="60" step="0.1">
      <div style="margin-top:18px">
        <button class="btn mor" onclick="diziBaslat()">Başlat</button>
        <button class="btn kirmizi" onclick="kapat()">Durdur</button>
      </div>
    </div>

    <div class="kart" id="lp1" style="display:none">
      <div class="etiket">Rengi seç, LED anında değişir</div>
      <input type="color" id="renkSec" class="renksec" value="#ff0000">
      <div style="margin-top:14px"><button class="btn kirmizi" onclick="kapat()">LED'i kapat</button></div>
    </div>

    <div class="kart" id="lp2" style="display:none">
      <div class="etiket">Hazır efektler</div>
      <label>Renk (nefes ve yanıp sönme için)</label>
      <input type="color" id="efRenk" value="#ff0000">
      <label>Bir tur süresi (ms): küçük sayı = hızlı</label>
      <input type="range" id="efHiz" min="300" max="6000" step="100" value="3000">
      <div style="margin-top:18px">
        <button class="btn mor" onclick="efekt('gokkusagi')">Gökkuşağı</button>
        <button class="btn mor" onclick="efekt('nefes')">Nefes alma</button>
        <button class="btn mor" onclick="efekt('yanipsonme')">Yanıp sönme</button>
        <button class="btn kirmizi" onclick="kapat()">Durdur</button>
      </div>
    </div>
  </section>

  <!-- ===================== SENSORLER ===================== -->
  <section id="g-sensor" class="gorunum">
    <h2 class="baslik">Sensörler</h2>
    <div class="seg">
      <button id="ss0" class="aktif" onclick="sensorSec('sicaklik')">Sıcaklık</button>
      <button id="ss1" onclick="sensorSec('nem')">Nem</button>
      <button id="ss2" onclick="sensorSec('titresim')">Titreşim</button>
    </div>
    <div class="kart">
      <div class="etiket"><span id="sBaslik">--</span><span class="rozet gri-r">son 5 dakika</span></div>
      <div id="sGrafik"></div>
      <div class="istatlar" id="sIstat"></div>
      <div id="sOlay"></div>
    </div>
  </section>

</div>
<div id="toast"></div>

<script>
  var DEMO = (location.protocol === 'file:');
  var V = null;
  var sensor = 'sicaklik';
  var demoBas = Date.now();
  var MODLAR = { kapali: 'Kapalı', manuel: 'Renk seçici', sirali: 'Sıralı renkler', efekt: 'Efekt çalışıyor' };

  function $(id) { return document.getElementById(id); }
  function f(x, n) { return (x === null || x === undefined) ? '--' : Number(x).toFixed(n); }
  function hex(r, g, b) {
    return '#' + [r, g, b].map(function (v) { return ('0' + Number(v).toString(16)).slice(-2); }).join('');
  }
  function hex2rgb(h) {
    return [parseInt(h.substr(1, 2), 16), parseInt(h.substr(3, 2), 16), parseInt(h.substr(5, 2), 16)];
  }
  function sure(s) {
    var h = Math.floor(s / 3600), m = Math.floor((s % 3600) / 60);
    return h > 0 ? h + ' sa ' + m + ' dk' : m + ' dk ' + (s % 60) + ' sn';
  }
  function once(s) {
    if (s < 60) return s + ' sn önce';
    if (s < 3600) return Math.floor(s / 60) + ' dk önce';
    return Math.floor(s / 3600) + ' sa önce';
  }
  function toast(m) {
    var t = $('toast');
    t.textContent = m;
    t.classList.add('goster');
    clearTimeout(toast.z);
    toast.z = setTimeout(function () { t.classList.remove('goster'); }, 1800);
  }

  /* ---------- sayfa gecisleri ---------- */
  function git(ad, alt) {
    ['ana', 'led', 'sensor'].forEach(function (a, i) {
      $('g-' + a).classList.toggle('ac', a === ad);
      $('m' + i).classList.toggle('aktif', a === ad);
    });
    if (ad === 'sensor' && alt) sensor = alt;
    ciz();
    window.scrollTo(0, 0);
  }
  function ledSekme(n) {
    for (var i = 0; i < 3; i++) {
      $('lp' + i).style.display = (i === n) ? 'block' : 'none';
      $('lt' + i).classList.toggle('aktif', i === n);
    }
  }
  function sensorSec(k) { sensor = k; ciz(); }

  /* ---------- ESP32 ile konusma ---------- */
  function api(yol, mesaj) {
    if (DEMO) { toast('Önizleme: ESP32 bağlı değil'); return; }
    fetch(yol).then(function () { if (mesaj) toast(mesaj); }).catch(function () { toast('ESP32 ile bağlantı kurulamadı'); });
  }
  function kapat() { api('/api/kapat', 'LED kapatıldı'); }

  function baglanti(ok) {
    $('bagRozet').classList.toggle('canli', ok);
    $('bagYazi').textContent = DEMO ? 'Önizleme' : (ok ? 'Bağlı' : 'Bağlantı yok');
  }
  function yenile() {
    if (DEMO) { V = demoVeri(); baglanti(true); ciz(); return; }
    fetch('/api/veri').then(function (r) { return r.json(); }).then(function (d) {
      V = d; baglanti(true); ciz();
    }).catch(function () { baglanti(false); });
  }

  /* ---------- onizleme icin sahte veri ---------- */
  function demoVeri() {
    var s = (Date.now() - demoBas) / 1000;
    var hT = [], hH = [];
    for (var i = 0; i < 30; i++) {
      var k = s - (29 - i) * 10 + 400;
      hT.push(+(24 + 2.5 * Math.sin(k / 40) + Math.sin(k * 1.3) * 0.2).toFixed(1));
      hH.push(Math.round(48 + 8 * Math.cos(k / 55)));
    }
    return { t: hT[29], h: hH[29], tit: 7, son: 42, up: Math.floor(s) + 3700, rssi: -58, ip: '192.168.1.45',
      r: 140, g: 90, b: 255, mod: 'sirali', sim: true, hT: hT, hH: hH, ol: [42, 130, 300, 820, 1900] };
  }

  /* ---------- grafik cizimi ---------- */
  function barlar(el, veri, birim, adet) {
    var d = veri.filter(function (x) { return x !== null; }).slice(-adet);
    if (d.length < 2) { el.innerHTML = '<div class="bos">Veri toplanıyor...</div>'; return; }
    var W = 480, H = 170, n = d.length, gap = n > 16 ? 3 : 8;
    var bw = (W - gap * (n - 1)) / n;
    var mn = Math.min.apply(null, d), mx = Math.max.apply(null, d);
    var aralik = Math.max(mx - mn, 1);
    var s = '<svg viewBox="0 0 ' + W + ' ' + H + '" width="100%">';
    d.forEach(function (v, i) {
      var h = 36 + (v - mn) / aralik * (H - 50);
      var x = i * (bw + gap), y = H - 4 - h, son = (i === n - 1);
      s += '<rect x="' + x.toFixed(1) + '" y="' + y.toFixed(1) + '" width="' + bw.toFixed(1) + '" height="' + h.toFixed(1) +
           '" rx="' + Math.min(12, bw / 2).toFixed(1) + '" fill="' + (son ? '#c9a8ff' : '#6b5e93') + '" opacity="' + (son ? 1 : 0.8) + '"/>';
      if (bw >= 20) {
        s += '<text x="' + (x + bw / 2).toFixed(1) + '" y="' + (H - 14) + '" text-anchor="middle" font-size="10" fill="' + (son ? '#1a1230' : '#fff') + '">' + Number(v).toFixed(birim === '%' ? 0 : 1) + '</text>';
      }
    });
    el.innerHTML = s + '</svg>';
  }
  function olayListe(el, ol) {
    if (!ol || ol.length === 0) { el.innerHTML = '<div class="bos">Henüz titreşim algılanmadı</div>'; return; }
    el.innerHTML = ol.map(function (s) {
      return '<div class="olay"><div class="nokta"><svg viewBox="0 0 24 24"><path d="M22 12h-4l-3 9L9 3l-3 9H2"/></svg></div>' +
             '<span>Titreşim algılandı</span><span class="rozet yesil" style="margin-left:8px">Algılandı</span>' +
             '<span class="zaman">' + once(s) + '</span></div>';
    }).join('');
  }
  function fark(a) {
    var d = a.filter(function (x) { return x !== null; });
    return d.length < 2 ? null : d[d.length - 1] - d[d.length - 2];
  }

  /* ---------- ekrani guncelle ---------- */
  function ciz() {
    if (!V) return;

    // simulasyon rozeti
    var sr = $('simRozet');
    sr.style.display = (DEMO || V.sim) ? 'inline-block' : 'none';
    sr.textContent = DEMO ? 'ÖNİZLEME VERİSİ' : 'SİMÜLASYON';

    // hero
    var kapali = (V.mod === 'kapali');
    $('hero').style.setProperty('--led', kapali ? '#2a2f45' : 'rgb(' + V.r + ',' + V.g + ',' + V.b + ')');
    $('heroMod').textContent = MODLAR[V.mod] || V.mod;
    $('heroRenk').textContent = kapali ? 'LED kapalı' : 'Şu anki renk: ' + hex(V.r, V.g, V.b).toUpperCase();
    $('cIp').textContent = 'IP ' + V.ip;
    $('cWifi').textContent = 'Wi-Fi ' + (V.rssi > -60 ? 'güçlü' : V.rssi > -75 ? 'iyi' : 'zayıf') + ' (' + V.rssi + ' dBm)';
    $('cSure').textContent = 'Çalışma ' + sure(V.up);

    // sicaklik / nem
    $('tDeger').textContent = f(V.t, 1) + '°C';
    $('hDeger').textContent = '%' + f(V.h, 0);
    var ft = fark(V.hT), fh = fark(V.hH);
    var tr = $('tRozet');
    tr.textContent = ft === null ? '--' : (ft > 0 ? '+' : '') + ft.toFixed(1) + '°';
    tr.className = 'rozet ' + (ft === null || ft === 0 ? 'gri-r' : ft > 0 ? 'kirmizi-r' : 'yesil');
    $('hRozet').textContent = fh === null ? '--' : (fh > 0 ? '+' : '') + fh.toFixed(0) + '%';

    // donut
    var pct = (V.h === null) ? 0 : Math.max(0, Math.min(100, V.h));
    $('dArc').setAttribute('stroke-dasharray', (pct * 3.267).toFixed(1) + ' 400');
    $('dYazi').textContent = V.h === null ? '--' : '%' + Math.round(pct);
    $('konfor').textContent = V.h === null ? '--' : (pct < 30 ? 'Kuru' : pct <= 60 ? 'İdeal' : 'Nemli');

    // titresim karti
    $('vDeger').textContent = V.tit;
    var vr = $('vRozet');
    if (V.tit > 0 && V.son >= 0 && V.son < 10) { vr.textContent = 'Titreşim var'; vr.className = 'rozet kirmizi-r'; }
    else { vr.textContent = 'Sakin'; vr.className = 'rozet yesil'; }
    $('vAlt').textContent = V.tit > 0 ? 'Son: ' + once(V.son) : 'Henüz titreşim yok';

    // sag liste
    $('pLed').textContent = MODLAR[V.mod] || '--';
    $('pSic').textContent = f(V.t, 1) + '°C';
    $('pNem').textContent = '%' + f(V.h, 0);
    $('pTit').textContent = V.tit;

    // grafikler
    barlar($('anaGrafik'), V.hT, '°C', 12);
    olayListe($('anaOlay'), V.ol.slice(0, 4));

    // sensor sayfasi
    sensorCiz();
  }

  function sensorCiz() {
    var adlar = ['sicaklik', 'nem', 'titresim'];
    adlar.forEach(function (k, i) { $('ss' + i).classList.toggle('aktif', k === sensor); });
    var g = $('sGrafik'), ist = $('sIstat'), ol = $('sOlay');

    if (sensor === 'titresim') {
      $('sBaslik').textContent = 'Titreşim sensörü';
      g.style.display = 'none';
      ist.innerHTML = '<div class="istat"><small>Toplam olay</small><b>' + V.tit + '</b></div>' +
                      '<div class="istat"><small>Son olay</small><b>' + (V.tit > 0 ? once(V.son) : '--') + '</b></div>';
      olayListe(ol, V.ol);
      return;
    }
    g.style.display = 'block';
    ol.innerHTML = '';
    var veri = (sensor === 'sicaklik') ? V.hT : V.hH;
    var br = (sensor === 'sicaklik') ? '°C' : '%';
    var n = (sensor === 'sicaklik') ? 1 : 0;
    $('sBaslik').textContent = (sensor === 'sicaklik') ? 'Sıcaklık geçmişi' : 'Nem geçmişi';
    barlar(g, veri, br, 30);
    var d = veri.filter(function (x) { return x !== null; });
    if (d.length === 0) { ist.innerHTML = ''; return; }
    var min = Math.min.apply(null, d), max = Math.max.apply(null, d);
    var ort = d.reduce(function (a, b) { return a + b; }, 0) / d.length;
    ist.innerHTML =
      '<div class="istat"><small>Şimdi</small><b>' + f(d[d.length - 1], n) + br + '</b></div>' +
      '<div class="istat"><small>En düşük</small><b>' + f(min, n) + br + '</b></div>' +
      '<div class="istat"><small>En yüksek</small><b>' + f(max, n) + br + '</b></div>' +
      '<div class="istat"><small>Ortalama</small><b>' + f(ort, n) + br + '</b></div>';
  }

  /* ---------- LED: sirali renkler ---------- */
  var renkler = ['#ff0000', '#00ff00', '#0000ff'];
  function listeCiz() {
    var k = $('liste');
    k.innerHTML = '';
    renkler.forEach(function (c, i) {
      var d = document.createElement('div');
      d.className = 'satir';
      var inp = document.createElement('input');
      inp.type = 'color';
      inp.value = c;
      inp.oninput = function () { renkler[i] = inp.value; };
      var sil = document.createElement('button');
      sil.textContent = 'x';
      sil.className = 'btn acik kucuk';
      sil.onclick = function () { if (renkler.length > 2) { renkler.splice(i, 1); listeCiz(); } };
      d.appendChild(inp);
      d.appendChild(sil);
      k.appendChild(d);
    });
  }
  function renkEkle() { if (renkler.length < 10) { renkler.push('#ffffff'); listeCiz(); } }
  function diziBaslat() {
    var sn = parseFloat($('gecikme').value) || 1;
    var ms = Math.max(50, Math.round(sn * 1000));
    var c = renkler.map(function (x) { return x.substr(1); }).join(',');
    api('/api/dizi?d=' + ms + '&c=' + c, 'Sıralı mod başladı');
  }
  listeCiz();

  /* ---------- LED: renk secici ---------- */
  var sonGonderim = 0;
  function renkGonder(h) {
    var c = hex2rgb(h);
    api('/api/renk?r=' + c[0] + '&g=' + c[1] + '&b=' + c[2], '');
  }
  var sec = $('renkSec');
  sec.addEventListener('input', function () {
    var n = Date.now();
    if (n - sonGonderim < 80) return;
    sonGonderim = n;
    renkGonder(sec.value);
  });
  sec.addEventListener('change', function () { renkGonder(sec.value); });

  /* ---------- LED: efektler ---------- */
  function efekt(ad) {
    var c = hex2rgb($('efRenk').value);
    api('/api/efekt?ad=' + ad + '&hiz=' + $('efHiz').value + '&r=' + c[0] + '&g=' + c[1] + '&b=' + c[2], 'Efekt başladı');
  }

  yenile();
  setInterval(yenile, 2000);
</script>
</body>
</html>
)rawliteral";

// ---------------- AYARLAR ----------------

const char* ssid  = "wifi_adresin";      // kendi Wi-Fi adini yaz
const char* sifre = "wifi_sifren";  // kendi Wi-Fi sifreni yaz


// 1: sensor yokken sahte veriyle dene, 0: gercek sensorleri oku
#define SIMULASYON 1

// LED pinleri
const int KIRMIZI = 21;
const int YESIL   = 22;
const int MAVI    = 23;

// Sensor pinleri (SIMULASYON 0 yapinca kullanilir)
#define DHT_PIN        4        // sicaklik + nem sensorunun veri pini
#define DHT_TIP        DHT11    // dersteki sensor DHT22 ise DHT22 yaz
#define TITRESIM_PIN   27       // titresim sensorunun dijital cikisi
#define TITRESIM_AKTIF HIGH     // sensor titresimde LOW veriyorsa LOW yaz

#if !SIMULASYON
#include <DHT.h>
DHT dht(DHT_PIN, DHT_TIP);
#endif

WebServer server(80);

// ---------------- LED DURUMU ----------------
enum Mod { KAPALI, MANUEL, SIRALI, EFEKT };
Mod mod = KAPALI;
const char* modAdi[] = { "kapali", "manuel", "sirali", "efekt" };

uint8_t curR = 0, curG = 0, curB = 0;  // LED'in su anki rengi (panelde gosterilir)

const int MAX_RENK = 10;
uint8_t dizi[MAX_RENK][3];
int diziSayisi = 0;
int diziIndex = 0;
unsigned long diziGecikme = 1000;
unsigned long sonAdim = 0;

String efektAdi = "gokkusagi";
unsigned long efektHiz = 3000;
uint8_t efektR = 255, efektG = 0, efektB = 0;

// ---------------- SENSOR VERILERI ----------------
float sicaklik = NAN;
float nem = NAN;

const int GECMIS = 30;            // 30 olcum x 10 sn = 5 dakika
float gecmisT[GECMIS];
float gecmisH[GECMIS];
int gecmisN = 0;

volatile uint32_t titresimSayisi = 0;
volatile unsigned long sonTitresim = 0;
volatile bool yeniTitresim = false;

const int OLAY_MAX = 10;
unsigned long olaylar[OLAY_MAX];
int olayN = 0;

unsigned long sonOkuma = 0;
unsigned long sonKayit = 0;
bool ilkOkuma = true;

// ---------------- YARDIMCI FONKSIYONLAR ----------------
void renkAyarla(int r, int g, int b) {
  curR = r; curG = g; curB = b;
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(KIRMIZI, r);
  ledcWrite(YESIL, g);
  ledcWrite(MAVI, b);
#else
  ledcWrite(0, r);
  ledcWrite(1, g);
  ledcWrite(2, b);
#endif
}

void hsvToRgb(float h, uint8_t &r, uint8_t &g, uint8_t &b) {
  float x = 1.0 - fabs(fmod(h / 60.0, 2.0) - 1.0);
  float rr = 0, gg = 0, bb = 0;
  if (h < 60)       { rr = 1; gg = x; }
  else if (h < 120) { rr = x; gg = 1; }
  else if (h < 180) { gg = 1; bb = x; }
  else if (h < 240) { gg = x; bb = 1; }
  else if (h < 300) { rr = x; bb = 1; }
  else              { rr = 1; bb = x; }
  r = rr * 255;
  g = gg * 255;
  b = bb * 255;
}

void gecmisEkle(float t, float h) {
  if (gecmisN == GECMIS) {
    for (int i = 1; i < GECMIS; i++) {
      gecmisT[i - 1] = gecmisT[i];
      gecmisH[i - 1] = gecmisH[i];
    }
    gecmisN--;
  }
  gecmisT[gecmisN] = t;
  gecmisH[gecmisN] = h;
  gecmisN++;
}

void olayEkle(unsigned long t) {
  if (olayN == OLAY_MAX) {
    for (int i = 1; i < OLAY_MAX; i++) olaylar[i - 1] = olaylar[i];
    olayN--;
  }
  olaylar[olayN++] = t;
}

#if !SIMULASYON
void IRAM_ATTR titresimISR() {
  unsigned long n = millis();
  if (n - sonTitresim > 300) {   // 300 ms icinde tekrar sayma
    sonTitresim = n;
    titresimSayisi++;
    yeniTitresim = true;
  }
}
#endif

String sayi(float x, int n) {
  if (isnan(x)) return String("null");
  return String(x, n);
}

// ---------------- ISTEK YONETICILERI ----------------
void anaSayfa() {
  server.send_P(200, "text/html", sayfa);
}

void apiVeri() {
  unsigned long simdi = millis();
  String s;
  s.reserve(1600);

  s += "{\"t\":";   s += sayi(sicaklik, 1);
  s += ",\"h\":";   s += sayi(nem, 0);
  s += ",\"tit\":"; s += String((unsigned long)titresimSayisi);
  s += ",\"son\":";
  if (titresimSayisi > 0) s += String((simdi - sonTitresim) / 1000);
  else s += "-1";
  s += ",\"up\":";  s += String(simdi / 1000);
  s += ",\"rssi\":"; s += String(WiFi.RSSI());
  s += ",\"ip\":\""; s += WiFi.localIP().toString(); s += "\"";
  s += ",\"r\":"; s += String(curR);
  s += ",\"g\":"; s += String(curG);
  s += ",\"b\":"; s += String(curB);
  s += ",\"mod\":\""; s += modAdi[mod]; s += "\"";
  s += ",\"sim\":"; s += (SIMULASYON ? "true" : "false");

  s += ",\"hT\":[";
  for (int i = 0; i < gecmisN; i++) { if (i) s += ","; s += String(gecmisT[i], 1); }
  s += "],\"hH\":[";
  for (int i = 0; i < gecmisN; i++) { if (i) s += ","; s += String(gecmisH[i], 0); }
  s += "],\"ol\":[";
  for (int i = olayN - 1; i >= 0; i--) {
    s += String((simdi - olaylar[i]) / 1000);
    if (i > 0) s += ",";
  }
  s += "]}";

  server.send(200, "application/json", s);
}

void apiRenk() {
  mod = MANUEL;
  renkAyarla(server.arg("r").toInt(), server.arg("g").toInt(), server.arg("b").toInt());
  server.send(200, "text/plain", "OK");
}

void apiDizi() {
  String c = server.arg("c");
  long d = server.arg("d").toInt();
  if (d < 50) d = 50;
  if (d > 60000) d = 60000;

  diziSayisi = 0;
  int bas = 0;
  while (bas < (int)c.length() && diziSayisi < MAX_RENK) {
    int virgul = c.indexOf(',', bas);
    if (virgul < 0) virgul = c.length();
    String h = c.substring(bas, virgul);
    long v = strtol(h.c_str(), NULL, 16);
    dizi[diziSayisi][0] = (v >> 16) & 255;
    dizi[diziSayisi][1] = (v >> 8) & 255;
    dizi[diziSayisi][2] = v & 255;
    diziSayisi++;
    bas = virgul + 1;
  }

  if (diziSayisi == 0) {
    server.send(400, "text/plain", "Renk yok");
    return;
  }

  diziGecikme = d;
  diziIndex = 0;
  sonAdim = millis() - diziGecikme;  // ilk renk hemen yansin
  mod = SIRALI;
  server.send(200, "text/plain", "OK");
}

void apiEfekt() {
  efektAdi = server.arg("ad");
  long hiz = server.arg("hiz").toInt();
  if (hiz < 100) hiz = 100;
  efektHiz = hiz;
  efektR = server.arg("r").toInt();
  efektG = server.arg("g").toInt();
  efektB = server.arg("b").toInt();
  mod = EFEKT;
  server.send(200, "text/plain", "OK");
}

void apiKapat() {
  mod = KAPALI;
  renkAyarla(0, 0, 0);
  server.send(200, "text/plain", "OK");
}

// ---------------- KURULUM ----------------
void setup() {
  Serial.begin(115200);

#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(KIRMIZI, 5000, 8);
  ledcAttach(YESIL, 5000, 8);
  ledcAttach(MAVI, 5000, 8);
#else
  ledcSetup(0, 5000, 8); ledcAttachPin(KIRMIZI, 0);
  ledcSetup(1, 5000, 8); ledcAttachPin(YESIL, 1);
  ledcSetup(2, 5000, 8); ledcAttachPin(MAVI, 2);
#endif
  renkAyarla(0, 0, 0);

#if !SIMULASYON
  dht.begin();
  pinMode(TITRESIM_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(TITRESIM_PIN), titresimISR,
                  TITRESIM_AKTIF == HIGH ? RISING : FALLING);
#endif

  WiFi.begin(ssid, sifre);
  Serial.print("Wi-Fi'ye baglaniyor");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Tarayiciya su adresi yaz: http://");
  Serial.println(WiFi.localIP());

  server.on("/", anaSayfa);
  server.on("/api/veri", apiVeri);
  server.on("/api/renk", apiRenk);
  server.on("/api/dizi", apiDizi);
  server.on("/api/efekt", apiEfekt);
  server.on("/api/kapat", apiKapat);
  server.begin();
}

// ---------------- DONGU ----------------
void loop() {
  server.handleClient();
  unsigned long simdi = millis();

  // ---- sensorleri oku (2 sn'de bir) ----
  if (ilkOkuma || simdi - sonOkuma >= 2000) {
    ilkOkuma = false;
    sonOkuma = simdi;
#if SIMULASYON
    sicaklik = 24.0 + 2.5 * sin(simdi / 40000.0) + random(-10, 11) / 50.0;
    nem = 48 + 8 * cos(simdi / 55000.0) + random(-2, 3);
#else
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (!isnan(t)) sicaklik = t;
    if (!isnan(h)) nem = h;
#endif
  }

  // ---- gecmise kaydet (10 sn'de bir) ----
  if (simdi - sonKayit >= 10000 && !isnan(sicaklik) && !isnan(nem)) {
    sonKayit = simdi;
    gecmisEkle(sicaklik, nem);
  }

  // ---- titresim ----
#if SIMULASYON
  static unsigned long simSon = 0;
  if (simdi - simSon >= 1000) {
    simSon = simdi;
    if (random(100) < 6) {
      sonTitresim = simdi;
      titresimSayisi++;
      olayEkle(simdi);
    }
  }
#else
  if (yeniTitresim) {
    yeniTitresim = false;
    olayEkle(sonTitresim);
  }
#endif

  // ---- LED modlari ----
  if (mod == SIRALI && diziSayisi > 0) {
    if (simdi - sonAdim >= diziGecikme) {
      sonAdim = simdi;
      renkAyarla(dizi[diziIndex][0], dizi[diziIndex][1], dizi[diziIndex][2]);
      diziIndex = (diziIndex + 1) % diziSayisi;
    }
  } else if (mod == EFEKT) {
    float faz = (simdi % efektHiz) / (float)efektHiz;  // 0..1
    if (efektAdi == "gokkusagi") {
      uint8_t r, g, b;
      hsvToRgb(faz * 360.0, r, g, b);
      renkAyarla(r, g, b);
    } else if (efektAdi == "nefes") {
      float parlaklik = (sin(faz * 2 * PI - PI / 2) + 1.0) / 2.0;
      renkAyarla(efektR * parlaklik, efektG * parlaklik, efektB * parlaklik);
    } else if (efektAdi == "yanipsonme") {
      if (faz < 0.5) renkAyarla(efektR, efektG, efektB);
      else renkAyarla(0, 0, 0);
    }
  }
}