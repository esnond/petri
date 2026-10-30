/* site.js: волк-беглец, синтезированные звуки, пасхалки.
   ноль внешних файлов: звук делает webaudio, волк — inline svg.
   уважает prefers-reduced-motion: в тихом режиме зверинца нет. */
(function () {
  'use strict';
  var reduce = window.matchMedia && matchMedia('(prefers-reduced-motion: reduce)').matches;

  /* ---------- звук: файлы не нужны ---------- */
  var AC = null, muted = localStorage.getItem('esnond-mute') === '1';
  function ac() {
    if (!AC) { try { AC = new (window.AudioContext || window.webkitAudioContext)(); } catch (e) {} }
    return AC;
  }
  function tone(f0, f1, dur, type, vol) {
    if (muted) return;
    var a = ac(); if (!a) return;
    var o = a.createOscillator(), g = a.createGain();
    o.type = type || 'triangle';
    o.frequency.setValueAtTime(f0, a.currentTime);
    if (f1) o.frequency.exponentialRampToValueAtTime(f1, a.currentTime + dur / 1000);
    g.gain.setValueAtTime(vol || 0.04, a.currentTime);
    g.gain.exponentialRampToValueAtTime(0.0001, a.currentTime + dur / 1000);
    o.connect(g); g.connect(a.destination);
    o.start(); o.stop(a.currentTime + dur / 1000 + 0.02);
  }
  var sfx = {
    tick: function () { tone(1500, 1900, 0.045, 'triangle', 0.02); },
    plop: function () { tone(330, 150, 0.09, 'sine', 0.05); },
    bark: function () { tone(250, 190, 0.07, 'square', 0.045); setTimeout(function(){ tone(210, 150, 0.09, 'square', 0.045); }, 95); },
    pop:  function () { tone(620, 940, 0.06, 'triangle', 0.04); }
  };
  var mb = document.createElement('button');
  mb.className = 'mutebtn'; mb.type = 'button';
  mb.setAttribute('aria-label', 'звук сайта');
  mb.setAttribute('aria-pressed', muted ? 'true' : 'false');
  mb.textContent = muted ? '\uD83D\uDD07' : '\uD83D\uDD0A';
  mb.addEventListener('click', function () {
    muted = !muted;
    localStorage.setItem('esnond-mute', muted ? '1' : '0');
    mb.textContent = muted ? '\uD83D\uDD07' : '\uD83D\uDD0A';
    mb.setAttribute('aria-pressed', muted ? 'true' : 'false');
    if (!muted) sfx.pop();
  });
  document.body.appendChild(mb);

  /* тактильность: тихий тик на карточках, плоп на кнопках */
  Array.prototype.forEach.call(document.querySelectorAll('.sk'), function (el) {
    el.addEventListener('mouseenter', function () { sfx.tick(); });
  });
  Array.prototype.forEach.call(document.querySelectorAll('.btn, .chip'), function (el) {
    el.addEventListener('click', function () { sfx.plop(); });
  });

  /* оживление портретов: наведение меняет настроение, уход возвращает */
  var moods = ['alert','happy','star','cry','angel','sleep','smug','calm','excited','pouty','shocked','ko'];
  Array.prototype.forEach.call(document.querySelectorAll('.ava img, .mascot img'), function (img) {
    var orig = img.getAttribute('src'), busy = false;
    img.addEventListener('mouseenter', function () {
      if (busy) return; busy = true;
      img.classList.add('wiggle');
      if (orig.indexOf('/wolf/') !== -1) {
        var m = moods[Math.floor(Math.random() * moods.length)];
        img.setAttribute('src', 'assets/wolf/' + m + '.jpg?v=3');
      }
    });
    img.addEventListener('mouseleave', function () {
      img.classList.remove('wiggle');
      img.setAttribute('src', orig);
      busy = false;
    });
    img.addEventListener('click', function () { sfx.bark(); });
  });

  /* ---------- волк-беглец, как жук в claude code ---------- */
  var WOLF = '<svg viewBox="0 0 120 84" xmlns="http://www.w3.org/2000/svg" aria-hidden="true">'
    + '<g fill="none" stroke="currentColor" stroke-width="5" stroke-linecap="round" stroke-linejoin="round">'
    + '<path class="wbody" d="M14 44 h58 q10 -2 12 -14 q2 -10 12 -8 l14 4 q6 2 4 8 l-6 10 q-4 6 -14 6 h-6 v10 h-8 l-2 -10 h-26 l-2 10 h-8 v-10 q-16 -2 -24 -6 z"/>'
    + '<path d="M14 44 l-4 10 m8 -10 l-2 10" stroke-width="3.4"/>'
    + '<path d="M16 52 q4 4 10 2 M16 58 q4 3 9 1" stroke-width="2.6"/>'
    + '<g class="wl l1"><line x1="36" y1="56" x2="34" y2="78"/></g>'
    + '<g class="wl l2"><line x1="48" y1="56" x2="50" y2="78"/></g>'
    + '<g class="wl l3"><line x1="68" y1="56" x2="66" y2="78"/></g>'
    + '<g class="wl l4"><line x1="80" y1="56" x2="84" y2="78"/></g>'
    + '<path class="wtail" d="M100 30 q10 -12 6 -22" stroke-width="5.4"/>'
    + '<circle cx="102" cy="36" r="2.6" fill="currentColor" stroke="none"/></g></svg>';

  var run = document.createElement('div');
  run.className = 'wolfrun'; run.setAttribute('aria-hidden', 'true');
  run.title = 'погладить волка';
  run.innerHTML = WOLF;
  document.body.appendChild(run);
  run.addEventListener('click', function () {
    sfx.bark();
    run.classList.remove('jump');
    void run.offsetWidth;
    run.classList.add('jump');
  });

  /* пасхалка: наберите "волк" — придёт стая */
  var typed = '';
  document.addEventListener('keydown', function (e) {
    if (e.key && e.key.length === 1) {
      typed = (typed + e.key.toLowerCase()).slice(-4);
      if (typed === 'волк' || typed === 'wolf') {
        typed = ''; sfx.pop();
        for (var i = 0; i < 2; i++) {
          (function (i) {
            var d = document.createElement('div');
            d.className = 'wolfrun pack' + i;
            d.setAttribute('aria-hidden', 'true');
            d.innerHTML = WOLF;
            document.body.appendChild(d);
            setTimeout(function () { d.remove(); }, 9000);
          })(i);
        }
      }
    }
  });

  /* ---------- загрузка: волк подбегает к сайту ---------- */
  var ld = document.createElement('div');
  ld.id = 'loader';
  ld.innerHTML = '<div class="wl-mascot">' + WOLF + '</div><p>волк бежит за сайтом...</p>';
  document.body.insertBefore(ld, document.body.firstChild);
  window.addEventListener('load', function () {
    setTimeout(function () {
      ld.classList.add('done');
      setTimeout(function () { ld.remove(); }, 650);
    }, 250);
  });
})();
