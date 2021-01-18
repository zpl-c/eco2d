
Vue.component('perf-fps', {
  props: ['data'],
  data: function() {
    return {
      last_tick: 0,
      last_fps: 0,
      last_update: 0,
      last_result: 0
    }
  },
  computed: {
    fps() {
      if (!this.data) {
        return 0;
      }

      var d = new Date();
      var t = d.getTime();

      if (t - this.last_update < 1000) {
        return this.last_result;
      }

      this.last_update = t;

      const fps = this.data.data["flecs.dash.monitor.Fps"];
      const tick = this.data.data["flecs.dash.monitor.Tick"];

      let result = (fps.count - this.last_fps) / (tick.count - this.last_tick);

      this.last_fps = fps.count;
      this.last_tick = tick.count;

      if (result < 0) {
        result = 0;
      }

      if (result > 1000000) {
        this.last_result = (result / 1000000).toFixed(2) + "M";
      } else 
      if (result > 1000) {
        this.last_result = (result / 1000).toFixed(2) + "K";
      } else if (result > 100) {
        this.last_result = result.toFixed(0);
      } else {
        this.last_result = result.toFixed(1);
      }

      return this.last_result;
    },
    css() {
      if (!this.data) {
        return "";
      }

      const fps = this.data.data["flecs.dash.monitor.Fps"];
      if (this.fps >= fps.target * 0.9) {
        return "perf-fps-ok";
      }
    }
  },
  template: `
    <div>
      <div class="perf-fps">
        <span :class="'perf-fps-value ' + css">{{ fps }}</span> 
        <span class="perf-label">FPS</span>
      </div>
    </div>
    `
});

Vue.component('perf-load', {
  props: ['data'],
  data: function() {
    return {
      last_total: 0,
      last_frame: 0
    }
  },
  computed: {
    load() {
      if (!this.data) {
        return 100;
      }

      const load = this.data.data["flecs.dash.monitor.Load"];

      let result = (load.frame_time_count - this.last_frame) / (load.total_time_count - this.last_total);
      this.last_frame = load.frame_time_count;
      this.last_total = load.total_time_count;
      
      if (result < 0) {
        result = 0;
      }

      if (!result) {
        result = 1;
      }

      return (result * 100);
    },
    css() {
      return "";
    }
  },
  template: `
    <div :class="'perf-load'">
      <svg width="100px" height="25px">
        <defs>
          <linearGradient id="grad1" x1="0%" y1="0%" :x2="1.0 / (load / 100)" y2="0%">
            <stop offset="0%" style="stop-color:#5BE595;stop-opacity:1" />
            <stop offset="100%" style="stop-color:#4981B5;stop-opacity:1" />
          </linearGradient>
        </defs>
        <rect x="0" y="0" :width="5 + (load * 0.95)" height="25" fill="url(#grad1)" />        
      </svg>
      <span class="perf-label">Load</span>
    </div>
    `
});

Vue.component('perf-clock', {
  props: ['data'],
  methods: {
    digit(num) {
      if (num < 10) {
        return "0" + num;
      } else {
        return num;
      }
    }
  },
  computed: {
    clock() {
      if (!this.data) {
        return "00:00:00.00";
      }

      const clock = this.data.data["flecs.dash.monitor.WorldClock"];

      let hours = Math.floor(clock.world_time / 3600);
      let minutes = Math.floor((clock.world_time - hours * 3600) / 60);
      let seconds = Math.floor(clock.world_time - hours * 3600 - minutes * 60);
      let remainder = Math.floor((clock.world_time - Math.floor(clock.world_time)) * 10);

      return this.digit(hours) + ":" + this.digit(minutes) + ":" + 
        this.digit(seconds) + "." + remainder;
    },
    css() {
      if (!this.data) {
        return "";
      }

      const clock = this.data.data["flecs.dash.monitor.WorldClock"];   
      if (clock.world_time != 0) {
        return "perf-clock-active";
      } else {
        return "";
      }
    }
  },
  template: `
    <div :class="'perf-clock ' + css">
      {{ clock }}
      <img src="images/clock.png">
    </div>
    `
});

Vue.component('perf-summary', {
  props: ['data'],
  template: `
    <div>
      <perf-clock :data="data"></perf-clock>
      <div class="perf-summary">
        <perf-fps :data="data"></perf-fps>
        <perf-load :data="data"></perf-load>
      </div>
    </div>
    `
});
