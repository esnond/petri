/* site.js v5: настоящие звуки (wav), появление секций при прокрутке,
   оживление портретов. без зверинца. */
(function () {
  'use strict';
  var muted = false;
  function play(name, vol) {
    if (muted) return;
    try {
      var a = new Audio('assets/snd/' + name + '.wav?v=1');
      a.volume = vol || 0.45;
      var p = a.play();
      if (p && p.catch) p.catch(function () {});
    } catch (e) {}
  }
  var sfx = {
    tick: function () { play('tick', 0.3); },
    plop: function () { play('plop', 0.5); },
    bark: function () { play('bark', 0.55); }
  };

  var mb = document.createElement('button');
  mb.className = 'mutebtn'; mb.type = 'button';
  mb.setAttribute('aria-label', 'звук сайта');
  mb.setAttribute('aria-pressed', 'false');
  mb.textContent = '\uD83D\uDD0A';
  mb.addEventListener('click', function () {
    muted = !muted;
    mb.textContent = muted ? '\uD83D\uDD07' : '\uD83D\uDD0A';
    mb.setAttribute('aria-pressed', muted ? 'true' : 'false');
    if (!muted) sfx.plop();
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

  /* появление: секции и карточки мягко всплывают при прокрутке */
  var reduce = window.matchMedia && matchMedia('(prefers-reduced-motion: reduce)').matches;
  var targets = document.querySelectorAll('.sk, section h2, .wrap > p');
  if (!reduce && 'IntersectionObserver' in window) {
    Array.prototype.forEach.call(targets, function (el) { el.classList.add('reveal'); });
    var io = new IntersectionObserver(function (es) {
      es.forEach(function (e) {
        if (e.isIntersecting) { e.target.classList.add('on'); io.unobserve(e.target); }
      });
    }, { threshold: 0.12 });
    Array.prototype.forEach.call(targets, function (el) { io.observe(el); });
  }
})();
