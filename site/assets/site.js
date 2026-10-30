/* site.js v3: каштан-беглец из настоящих мелковых кадров, звуки, пасхалки.
   ноль внешних файлов: звук синтезирует webaudio, кадры — те же jpg, что
   и в портретах. работает с file://, уважает prefers-reduced-motion. */
(function () {
  'use strict';
  function store(k, v) { try { if (v === undefined) return localStorage.getItem(k); localStorage.setItem(k, v); } catch (e) { return null; } }
  var reduce = window.matchMedia && matchMedia('(prefers-reduced-motion: reduce)').matches;

  /* ---------- звук: файлы не нужны ---------- */
  var AC = null, muted = false;
  function ac() {
    if (!AC) { try { AC = new (window.AudioContext || window.webkitAudioContext)(); } catch (e) {} }
    if (AC && AC.state === 'suspended') { try { AC.resume(); } catch (e) {} }
    return AC;
  }
  function tone(f0, f1, dur, type, vol) {
    if (muted) return;
    var a = ac(); if (!a) return;
    var o = a.createOscillator(), g = a.createGain();
    o.type = type || 'triangle';
    o.frequency.setValueAtTime(f0, a.currentTime);
    if (f1) o.frequency.exponentialRampToValueAtTime(f1, a.currentTime + dur / 1000);
    g.gain.setValueAtTime(vol * 1.6, a.currentTime);
    g.gain.exponentialRampToValueAtTime(0.0001, a.currentTime + dur / 1000);
    o.connect(g); g.connect(a.destination);
    o.start(); o.stop(a.currentTime + dur / 1000 + 0.02);
  }
  var sfx = {
    tick: function () { tone(1400, 1800, 0.05, 'triangle', 0.045); },
    plop: function () { tone(330, 150, 0.1, 'sine', 0.09); },
    bark: function () { tone(250, 190, 0.07, 'square', 0.07); setTimeout(function(){ tone(210, 150, 0.09, 'square', 0.07); }, 95); },
    pop:  function () { tone(620, 940, 0.06, 'triangle', 0.06); }
  };
  /* контекст просыпается от первого жеста: без этого браузеры молчат */
  ['pointerdown', 'keydown'].forEach(function (ev) {
    document.addEventListener(ev, function once() { ac(); sfx.pop(); document.removeEventListener(ev, once); });
  });

  var mb = document.createElement('button');
  mb.className = 'mutebtn'; mb.type = 'button';
  mb.setAttribute('aria-label', 'звук сайта');
  mb.setAttribute('aria-pressed', muted ? 'true' : 'false');
  mb.textContent = muted ? '\uD83D\uDD07' : '\uD83D\uDD0A';
  mb.addEventListener('click', function () {
    muted = !muted;
    mb.textContent = muted ? '\uD83D\uDD07' : '\uD83D\uDD0A';
    mb.setAttribute('aria-pressed', muted ? 'true' : 'false');
    if (!muted) sfx.pop();
  });
  document.body.appendChild(mb);

  Array.prototype.forEach.call(document.querySelectorAll('.sk'), function (el) {
    el.addEventListener('mouseenter', function () { sfx.tick(); });
  });
  Array.prototype.forEach.call(document.querySelectorAll('.btn, .chip'), function (el) {
    el.addEventListener('click', function () { sfx.plop(); });
  });

  /* ---------- каштан: анимированный svg субагента, прозрачный фон ---------- */
  var base = (function () {
    var probe = document.querySelector('img[src*="/wolf/"]');
    if (probe) return probe.getAttribute('src').replace(/wolf\/[^/]*$/, 'wolf/');
    return 'assets/wolf/';
  })();
  var RUNSVG = base + 'wolf-run.svg?v=1';

  function runner(cls) {
    var d = document.createElement('div');
    d.className = 'wolfrun ' + (cls || '');
    d.setAttribute('aria-hidden', 'true');
    d.title = 'погладить каштана';
    var img = document.createElement('img');
    img.alt = ''; img.src = RUNSVG;
    d.appendChild(img);
    d.addEventListener('click', function () {
      sfx.bark();
      d.classList.remove('jump'); void d.offsetWidth; d.classList.add('jump');
    });
    document.body.appendChild(d);
    return d;
  }
  if (!reduce) runner();

  /* оживление портретов: наведение меняет настроение */
  var moods = ['alert','happy','star','cry','angel','sleep','smug','calm','excited','pouty','shocked','ko'];
  Array.prototype.forEach.call(document.querySelectorAll('.ava img, .mascot img'), function (img) {
    var orig = img.getAttribute('src'), busy = false;
    img.addEventListener('mouseenter', function () {
      if (busy) return; busy = true;
      img.classList.add('wiggle');
      if (orig.indexOf('/wolf/') !== -1) {
        var m = moods[Math.floor(Math.random() * moods.length)];
        img.setAttribute('src', orig.replace(/wolf\/[^/]*$/, 'wolf/' + m + '.jpg'));
      }
    });
    img.addEventListener('mouseleave', function () {
      img.classList.remove('wiggle');
      img.setAttribute('src', orig);
      busy = false;
    });
    img.addEventListener('click', function () { sfx.bark(); });
  });

  /* пасхалка: наберите "волк" — придёт стая настоящих каштанов */
  var typed = '';
  document.addEventListener('keydown', function (e) {
    if (!e.key || e.key.length !== 1) return;
    typed = (typed + e.key.toLowerCase()).slice(-4);
    if (typed === 'волк' || typed === 'wolf') {
      typed = ''; sfx.pop();
      ['pack1', 'pack2'].forEach(function (cls) {
        if (!reduce) {
          var d = runner(cls);
          setTimeout(function () { d.remove(); }, 9000);
        }
      });
    }
  });

  /* ---------- загрузка: каштан бежит к сайту ---------- */
  var ld = document.createElement('div');
  ld.id = 'loader';
  var limg = document.createElement('img'); limg.src = RUNSVG; limg.alt = '';
  ld.appendChild(limg);
  var lp = document.createElement('p'); lp.textContent = 'каштан бежит к сайту...';
  ld.appendChild(lp);
  document.body.insertBefore(ld, document.body.firstChild);
  window.addEventListener('load', function () {
    setTimeout(function () {
      ld.classList.add('done');
      setTimeout(function () { ld.remove(); }, 650);
    }, 250);
  });
})();
