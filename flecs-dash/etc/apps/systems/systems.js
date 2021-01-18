
Vue.component('app-systems', {
  mounted: function() {
    this.startRequesting("");
  },
  beforeDestroy: function() {
    this.stopRequesting();
  },  
  data: function() {
    return {
      data: {},
      data_request: undefined,
      error: false,
      tick: 0
    }
  },  
  methods: {
    request_data() {
      var url = "metrics?pipeline=yes";

      this.tick ++;

      app.get(url, (msg) => {
        this.data = msg;
        this.error = false;
      }, (Http) => {
        if (Http.status == 404) {
          this.error = true;
        }
      });      
    },
    // Stop periodically requesting the scope
    stopRequesting() {
      this.data = [];
      clearInterval(this.data_request);
    },
    // Start periodically requesting a scope
    startRequesting(scope) {
      this.stopRequesting();

      // Initial request
      this.request_data();

      // Start periodic request
      this.data_request = window.setInterval(function() {
        this.request_data();
      }.bind(this), 1000);
    },
    format_num(val) {
      if (val < 1.0) {
        return val.toFixed(2);
      } else if (val < 10.0) {
        return val.toFixed(1);
      } else {
        return val.toFixed(0);
      }
    },
    to_unit(val) {
      if (val >= 1.0) {
        return this.format_num(val) + "s";
      }

      val *= 1000;
      if (val >= 1.0) {
        return this.format_num(val) + "ms";
      }

      val *= 1000;
      if (val >= 1.0) {
        return this.format_num(val) + "us";
      }

      val *= 1000;
      if (val >= 1.0) {
        return this.format_num(val) + "ns";
      } else {
        return this.format_num(val) + "ns";
      }
    }
  },
  computed: {
    systems() {
      if (!this.data.pipeline) {
        return [];
      }
      return this.data.pipeline.systems;
    },
    fps() {
      if (!this.data.pipeline) {
        return 0;
      }
      return this.data.pipeline.fps.toFixed(1) + "Hz";
    },
    frame_time() {
      if (!this.data.pipeline) {
        return 0;
      }
      return this.to_unit(this.data.pipeline.frame_time_total);
    },
    system_time() {
      if (!this.data.pipeline) {
        return 0;
      }
      return this.to_unit(this.data.pipeline.system_time_total);
    },
    merge_time() {
      if (!this.data.pipeline) {
        return 0;
      }
      return this.to_unit(this.data.pipeline.merge_time_total);
    }
  },
  template: `
    <div class="systems">
      <link rel="stylesheet" href="apps/systems/style.css">

      <div class="metric-row">
        <div class="metric metric-column">
          <div class="metric-header">fps</div>
          <div class="metric-content">
            {{fps}}
          </div>
        </div>  

        <div class="metric metric-column">
          <div class="metric-header">frame time</div>
          <div class="metric-content">
            {{frame_time}}
          </div>
        </div>        

        <div class="metric metric-column">
          <div class="metric-header">system time</div>
          <div class="metric-content">
            {{system_time}}
          </div>
        </div>

        <div class="metric metric-column">
          <div class="metric-header">merge time</div>
          <div class="metric-content">
            {{merge_time}}
          </div>
        </div>   
      </div>

      <div style="position: relative;">
        <div v-for="s in systems">
          <div v-if="s">
            <system :system="s" :system_time="data.pipeline.system_time_total" :tick="tick"></system>
          </div>
          <div v-else class="merge">
            Merge
          </div>
        </div>
      </div>
    </div>
    `
});

app.app_loaded("systems", [{
  name: "system-entity-graph",
  url: "apps/systems/system_entity_graph.js"
}, {
  name: "system-time-graph",
  url: "apps/systems/system_time_graph.js"
}, {
  name: "system",
  url: "apps/systems/system.js"
}, {
  name: "system-component",
  url: "apps/systems/system_component.js"
}]);
