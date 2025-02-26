//
// 2021/9/19 Waync Cheng.
//

var PARTICLE_W = 8, PARTICLE_H = 8;
var FPS = 60;

function stge_module(canvas) {
  this.canvas = canvas;
  this.ctx = canvas.getContext("2d");
  this.last_x = 0;
  this.last_y = 0;
  this.timer = null;

  var ret = stge_init(canvas.width, canvas.height);
  if (!ret) {
    alert('init stge failed.');
    return;
  }

  function getEventOffset(e) {
    if (e.offsetX) {
      return {x:e.offsetX, y:e.offsetY};
    }
    var el = e.target;
    var offset = {x:0, y:0};
    while (el.offsetParent) {
      offset.x += el.offsetLeft;
      offset.y += el.offsetTop;
      el = el.offsetParent;
    }
    offset.x = e.pageX - offset.x;
    offset.y = e.pageY - offset.y;
    return offset;
  }

  canvas.onmousemove = function(e) {
    var off = getEventOffset(e);
    last_x = off.x, last_y = off.y;
  }

  this.parse = function(code) {
    var ret = stge_parse(code);
    if (!ret) {
      var err = stge_get_last_parse_error();
      alert(err);
      return false;
    }
    return true;
  }

  this.getScripts = function() {
    var scripts = [];
    var n = stge_get_script_count();
    for (var i = 0; i < n; i++) {
      var s = stge_get_script(i);
      scripts.push(s);
    }
    return scripts;
  }

  this.run = function(script) {
    var ret = stge_run(script);
    if (-1 == ret) {
      alert('run \'' + script + '\' failed!');
      return;
    }
    if (this.timer) {
      clearInterval(this.timer);
    }
    this.timer = setInterval(update, 1000 / FPS, this);

    function update(stge) {
      var ret = stge_update(1000 / FPS, this.last_x, this.last_y);
      stge.ctx.fillStyle = 'black';
      stge.ctx.fillRect(0, 0, stge.canvas.width, stge.canvas.height);
      stge.ctx.fillStyle = 'red';
      var n = stge_get_obj_count();
      for (var i = 0; i < n; i++) {
        var x = stge_get_obj_xpos(i);
        var y = stge_get_obj_ypos(i);
        stge.ctx.fillRect(x - PARTICLE_W/2 + stge.canvas.width/2, y - PARTICLE_H/2 + stge.canvas.height/2, PARTICLE_W, PARTICLE_H);
      }
      stge.ctx.font = "16px Georgia";
      stge.ctx.fillStyle = 'white';
      var a = stge_get_action_count();
      stge.ctx.fillText('a' + a + ' o' + n, 0, 16);
      if (!ret) {
        clearInterval(stge.timer);
        stge.timer = null;
      }
    }
  }

  this.uninit = function() {
    if (this.timer) {
      clearInterval(this.timer);
      this.timer = null;
    }
  }
}

// STGE wrapper.
function stge_get_action_count() {
  return Module.ccall('stge_get_action_count', 'number', [], []);
}

function stge_get_last_parse_error() {
  return Module.ccall('stge_get_last_parse_error', 'string', [], []);
}

function stge_get_obj_count() {
  return Module.ccall('stge_get_obj_count', 'number', [], []);
}

function stge_get_obj_xpos(i) {
  return Module.ccall('stge_get_obj_xpos', 'number', ['number'], [i]);
}

function stge_get_obj_ypos(i) {
  return Module.ccall('stge_get_obj_ypos', 'number', ['number'], [i]);
}

function stge_get_parsed_data() {
  return Module.ccall('stge_get_parsed_data', 'string', [], []);
}

function stge_get_script(i) {
  return Module.ccall('stge_get_script', 'string', ['number'], [i]);
}

function stge_get_script_count() {
  return Module.ccall('stge_get_script_count', 'number', [], []);
}

function stge_init(wbound, hbound) {
  return Module.ccall('stge_init', 'number', ['number', 'number'], [wbound, hbound]);
}

function stge_parse(code) {
  return Module.ccall('stge_parse', 'number', ['string'], [code]);
}

function stge_run(script) {
  return Module.ccall('stge_run', 'number', ['string'], [script]);
}

function stge_update(dt, x, y) {
  return Module.ccall('stge_update', 'number', ['number', 'number', 'number'], [dt, x, y]);
}
