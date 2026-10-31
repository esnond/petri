/* site.js v4: звуки, кнопка звука, оживление портретов. без зверинца:
   бегущего svg волка и загрузчик снял владелец. */
(function () {
  'use strict';
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
  ['pointerdown', 'keydown'].forEach(function (ev) {
    document.addEventListener(ev, function once() { ac(); sfx.pop(); document.removeEventListener(ev, once); });
  });

  var mb = document.createElement('button');
  mb.className = 'mutebtn'; mb.type = 'button';
  mb.setAttribute('aria-label', 'звук сайта');
  mb.setAttribute('aria-pressed', 'false');
  mb.textContent = '\uD83D\uDD0A';
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
})();
